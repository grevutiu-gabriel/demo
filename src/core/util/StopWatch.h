//
//
// This is a very simple timer class based on timeGetTime() c function.
// http://tdistler.com/2010/06/27/high-performance-timing-on-linux-windows
//
//
#pragma once

#ifdef LINUX
#include <unistd.h>
#endif
#ifdef _WINDOWS
#include <windows.h>
#include <mmsystem.h>
#endif

namespace base
{
	//
	// This class is able to measure elapsed time from a certain start point
	//
	class StopWatch
	{
	public:
		StopWatch();
		~StopWatch();
		void start();
		void stop();
		void reset();
		float elapsedSeconds();
		bool isRunning()const;

	private:
		bool                  m_running;
		#ifdef _WINDOWS
		unsigned long           m_time0; // time in ms
		#endif
		#ifdef linux
		float                   m_time0; // time in s
		#endif
		float                 m_elapsed; // time in s
		float               m_frequency;
	};


	inline void sleep( int sleepMs )
	{
	#ifdef LINUX
		usleep(sleepMs * 1000);   // usleep takes sleep time in us
	#endif
	#ifdef _WINDOWS
		Sleep(sleepMs);
	#endif
	}

}
