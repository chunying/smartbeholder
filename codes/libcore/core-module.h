/*
 * Copyright (c) 2013 Chun-Ying Huang
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

/**
 * @file
 * Common functions and definitions for implementing a Smart Beholder module: headers.
 */

#ifndef __SB_MODULE__
#define __SB_MODULE__

#include "core-common.h"

#ifndef WIN32
typedef void * HMODULE;
#endif

/**
 * Enumeration for types of a module.
 */
enum sb_module_types {
	SB_MODULE_TYPE_NULL = 0,	/**< Not used */
	SB_MODULE_TYPE_CONTROL,		/**< Is a controller module */
	SB_MODULE_TYPE_ASOURCE,		/**< Is an audio source module */
	SB_MODULE_TYPE_VSOURCE,		/**< Is an video source module */
	SB_MODULE_TYPE_FILTER,		/**< Is a filter module */
	SB_MODULE_TYPE_AENCODER,	/**< Is an audio encoder module */
	SB_MODULE_TYPE_VENCODER,	/**< Is a video encoder module */
	SB_MODULE_TYPE_ADECODER,	/**< Is an audio decoder module */
	SB_MODULE_TYPE_VDECODER,	/**< Is a video decoder module */
	SB_MODULE_TYPE_SERVER		/**< Is a server module */
};

/**
 * Enumeration for module ioctl() commands.
 */
enum sb_ioctl_commands {
	SB_IOCTL_NULL = 0,		/**< Not used */
	SB_IOCTL_RECONFIGURE,		/**< Reconfiguration */
	SB_IOCTL_GETSPS = 0x100,	/**< Get SPS: for H.264 and H.265 */
	SB_IOCTL_GETPPS,		/**< Get PPS: for H.264 and H.265 */
	SB_IOCTL_GETVPS,		/**< Get VPS: for H.265 */
	SB_IOCTL_CUSTOM = 0x40000000	/**< For user customization */
};

/**
 * GamingAnyhwere ioctl() error codes.
 */
#define	SB_IOCTL_ERR_NONE		0	/**< No error */
#define	SB_IOCTL_ERR_GENERAL		-1	/**< General error */
#define	SB_IOCTL_ERR_NULLMODULE		-2	/**< Module is NULL */
#define	SB_IOCTL_ERR_NOIOCTL		-3	/**< ioctl() is not implemented */
#define	SB_IOCTL_ERR_NOINTERFACE	-3	/**< ioctl() or interface is not implemented */
#define	SB_IOCTL_ERR_NOTINITIALIZED	-4	/**< Module has not been initialized */
#define	SB_IOCTL_ERR_NOTSUPPORTED	-5	/**< Command is not supported */
#define	SB_IOCTL_ERR_INVALID_ARGUMENT	-6	/**< Invalid argument */
#define	SB_IOCTL_ERR_NOTFOUND		-7	/**< Not found */
#define	SB_IOCTL_ERR_BUFFERSIZE		-8	/**< Buffer error */
#define	SB_IOCTL_ERR_BADID		-9	/**< Invalid Id */
#define	SB_IOCTL_ERR_NOMEM		-10	/**< No memory */

/**
 * Parameter for ioctl()'s codec GET SPS/PPS/VPS command.
 */
typedef struct sb_ioctl_buffer_s {
	int id;
	unsigned char *ptr;	/**< Pointer to the buffer */
	int size;		/**< Size of the buffer */
}	sb_ioctl_buffer_t;

/**
 * Parameter for ioctl()'s codec reconfiguration command.
 */
typedef struct sb_ioctl_reconfigure_s {
	int id;
	int crf;		/**< Constant rate factor */
	int framerate_n;	/**< Framerate numerator */
	int framerate_d;	/**< Framerate denominator */
	int bitrateKbps;	/**< bitrate in Kbit-per-second. Affects both bitrate and vbv-maxrate */
	int bufsize;		/**< vbv-bufsize */
	int width;		/**< Width */
	int height;		/**< Height */
}	sb_ioctl_reconfigure_t;

#ifdef __cplusplus
extern "C" {
#endif
//////////////////////////////////////////////
/**
 * Data strucure to represent a module.
 */
typedef struct sb_module_s {
	HMODULE	handle;		/**< Handle to a module */
	int type;		/**< Type of the module */
	char *name;		/**< Name of the module */
	char *mimetype;		/**< MIME-type of the module */
	int (*init)(void *arg);		/**< Pointer to the init function */
	int (*start)(void *arg);	/**< Pointer to the start function */
	//void * (*threadproc)(void *arg);
	int (*stop)(void *arg);		/**< Pointer to the stop function */
	int (*deinit)(void *arg);	/**< Pointer to the deinit function */
	int (*ioctl)(int command, int argsize, void *arg);	/**< Pointer to ioctl function */
	int (*notify)(void *arg);	/**< Pointer to the notify function */
	void * (*raw)(void *arg, int *size);	/**< Pointer to the raw function */
	int (*send_packet)(const char *prefix, int channelId, AVPacket *pkt, int64_t encoderPts, struct timeval *ptv);	/**< Pointer to the send packet function: sink only */
	void * privdata;		/**< Private data of this module */
}	sb_module_t;
//////////////////////////////////////////////
#ifdef __cplusplus
}
#endif

#define	MODULE	extern "C"	/**< Module interface is pure-C implementation: Export C symbols */
#if defined WIN32 && defined SB_MODULE
#define	MODULE_EXPORT	__declspec(dllexport)
#else
#define	MODULE_EXPORT	/**< MODULE_EXPORT is not unsed in UNIX-like environment */
#endif

EXPORT sb_module_t * sb_load_module(const char *modname, const char *prefix);
EXPORT void sb_unload_module(sb_module_t *m);
EXPORT int sb_init_single_module(const char *name, sb_module_t *m, void *arg);
EXPORT void sb_init_single_module_or_quit(const char *name, sb_module_t *m, void *arg);
EXPORT int sb_run_single_module(const char *name, void * (*threadproc)(void*), void *arg);
EXPORT void sb_run_single_module_or_quit(const char *name, void * (*threadproc)(void*), void *arg);
// module function wrappers
EXPORT int sb_module_init(sb_module_t *m, void *arg);
EXPORT int sb_module_start(sb_module_t *m, void *arg);
EXPORT int sb_module_stop(sb_module_t *m, void *arg);
EXPORT int sb_module_deinit(sb_module_t *m, void *arg);
EXPORT int sb_module_ioctl(sb_module_t *m, int command, int argsize, void *arg);
EXPORT int sb_module_notify(sb_module_t *m, void *arg);
EXPORT void * sb_module_raw(sb_module_t *m, void *arg, int *size);
EXPORT int sb_module_send_packet(sb_module_t *m, const char *prefix, int channelId, AVPacket *pkt, int64_t encoderPts, struct timeval *ptv);

#ifdef SB_MODULE
// a module must have exported the module_load function
MODULE MODULE_EXPORT sb_module_t * module_load();
#endif

#endif /* __SB_MODULE__ */
