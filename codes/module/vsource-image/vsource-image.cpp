/*
 * Copyright (c) 2013-2016 Chun-Ying Huang
 *
 * This file is part of Smart Beholder (SB).
 *
 * SB is free software; you can redistribute it and/or modify it
 * under the terms of the 3-clause BSD License as published by the
 * Free Software Foundation: http://directory.fsf.org/wiki/License:BSD_3Clause
 *
 * SB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the 3-clause BSD License along with SB;
 * if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <pthread.h>
#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#endif

#include <map>

#include "vsource.h"
#include "dpipe.h"
#include "encoder-common.h"
#include "core-conf.h"
#include "core-common.h"
#include "core-avcodec.h"

using namespace std;

static int image_width;
static int image_height;
static int image_fps = 24;
static unsigned int image_size = 0;
static char *image_buf = NULL;

static int vsource_initialized = 0;
static int vsource_started = 0;
static pthread_t vsource_tid;

/* support reconfiguration of frame rate */
static int vsource_framerate_n = -1;
static int vsource_framerate_d = -1;
static int vsource_reconfigured = 0;

/* video source has to send images to video-# pipes */
/* the format is defined in VIDEO_SOURCE_PIPEFORMAT */

/*
 * vsource_init(void *arg)
 * arg is a pointer to a gaRect (if cropping is enabled)
 */
static int
vsource_init(void *arg) {
	int width, height, fps;
	unsigned int size;
	char filename[1024];
	FILE *fp;
	//
	if(vsource_initialized != 0)
		return 0;
	// load configuration
	if((sb_conf_readv("image-source-file",
			filename, sizeof(filename))) == NULL) {
		sb_error("image-source: no filename given\n");
		return -1;
	}
	if((width = sb_conf_readint("image-source-width")) <= 0) {
		sb_error("image-source: no width given\n");
		return -1;
	}
	if((height = sb_conf_readint("image-source-height")) <= 0) {
		sb_error("image-source: no height given\n");
		return -1;
	}
	if((fps = sb_conf_readint("image-source-fps")) <= 0) {
		fps = image_fps;
	}
	// load the file
	size = width * height * 3 / 2;
	if((image_buf = (char*) malloc(size)) == NULL) {
		sb_error("image-source: memory allocation failed.\n");
		return -1;
	}
	if((fp = fopen(filename, "rb")) == NULL) {
		free(image_buf);
		sb_error("image-source: open %s failed.\n", filename);
		return -1;
	}
	if(fread(image_buf, sizeof(char), size, fp) != size) {
		sb_error("image-source: load image failed.\n");
		return -1;
	}
	fclose(fp);
	/* create image source, with sufficient per-frame size
	   the actual size for YUV is W*H*3/2,
	   but we use a large one: W*H*4  */
	if(video_source_setup(width, height, width * 4) < 0) {
		return -1;
	}
	//
	image_width = width;
	image_height = height;
	image_size = size;
	image_fps = fps;
	vsource_initialized = 1;
	return 0;
}

/*
 * vsource_threadproc accepts no arguments
 */
