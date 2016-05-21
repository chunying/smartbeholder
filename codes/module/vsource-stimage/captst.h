#include <ctime>
#include <iostream>
#include <string.h>
#include <raspicam/raspicam_cv.h>
#include "portPi.h"
#include <opencv2/stitching/stitcher.hpp>
#include <opencv/highgui.h>
#include "core-common.h"
#include <cv.h>
#define CameraNum 3

int captst_init();
void captst_perform(char *buf, int buflen);
void captst_deinit();
