#include "EP_PCH.h"
#include "Enterprise/Time/Time.h"

// Length, in game-world seconds, each PhysFrame represents.
#define PHYSFRAMELENGTH (1.0f / 50.0f) //TODO: Set in client
// Longest amount of time we're allowed to call PhysFrames between Frames.
#define PHYSREPEATCAP (3.0f * PHYSFRAMELENGTH) //TODO: Set in client

// Vars for Tick()
float runningTime = 0.0f, prevTime = 0.0f, tickDeltaReal = 0.0f, tickDeltaScaled = 0.0f;
// Vars for frame-time getters
float frameDelta = 0.0f, physPhase = 0.0f;
// Vars for accumulation logic
float timeScale = 1.0f, frameDelta_Accumulator = 0.0f, physFrame_Accumulator = 0.0f, physFrameRepeat_Accumulator = 0.0f;

namespace Enterprise 
{
	// Getters ----------------------------------
	float Time::RunningTime() { return runningTime; }
	float Time::FrameDelta() { return frameDelta; }
	float Time::PhysPhase() { return physPhase; } //TODO: Prevent use in PhysUpdate().
	//float Time::PhysDelta()

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
		runningTime = Time::GetRawTime();
		tickDeltaReal = runningTime - prevTime; // Calculate real-world time delta since previous Time::Tick()
		tickDeltaScaled = tickDeltaReal * timeScale; // Convert into game-world time delta based on timeScale

		// Increment accumulators
		frameDelta_Accumulator += tickDeltaScaled; // Used to track game-seconds between Frames
		physFrame_Accumulator += tickDeltaScaled; // Used to time PhysUpdate() call frequency
		physFrameRepeat_Accumulator += tickDeltaReal; // Used to curb PhysFrame death spirals
	}

	// Checks the timer for PhysFrame.  Returns true if PhysUpdate() should be called.
	bool Time::PhysFramePending()
	{
		Tick();

		// Prevent spiral of death
		if (physFrameRepeat_Accumulator >= PHYSREPEATCAP)
			return false; // Force a frame to happen.  Time::FrameStep_begin() will handle the excess time.

		// Check if it's time for a PhysFrame
		if (physFrame_Accumulator >= PHYSFRAMELENGTH)
		{
			physFrame_Accumulator -= PHYSFRAMELENGTH;
			return true;
		}
		else
			return false;
	}

	// Updates DeltaTime, DeltaTime_Real, and PhysPhase for next frame.
	void Time::FrameStep_begin()
	{
		Tick();

		// Update deltaTime for this frame
		frameDelta = frameDelta_Accumulator;
		frameDelta_Accumulator = 0.0f;

		// Calculate physPhase for this frame
		physPhase = physFrame_Accumulator / PHYSFRAMELENGTH;

		// Handle when physics frames lagging too far behind
		if (physPhase >= 1.0)
		{
			// Drop physFrame_Accumulator's unprocessed game time to catch up to real time.
			frameDelta -= physFrame_Accumulator;
			frameDelta_Accumulator -= physFrame_Accumulator;
			physFrame_Accumulator = 0.0f;
			physPhase = 0.0f;
		}
	}

	void Time::FrameStep_end()
	{
		//TODO: Find a way to reset the repeat accumulator in one of the other functions.
		physFrameRepeat_Accumulator = 0.0f;
	}
}
