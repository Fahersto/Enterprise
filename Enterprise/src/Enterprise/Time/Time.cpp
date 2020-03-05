#include "EP_PCH.h"
#include "Enterprise/Time/Time.h"

// Length, in game-world seconds, each PhysFrame represents.
#define PHYSFRAMELENGTH (1.0f / 50.0f) //TODO: Set in client
// Longest amount of time we're allowed to call PhysFrames between Frames.
#define PHYSREPEATCAP (3.0f * PHYSFRAMELENGTH) //TODO: Set in client

// Vars for Tick()
float runningTime = 0.0f, prevTime = 0.0f, tickDeltaReal = 0.0f, tickDeltaScaled = 0.0f;
// Vars for frame-time getters
float frameDelta = 0.0f, physPhase = 1.0f;
// Vars for accumulation logic
float timeScale = 1.0f, frameAccumulator = 0.0f, physFrameAccumulator = PHYSFRAMELENGTH, physFrameRepeatAccumulator = 0.0f;

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

	// Core Calls -------------------------------
	void Time::Tick()
	{
		prevTime = runningTime;
		runningTime = GetRawTime();
		tickDeltaReal = runningTime - prevTime; // Calculate real-time delta since previous Time::Tick()
		tickDeltaScaled = tickDeltaReal * timeScale; // Convert into game-time delta based on timeScale

		// Increment accumulators
		frameAccumulator += tickDeltaScaled; // Used to track game-seconds between frames TODO: move descriptions above variable declarations
		physFrameAccumulator += tickDeltaScaled; // Used to time PhysUpdate() call frequency
		physFrameRepeatAccumulator += tickDeltaReal; // Used to curb PhysFrame death spirals

		// Perhaps here is the time to increment timers?
	}

	// Checks the timer for PhysFrame.  Returns true if PhysUpdate() should be called.
	bool Time::PhysFramePending()
	{
		Tick();

		// Prevent spiral of death
		if (physFrameRepeatAccumulator >= PHYSREPEATCAP)
			return false; // Force a frame to happen.  Time::FrameStep_begin() will handle the excess time.

		// Check if it's time for a PhysFrame
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
	void Time::FrameStep_begin()
	{
		Tick();

		// Update deltaTime for this frame
		frameDelta = frameAccumulator;
		frameAccumulator = 0.0f;

		// If we start a frame with a physics frame pending, it means we just aborted a physics death spiral.
		if (physFrameAccumulator > PHYSFRAMELENGTH)
		{
			// Drop physFrame_Accumulator's unprocessed time so the frame doesn't process it.
			frameDelta -= physFrameAccumulator;
			// Prime a PhysFrame to immediately follow this frame
			physFrameAccumulator = PHYSFRAMELENGTH;
			// Set physics-time "now" to be the same as frame-time "now"
			physPhase = 1.0f;
		}
		else
			physPhase = physFrameAccumulator / PHYSFRAMELENGTH;
	}

	void Time::FrameStep_end()
	{
		//TODO: Find a way to reset the repeat accumulator in one of the other functions.
		physFrameRepeatAccumulator = 0.0f;
	}
}
