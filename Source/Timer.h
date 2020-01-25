#pragma once

#include "SDL\include\SDL.h"

class Timer
{
public:
	Timer();

	void Start();
	void Stop();

	Uint32 Read();

private:
	bool	running;
	Uint32	started_at;
	Uint32	stopped_at;
};