#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

#define BlockSize 8

__global__ void mandelKernel(float lowerX, float lowerY, float stepX, float stepY, int* img, int resX, int maxIterations, int pitch) {

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    float c_re = lowerX + x * stepX;
    float c_im = lowerY + y * stepY;
    float z_re = c_re, z_im = c_im;

    int i;
    for (i = 0; i < maxIterations; ++i){

        if (z_re * z_re + z_im * z_im > 4.f)
            break;

        float new_re = z_re * z_re - z_im * z_im;
        float new_im = 2.f * z_re * z_im;
        z_re = c_re + new_re;
        z_im = c_im + new_im;
    }
    int* rowHead = (int*)((char*)img + y * pitch);
    rowHead[x] = i;
}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;

    size_t size = sizeof(int) * resX * resY;
    size_t widthByte = sizeof(int) * resX;
    size_t height = resY;
    size_t pitch;
    int* output;
    int* d_img;
    
    cudaHostAlloc (&output, size, cudaHostAllocDefault);
    cudaMallocPitch(&d_img, &pitch, widthByte, height);

    dim3 threadsPerBlock(BlockSize, BlockSize);
    dim3 numBlocks(resX / threadsPerBlock.x, resY / threadsPerBlock.y);
    mandelKernel<<<numBlocks, threadsPerBlock>>>(lowerX, lowerY, stepX, stepY, d_img, resX, maxIterations, pitch);

    cudaMemcpy2D(output, widthByte, d_img, pitch, widthByte, height, cudaMemcpyDeviceToHost);
    memcpy(img, output, size);

    cudaFree(d_img);
    cudaFreeHost(output);
}