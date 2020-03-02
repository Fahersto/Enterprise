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

		// Returns the number of real-world seconds that have elapsed since application start.  Updated every frame and every physics frame.
		inline static float RunningTime();
		// Returns the number of game-world seconds that have passed since the last frame.  Affected by timeScale.
		inline static float DeltaTime();
		// Returns a value in the range [0,1) representing current phase position of the SimStep cycle.  Used to smooth rendering of physics-based motion.
		inline static float SimPhase();

		// Setters ------------------------------

		// Sets the rate of in-game time flow.  Useful for bullet-time effects.
		inline static void SetTimeScale(float scalar);
		//static void LerpTimeScale(float targetScalar, float lerpTime);

		// TODO: Add timer functions

	private:
		friend class Application;
		// Initializes timing functions.  Defined per platform.
		static void Init();
		// Returns real-world time in seconds since Time::Init().  Defined per platform.
		static float GetRawTime();
		
		// Updates internal time storage and fills accumulators.  Called before every frame and physics frame.
		static void Tick();

		// Returns true if it is time to process a new physics frame.
		static bool SimStep();
		// Updates time values for use in Update(), PostUpdate(), and Draw().
		static void FrameStep_begin();
		// Resets clock for SimStep throttling (prevents spiral of death).
		static void FrameStep_end();
	};
}