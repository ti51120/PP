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
					float          *outputImage,
                    const int      width,
                    const int      height,
					const int      start,
					const int      total);

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

	int total = args->height / 2;
	if(args->threadId == 0){
		int start = 0;
		canny_edge_detection(args->in, args->output, args->width, args->height, start, total);
	}
	else{
		int start = args->height / 2;
		canny_edge_detection(args->in, args->output, args->width, args->height, start, total);
		// printf("thread ID %d not working...\n", args->threadId);
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
					float          *outputImage, 
					const int 	   numThreads,
                    const int      width, 
                    const int      height)
{
	// canny_edge_detection(in, outputImage, width, height);
	static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i = 0; i < numThreads; i++)
    {
		args[i].in = in;
        args[i].output = outputImage;
        args[i].width = width;
        args[i].height = height;
        args[i].numThreads = numThreads;
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
}
