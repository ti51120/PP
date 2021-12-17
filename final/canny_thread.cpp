#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <thread>

#define MAX_BRIGHTNESS 255
#define CANNY_LOWER 45
#define CANNY_UPPER 50
#define CANNY_SIGMA 1.0

typedef unsigned char uchar;
typedef float pixel_t;

typedef struct
{
    unsigned int width;
    unsigned int height;
	float *in;
    pixel_t *output;
    int threadId;
    int numThreads;
} WorkerArgs;

extern float *canny_edge_detection(
                    const float    *in,
                    const int      width,
                    const int      height);

// /*
//  * If normalize is true, then map pixels to range 0 -> MAX_BRIGHTNESS.
//  */
// static void
// convolution(const pixel_t *in,
//             pixel_t       *out,
//             const float   *kernel,
//             const int      nx,
//             const int      ny,
//             const int      kn,
//             const bool     normalize)
// {
// 	const int khalf = kn / 2;
// 	float min = 0.5;
// 	float max = 254.5;
// 	float pixel = 0.0;
// 	size_t c = 0;
// 	int m, n, i, j;

// 	assert(kn % 2 == 1);
// 	assert(nx > kn && ny > kn);

// 	for (m = khalf; m < nx - khalf; m++) {
// 		for (n = khalf; n < ny - khalf; n++) {
// 			pixel = c = 0;

// 			for (j = -khalf; j <= khalf; j++)
// 				for (i = -khalf; i <= khalf; i++)
// 					pixel += in[(n - j) * nx + m - i] * kernel[c++];

// 			if (normalize == true)
// 				pixel = MAX_BRIGHTNESS * (pixel - min) / (max - min);

// 			out[n * nx + m] = (pixel_t) pixel;
// 		}
// 	}
// }

// /*
//  * gaussianFilter: http://www.songho.ca/dsp/cannyedge/cannyedge.html
//  * Determine the size of kernel (odd #)
//  * 0.0 <= sigma < 0.5 : 3
//  * 0.5 <= sigma < 1.0 : 5
//  * 1.0 <= sigma < 1.5 : 7
//  * 1.5 <= sigma < 2.0 : 9
//  * 2.0 <= sigma < 2.5 : 11
//  * 2.5 <= sigma < 3.0 : 13 ...
//  * kernel size = 2 * int(2 * sigma) + 3;
//  */
// static void
// gaussian_filter(const pixel_t *in,
//                 pixel_t       *out,
//                 const int      nx,
//                 const int      ny,
//                 const float    sigma)
// {
// 	const int n = 2 * (int) (2 * sigma) + 3;
// 	const float mean = (float) floor(n / 2.0);
// 	float kernel[n * n];
// 	int i, j;
// 	size_t c = 0;

// 	for (i = 0; i < n; i++) {
// 		for (j = 0; j < n; j++)
// 			kernel[c++] = exp(-0.5 * (pow((i - mean) / sigma, 2.0) + pow((j - mean) / sigma, 2.0))) / (2 * M_PI * sigma * sigma);
// 	}

// 	convolution(in, out, kernel, nx, ny, n, true);
// }

void workerThreadStart(WorkerArgs *const args){

	if(args->threadId == 1){
		args->output = canny_edge_detection(args->in, args->width, args->height);
	}
	else{
		printf("thread ID %d not working...\n", args->threadId);
	}

}

/*
 * Links:
 * http://en.wikipedia.org/wiki/Canny_edge_detector
 * http://www.tomgibara.com/computer-vision/CannyEdgeDetector.java
 * http://fourier.eng.hmc.edu/e161/lectures/canny/node1.html
 * http://www.songho.ca/dsp/cannyedge/cannyedge.html
 */

