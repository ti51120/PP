// #include <cuda.h>
// #include <stdio.h>
// #include <stdlib.h>

// __global__ void mandelKernel() {
//     // To avoid error caused by the floating number, use the following pseudo code
//     //
//     // float x = lowerX + x * stepX;
//     // float y = lowerY + y * stepY;
// }

// // Host front-end function that allocates the memory and launches the GPU kernel
// void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
// {
//     float stepX = (upperX - lowerX) / resX;
//     float stepY = (upperY - lowerY) / resY;
// }

#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>

#define N 16

__global__ void mandelKernel(float lowerX, float lowerY, float stepX, float stepY, int* img, int resX, int maxIterations) {

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
    img[x + y * resX] = i;

}

// Host front-end function that allocates the memory and launches the GPU kernel
void hostFE (float upperX, float upperY, float lowerX, float lowerY, int* img, int resX, int resY, int maxIterations)
{
    float stepX = (upperX - lowerX) / resX;
    float stepY = (upperY - lowerY) / resY;

    size_t size = sizeof(int) * resX * resY;
    int* output;
    int* d_img;
    // output =  (int*)malloc(size);
    cudaHostAlloc (&output, size, cudaHostAllocDefault);
    cudaMalloc(&d_img, size);

    dim3 threadsPerBlock(N, N);
    dim3 numBlocks(resX / threadsPerBlock.x, resY / threadsPerBlock.y);

    mandelKernel<<<numBlocks, threadsPerBlock>>>(lowerX, lowerY, stepX, stepY, d_img, resX, maxIterations);

    cudaMemcpy(output, d_img, size, cudaMemcpyDeviceToHost);
    memcpy(img, output, size);

    cudaFree(d_img);
    cudaFreeHost(output);
}
