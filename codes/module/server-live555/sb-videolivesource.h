/*
 * Copyright (c) 2013-2014 Chun-Ying Huang
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

#ifndef __SB_VIDEOLIVESOURCE_H__
#define __SB_VIDEOLIVESOURCE_H__

#include <FramedSource.hh>
#include "core-module.h"

class SBVideoLiveSource : public FramedSource {
public:
	static SBVideoLiveSource * createNew(UsageEnvironment& env, int cid/* TODO: more params */);
	//static EventTriggerId eventTriggerId;
protected:
	SBVideoLiveSource(UsageEnvironment& env, int cid);
	~SBVideoLiveSource();
private:
	static unsigned referenceCount;
	static int remove_startcode;
	static sb_module_t *m;
	int channelId;
	//
	static void deliverFrame0(void* clientData);
	void doGetNextFrame();
	//virtual void doStopGettingFrames(); // optional
	void deliverFrame();
};

#endif /* __SB_VIDEOLIVESOURCE_H__ */
