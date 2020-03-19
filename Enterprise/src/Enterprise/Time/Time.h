#pragma once
#include "Core.h"
#include "Enterprise/Application/Application.h"

namespace Enterprise 
{
	/* Time
		Enterprise's Time system.  Use Time to track frame deltas, set timers, and measure performance.
	*/
	namespace Constants
	{
		extern const float physframelength; // Length, in game-seconds, of the physics timestep.
		extern const float physframerepeatcap; // The longest amount of real seconds back-to-back physics calculations are allowed to take.
	}

	class Time
	{
	public:
		// Getters ------------------------------  TODO: Why can't these be inline?

		// Gets the number of real-world seconds that have elapsed since application start.  Current to the top of each frame.
		static float RunningTime();
		// Gets the number of game-time seconds this frame or physics frame represents.  For physics frames, this is a constant.
		static float FrameDelta();
		// Gets the number of real seconds this frame or physics frame represents.  This does not reflect the current time scale.
		static float RealDelta();
		// Returns a value in the range [0,1] specifying progress through the current physics frame.  Do not use in PhysUpdate().
		static float PhysPhase();

		// Setters ------------------------------

		// Sets the rate of in-game time flow.  Useful for bullet-time effects.
		static void SetTimeScale(float scalar);
		//inline static void SetTimeScale(float scalar, float lerpTime);

		// TODO: Add timer functions

	private:
		// Needed for Application to advance time
		friend class Application;

		// Initializes timing functions.  Defined per platform.
		static void Init();
		// Updates internal time storage and fills accumulators.  Called at the start of each frame and physics frame.
		static void Tick();
		// Returns real-world time in seconds since Time::Init().  Defined per platform.
		static float GetRawTime();

		// Updates time values for use in a new physics frame.  Returns true when one is pending.
		static bool PhysFrame();
		// Updates time values for use in a new frame.
		static void FrameStart();
		// Resets clock for PhysFrame repeat check (prevents spiral of death).  Call at the end of the frame.
		static void FrameEnd();
	};
}