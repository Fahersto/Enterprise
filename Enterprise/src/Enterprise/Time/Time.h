#pragma once
#include "Core.h"
#include "Enterprise/Application/Application.h"

/* Time.h
	Enterprise's Time system.  Use Time to track frame deltas, set timers, and measure performance.
*/

namespace Enterprise 
{
	class Time
	{
	public:
		// Getters ------------------------------

		// Gets the number of real-world seconds that have elapsed since application start.  Updated every frame and physframe.
		inline static float RunningTime();
		// Gets the number of game-time seconds this frame represents.  In PhysUpdate(), gets fixed timestep.
		inline static float FrameDelta();
		// Returns a value in the range [0,1] indicating progress through current physics frame.
		inline static float PhysPhase();

		// Setters ------------------------------

		// Sets the rate of in-game time flow.  Useful for bullet-time effects.
		inline static void SetTimeScale(float scalar);
		//inline static void SetTimeScale(float scalar, float lerpTime);

		// TODO: Add timer functions

	private:
		friend class Application;
		// Initializes timing functions.  Defined per platform.
		static void Init();
		// Returns real-world time in seconds since Time::Init().  Defined per platform.
		static float GetRawTime();
		
		// Updates internal time storage and fills accumulators.  Called at the start of each frame and physics frame.
		static void Tick();

		// Updates time values for use in a new physics frame.  Returns true when one is pending.
		static bool PhysFrame();
		// Updates time values for use in a new frame.
		static void FrameStart();
		// Resets clock for PhysFrame repeat check (prevents spiral of death).
		static void FrameEnd();
	};
}