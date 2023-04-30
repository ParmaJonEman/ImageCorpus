// ImageBrowser.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <opencv2/core/utility.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace std;

static vector<string> list_directory(const char* dirname, String outdir);
static int parseParameters(int argc, char** argv, String* directory, int* rows, int* cols, int* preserveAspectRatio, int* greyScale, String* outputType, String* outdir);
static int processImages(vector<string> vectorOfFiles, int rows, int columns, int preserveAspectRatio, int greyScale, String outputType, String outdir, String indir);
static Mat scaleImage(Mat sourceImage, int rows, int columns, int preserveAspectRatio);

