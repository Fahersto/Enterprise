#include "EP_PCH.h"
#include "Enterprise/Time/Time.h"

// Length, in game-world seconds, each PhysFrame represents.
#define PHYSFRAMELENGTH (1.0f / 50.0f) //TODO: Set in client
// Longest amount of time we're allowed to call PhysFrames between Frames.
#define PHYSREPEATCAP (3.0f * PHYSFRAMELENGTH) //TODO: Set in client

// Tick vars
float runningTime = 0.0f; // Time, in real seconds, since the application started.
float prevTime = 0.0f; // The previous value of runningTime.
float timeScale = 1.0f; // The current conversion rate betwee real seconds and game seconds.
float tickDeltaReal = 0.0f; // The amount of real seconds that have passed since the previous Tick().
float tickDeltaScaled = 0.0f; // The amount of game-time seconds that have passed since the previous Tick().

// Accumulators
float frameAccumulator = 0.0f; float physFrameAccumulator = PHYSFRAMELENGTH; float physFrameRepeatAccumulator = 0.0f;

// Exposed vars
float frameDelta = 0.0f;  // The number of game-time seconds being simulated this frame or physics frame.
float physPhase = 1.0f; // A value in [0,1) representing progress through the current physics frame.

namespace Enterprise 
{
	// Getters ----------------------------------
	float Time::RunningTime() { return runningTime; }
	float Time::FrameDelta() { return frameDelta; }
	float Time::PhysPhase() { return physPhase; } //TODO: Prevent use in PhysUpdate().

	// Setters ----------------------------------
	void Time::SetTimeScale(float scalar)
	{
		EP_ASSERT(scalar >= 0.0f); //Time::SetTimeScale() called with negative parameter.  Scalar cannot be negative.  Set timeScale to 0.0f.
		timeScale = scalar;
	}
	// ------------------------------------------

	void Time::Tick()
	{
		prevTime = runningTime;
		runningTime = GetRawTime();
		tickDeltaReal = runningTime - prevTime;
		tickDeltaScaled = tickDeltaReal * timeScale;

		// Increment accumulators
		frameAccumulator += tickDeltaScaled;
		physFrameAccumulator += tickDeltaScaled;
		physFrameRepeatAccumulator += tickDeltaReal;

		// Perhaps here is the time to increment timers?
	}

	bool Time::PhysFrame()
	{
		Tick();

		// Abort death spirals
		if (physFrameRepeatAccumulator >= PHYSREPEATCAP)
		{			
			// Dump remaining time from accumulators
			frameAccumulator -= (physFrameAccumulator - PHYSFRAMELENGTH);
			physFrameAccumulator = PHYSFRAMELENGTH;
			physPhase = 1.0f;
			
			// Move to a new general frame
			return false;
			
			// The repeat accumulator is reset at the end of each general frame, not here.
		}

		// Check if it's time for a physics frame
		if (physFrameAccumulator >= PHYSFRAMELENGTH)
		{
			physFrameAccumulator -= PHYSFRAMELENGTH;
			frameDelta = PHYSFRAMELENGTH;
			return true;
		}
		else
			return false;
	}

	// Steps Time for a new frame
	void Time::FrameStart()
	{
		Tick();

		// Update exposed values
		frameDelta = frameAccumulator;
		physPhase = physFrameAccumulator / PHYSFRAMELENGTH;

		// Reset accumulator
		frameAccumulator = 0.0f;
	}

	void Time::FrameEnd()
	{
		/* physFrameRepeatAccumulator is reset at the end of the frame to ensure that 
			a super long frame doesn't itself cause a death spiral. */
		physFrameRepeatAccumulator = 0.0f;
	}
}