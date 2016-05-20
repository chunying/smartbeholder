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

#ifndef __SB_AUDIOLIVESOURCE_H__
#define __SB_AUDIOLIVESOURCE_H__

#include <FramedSource.hh>

class SBAudioLiveSource : public FramedSource {
public:
	static SBAudioLiveSource * createNew(UsageEnvironment& env, int cid/* TODO: more params */);
	static EventTriggerId eventTriggerId;
protected:
	SBAudioLiveSource(UsageEnvironment& env, int cid);
	~SBAudioLiveSource();
private:
	static unsigned referenceCount;
	int channelId;
	//
	static void deliverFrame0(void* clientData);
	void doGetNextFrame();
	//virtual void doStopGettingFrames(); // optional
	void deliverFrame();
};

void signalNewAudioFrameData();

#endif /* __SB_AUDIOLIVESOURCE_H__ */
