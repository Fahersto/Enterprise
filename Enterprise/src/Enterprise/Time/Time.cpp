#include "EP_PCH.h"
#include "Enterprise/Time/Time.h"

// Length, in game-world seconds, each SimStep() represents.
#define SIMSTEPLENGTH (5.0f) //TODO: Set in client
// Longest amount of time we're allowed to call SimSteps() without a FrameStep().
#define SIMSTEPCAP (6.0f) //TODO: Set in client

// Vars for Tick()
float runningTime = 0.0f, prevTime = 0.0f, tickDeltaReal = 0.0f, tickDeltaScaled = 0.0f;
// Vars for frame-time getters
float deltaTime = 0.0f, simPhase = 0.0f;
// Vars for accumulation logic
float timeScale = 1.0f, deltaTime_Accumulator = 0.0f, simStep_Accumulator = 0.0f, simStepRepeat_Accumulator = 0.0f;

namespace Enterprise 
{
	// Getters ----------------------------------
	float Time::RunningTime() { return runningTime; }
	float Time::DeltaTime() { return deltaTime; }
	float Time::SimPhase() { return simPhase; } //TODO: Prevent use in SimStep().

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
		deltaTime_Accumulator += tickDeltaScaled; // Used to track game-seconds between frames
		simStep_Accumulator += tickDeltaScaled; // Used to time SimStep() call frequency
		simStepRepeat_Accumulator += tickDeltaReal; // Used to curb SimStep death spirals
	}

	// Checks the timer for SimStep.  Returns true if SimStep() should be called.
	bool Time::SimStep()
	{
		Tick();

		// Prevent spiral of death
		if (simStepRepeat_Accumulator >= SIMSTEPCAP)			
			return false; // Force a frame to happen.  Time::FrameStep_begin() will handle the excess time.

		// Check for SimStep
		if (simStep_Accumulator >= SIMSTEPLENGTH) 
		{
			simStep_Accumulator -= SIMSTEPLENGTH;
			return true;
		}
		else
			return false;
	}

	// Updates DeltaTime, DeltaTime_Real, and SimPhase for next frame.
	void Time::FrameStep_begin()
	{
		Tick();

		// Update deltaTime for this frame
		deltaTime = deltaTime_Accumulator;
		deltaTime_Accumulator = 0.0f;

		// Calculate simPhase for this frame
		simPhase = simStep_Accumulator / SIMSTEPLENGTH;

		// Handle when physics frames lagging too far behind
		if (simPhase >= 1.0)
		{
			// Dump simStep_accumulator's unprocessed game time to catch up to real time.
			deltaTime -= simStep_Accumulator;
			deltaTime_Accumulator -= simStep_Accumulator;
			simStep_Accumulator = 0.0f;
			simPhase = 0.0f;
		}
	}

	void Time::FrameStep_end()
	{
		//TODO: Find a way to reset the repeat accumulator in one of the other functions.
		simStepRepeat_Accumulator = 0.0f;
	}
}
