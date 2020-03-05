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

		// Returns the number of real-world seconds that have elapsed since application start.  Updated every Time::Tick().
		inline static float RunningTime();
		// In Update(), returns the number of game-time seconds that have passed since the previous frame.  In PhysUpdate(), returns fixed timestep size.
		inline static float FrameDelta();
		// Returns a value in the range [0,1) indicating progress through current physics frame.
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
		
		// Updates internal time storage and fills accumulators.  Called before every FrameStep() and PhysFrameStep().
		static void Tick();

		// Returns true if it is time to process a new physics frame, and resets timer if it is.
		static bool PhysFramePending();
		// Updates time values for use in Update(), PostUpdate(), and Draw().
		static void FrameStep_begin();
		// Resets clock for PhysFrame repeat check (prevents spiral of death).
		static void FrameStep_end();
	};
}