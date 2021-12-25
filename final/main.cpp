#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include "../common/bmpfuncs.h"
#include "../common/CycleTimer.h"

typedef unsigned char uchar;
typedef float pixel_t;

/*
 * Links:
 * http://en.wikipedia.org/wiki/Canny_edge_detector
 * http://www.tomgibara.com/computer-vision/CannyEdgeDetector.java
 * http://fourier.eng.hmc.edu/e161/lectures/canny/node1.html
 * http://www.songho.ca/dsp/cannyedge/cannyedge.html
 *
 * Note: T1 and T2 are lower and upper thresholds.
 */

extern float *canny_edge_detection(
                    const float    *in,
					float 		   *outputImage,
                    const int      width,
                    const int      height,
					const int      start,
					const int      total);

extern float *canny_edge_detection_thread(
                    float 	       *in,
					float 		   *outputImage,
                    const int      thread,
                    const int      width,
                    const int      height);

int main(void){
	const char *inputFile = "./common/input.bmp";
	const char *outputFile_serial = "output_serial.bmp";
	const char *outputFile_thread = "output_thread.bmp";
	int imageWidth, imageHeight;
	float *inputImage = readImage(inputFile, &imageWidth, &imageHeight);
	int dataSize = imageWidth * imageHeight * sizeof(float);
	float *outputImage = (float*)malloc(dataSize);

	double start_time, end_time;
	double timer = 0;
	double record[10] = {0};

    // serial version
    double minSerial = 1e30;
	for (int i = 0; i < 10; i++){
		memset(outputImage, 0, dataSize);
		start_time = CycleTimer::currentSeconds();
		canny_edge_detection(inputImage, outputImage, imageWidth, imageHeight, 0, imageHeight);
		end_time = CycleTimer::currentSeconds();
        minSerial = std::min(minSerial, end_time - start_time);
	}

	printf("Serial version\n[execution time]: \t\t[%.3f] ms\n\n", minSerial * 1000);

	storeImage(outputImage, outputFile_serial, imageHeight, imageWidth, inputFile);

    // thread version
    int threads = 2;
    double minThread = 1e30;
	for (int i = 0; i < 10; i++){
		memset(outputImage, 0, dataSize);
		start_time = CycleTimer::currentSeconds();
		canny_edge_detection_thread(inputImage, outputImage, threads, imageWidth, imageHeight);
		end_time = CycleTimer::currentSeconds();
        minThread = std::min(minThread, end_time - start_time);
	}

    printf("Thread version\n[execution time]: \t\t[%.3f] ms\n\n", minThread * 1000);

	storeImage(outputImage, outputFile_thread, imageHeight, imageWidth, inputFile);
	
    free(outputImage);
}