float *canny_edge_detection_thread(
					float    	   *in, 
					const int 	   numThreads,
                    const int      width, 
                    const int      height)
{

	// return canny_edge_detection(in, width, height);
	static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

	pixel_t *output = (pixel_t*)malloc(width * height * sizeof(pixel_t)); //

    for (int i = 0; i < numThreads; i++)
    {
        args[i].width = width;
        args[i].height = height;
        args[i].numThreads = numThreads;
        args[i].output = output;
		args[i].in = in;

        args[i].threadId = i;
    }

    for (int i = 1; i < numThreads; i++)
    {
        workers[i] = std::thread(workerThreadStart, &args[i]);
    }

    workerThreadStart(&args[0]);

    // join worker threads
    for (int i = 1; i < numThreads; i++)
    {
        workers[i].join();
    }

	float *retval = (float*)malloc(width * height * sizeof(float));

	/* Convert back to float */
	for (int i = 0; i < width * height; i++) {
		retval[i] = (float)output[i];
	}

	return retval;
	// int i, j, k, nedges;
	// int *edges;
	// size_t t = 1;
	// float *retval;

	// const float Gx[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
	// const float Gy[] = {1, 2, 1, 0, 0, 0, -1, -2, -1};

	// pixel_t *G = (pixel_t*)calloc(width * height * sizeof(pixel_t), 1);

	// pixel_t *after_Gx = (pixel_t*)calloc(width * height * sizeof(pixel_t), 1);

	// pixel_t *after_Gy = (pixel_t*)calloc(width * height * sizeof(pixel_t), 1);

	// pixel_t *nms = (pixel_t*)calloc(width * height * sizeof(pixel_t), 1);


	// pixel_t *pixels = (pixel_t*)malloc(width * height * sizeof(pixel_t));

	// /* Convert to pixel_t. */
	// for (i = 0; i < width * height; i++) {
	// 	pixels[i] = (pixel_t)in[i];
	// }

	// gaussian_filter(pixels, out, width, height, CANNY_SIGMA);

	// convolution(out, after_Gx, Gx, width, height, 3, false);

	// convolution(out, after_Gy, Gy, width, height, 3, false);

	// for (i = 1; i < width - 1; i++) {
	// 	for (j = 1; j < height - 1; j++) {
	// 		const int c = i + width * j;
	// 		G[c] = (pixel_t)hypot(after_Gx[c], after_Gy[c]);
	// 	}
	// }

	// /* Non-maximum suppression, straightforward implementation. */
	// for (i = 1; i < width - 1; i++) {
	// 	for (j = 1; j < height - 1; j++) {
	// 		const int c = i + width * j;
	// 		const int nn = c - width;
	// 		const int ss = c + width;
	// 		const int ww = c + 1;
	// 		const int ee = c - 1;
	// 		const int nw = nn + 1;
	// 		const int ne = nn - 1;
	// 		const int sw = ss + 1;
	// 		const int se = ss - 1;
	// 		const float dir = (float) (fmod(atan2(after_Gy[c], after_Gx[c]) + M_PI, M_PI) / M_PI) * 8;

	// 		if (((dir <= 1 || dir > 7) && G[c] > G[ee] && G[c] > G[ww]) || // 0 deg
	// 			((dir > 1 && dir <= 3) && G[c] > G[nw] && G[c] > G[se]) || // 45 deg
	// 			((dir > 3 && dir <= 5) && G[c] > G[nn] && G[c] > G[ss]) || // 90 deg
	// 			((dir > 5 && dir <= 7) && G[c] > G[ne] && G[c] > G[sw]))   // 135 deg
	// 			nms[c] = G[c];
	// 		else
	// 			nms[c] = 0;
	// 	}
	// }

	// /* Reuse the array used as a stack, width * height / 2 elements should be enough. */
	// edges = (int *) after_Gy;
	// memset(out, 0, sizeof(pixel_t) * width * height);
	// memset(edges, 0, sizeof(pixel_t) * width * height);

	// /* Tracing edges with hysteresis. Non-recursive implementation. */
	// for (j = 1; j < height - 1; j++) {
	// 	for (i = 1; i < width - 1; i++) {
	// 		/* Trace edges. */
	// 		if (nms[t] >= CANNY_UPPER && out[t] == 0) {
	// 			out[t] = MAX_BRIGHTNESS;
	// 			nedges = 1;
	// 			edges[0] = t;

	// 			do {
	// 				nedges--;
	// 				const int e = edges[nedges];

	// 				int nbs[8]; // neighbours
	// 				nbs[0] = e - width;     // nn
	// 				nbs[1] = e + width;     // ss
	// 				nbs[2] = e + 1;      // ww
	// 				nbs[3] = e - 1;      // ee
	// 				nbs[4] = nbs[0] + 1; // nw
	// 				nbs[5] = nbs[0] - 1; // ne
	// 				nbs[6] = nbs[1] + 1; // sw
	// 				nbs[7] = nbs[1] - 1; // se

	// 				for (k = 0; k < 8; k++) {
	// 					if (nms[nbs[k]] >= CANNY_LOWER && out[nbs[k]] == 0) {
	// 						out[nbs[k]] = MAX_BRIGHTNESS;
	// 						edges[nedges] = nbs[k];
	// 						nedges++;
	// 					}
	// 				}
	// 			}while (nedges > 0);
	// 		}
	// 		t++;
	// 	}
	// }

	// retval = (float*)malloc(width * height * sizeof(float));

	// /* Convert back to float */
	// for (i = 0; i < width * height; i++) {
	// 	retval[i] = (float)out[i];
	// }

	// free(after_Gx);
	// free(after_Gy);
	// free(G);
	// free(nms);
	// free(pixels);
	// free(out);

	// return retval;
}