static void *
vsource_threadproc(void *arg) {
	int token;
	int frame_interval;
	dpipe_buffer_t *data;
	vsource_frame_t *frame;
	char pipename[64];
	dpipe_t *pipe[1];
	struct timeval initialTv, lastTv, captureTv;
	// reset framerate setup
	vsource_framerate_n = image_fps;
	vsource_framerate_d = 1;
	vsource_reconfigured = 0;
	//
	frame_interval = 1000000/image_fps;	// in the unif of us
	frame_interval++;
	//
	snprintf(pipename, sizeof(pipename), VIDEO_SOURCE_PIPEFORMAT, 0);
	if((pipe[0] = dpipe_lookup(pipename)) == NULL) {
		sb_error("video source: cannot find pipeline '%s'\n", pipename);
		exit(-1);
	}
	//
	sb_error("video source thread started: tid=%ld\n", sb_gettid());
	gettimeofday(&initialTv, NULL);
	lastTv = initialTv;
	token = frame_interval;
	while(vsource_started != 0) {
		// encoder has not launched?
		if(encoder_running() == 0) {
#ifdef WIN32
			Sleep(1);
#else
			usleep(1000);
#endif
			gettimeofday(&lastTv, NULL);
			token = frame_interval;
			continue;
		}
		// token bucket based capturing
		gettimeofday(&captureTv, NULL);
		token += tvdiff_us(&captureTv, &lastTv);
		if(token > (frame_interval<<1)) {
			token = (frame_interval<<1);
		}
		lastTv = captureTv;
		//
		if(token < frame_interval) {
#ifdef WIN32
			Sleep(1);
#else
			usleep(1000);
#endif
			continue;
		}
		token -= frame_interval;
		// copy image 
		data = dpipe_get(pipe[0]);
		frame = (vsource_frame_t*) data->pointer;
		frame->pixelformat = AV_PIX_FMT_YUV420P;
		////////////////////////////////////////
		frame->realwidth = image_width;
		frame->realheight = image_height;
		frame->realstride = image_width;
		frame->realsize = image_size;
		//
		frame->linesize[0] = image_width;
		frame->linesize[1] = image_width / 2;
		frame->linesize[2] = image_width / 2;
		bcopy(image_buf, frame->imgbuf, image_size);
		//gImgPts++;
		frame->imgpts = tvdiff_us(&captureTv, &initialTv)/frame_interval;
		frame->timestamp = captureTv;
		dpipe_store(pipe[0], data);
		// reconfigured?
		if(vsource_reconfigured != 0) {
			frame_interval = (int) (1000000.0 * vsource_framerate_d / vsource_framerate_n);
			frame_interval++;
			vsource_reconfigured = 0;
			sb_error("video source: reconfigured - framerate=%d/%d (interval=%d)\n",
				vsource_framerate_n, vsource_framerate_d, frame_interval);
		}
	}
	//
	sb_error("video source: thread terminated.\n");
	//
	return NULL;
}

static int
vsource_deinit(void *arg) {
	if(vsource_initialized == 0)
		return 0;
	if(image_buf)
		free(image_buf);
	image_buf = NULL;
	vsource_initialized = 0;
	return 0;
}

static int
vsource_start(void *arg) {
	if(vsource_started != 0)
		return 0;
	vsource_started = 1;
	if(pthread_create(&vsource_tid, NULL, vsource_threadproc, arg) != 0) {
		vsource_started = 0;
		sb_error("video source: create thread failed.\n");
		return -1;
	}
	pthread_detach(vsource_tid);
	return 0;
}

static int
vsource_stop(void *arg) {
	if(vsource_started == 0)
		return 0;
	vsource_started = 0;
	pthread_cancel(vsource_tid);
	return 0;
}

static int
vsource_ioctl(int command, int argsize, void *arg) {
	int ret = 0;
	sb_ioctl_reconfigure_t *reconf = (sb_ioctl_reconfigure_t*) arg;
	//
	if(vsource_initialized == 0)
		return SB_IOCTL_ERR_NOTINITIALIZED;
	//
	switch(command) {
	case SB_IOCTL_RECONFIGURE:
		if(argsize != sizeof(sb_ioctl_reconfigure_t))
			return SB_IOCTL_ERR_INVALID_ARGUMENT;
		if(reconf->framerate_n > 0 && reconf->framerate_d > 0) {
			double framerate;
			if(vsource_framerate_n == reconf->framerate_n
			&& vsource_framerate_d == reconf->framerate_d)
				break;
			framerate = 1.0 * reconf->framerate_n / reconf->framerate_d;
			if(framerate < 2 || framerate > 120) {
				return SB_IOCTL_ERR_INVALID_ARGUMENT;
			}
			vsource_framerate_n = reconf->framerate_n;
			vsource_framerate_d = reconf->framerate_d;
			vsource_reconfigured = 1;
		}
		break;
	default:
		ret = SB_IOCTL_ERR_NOTSUPPORTED;
		break;
	}
	return ret;
}

sb_module_t *
module_load() {
	static sb_module_t m;
	bzero(&m, sizeof(m));
	m.type = SB_MODULE_TYPE_VSOURCE;
	m.name = strdup("vsource-image");
	m.init = vsource_init;
	m.start = vsource_start;
	m.stop = vsource_stop;
	m.deinit = vsource_deinit;
	m.ioctl = vsource_ioctl;
	return &m;
}

