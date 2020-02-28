#pragma once

/* Time.h
	Enterprise's Time system.  Use Time to track frame deltas, set timers, and measure performance.
*/

#include "Enterprise/Application/Application.h"
namespace Enterprise {
	class Time
	{
	public:
		static float DeltaTime(); // Returns the time that has passed between the previous frame and the current one.
		static float SimPhase(); // Returns a unit interval representing where in a SimStep the current frame falls.

		//static void SetTimer(float duration, callbackPtr callback); // Sets a timer in game time.  At the end of the specified time period, generates event.
		//static void SetRealTimer(float duration, callbackPtr callback); // Sets a timer in real time (does not reflect time dilation).

		//static void DilateTime(float scalar); // Applies the provided scalar to the game time immediately.
		//static void DilateTime_Lerp(float scalar, float lerpTime);
		//static void DilateTime_Exponential(float scalar, float lerpTime);
		//static void DilateTime_Logarithmic(float scalar, float lerpTime);
		//static void DilateTime_SmoothStep(float scalar, float lerpTime);

	private:
		// Member time variables
		static float m_deltaTime, m_simPhase;
		// Time variables are set in Application::FrameStep exclusively
		friend bool Application::FrameStep(float, float);
		friend void Application::SimStep();
	};
}