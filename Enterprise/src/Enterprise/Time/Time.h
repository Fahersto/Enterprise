#pragma once
#include "Core.h"
#include "Enterprise/Application/Application.h"

namespace Enterprise 
{

namespace Constants
{
/// Length, in game-seconds, of the physics timestep.
HCEX(float, PhysFrameLength);
/// The maximum number of real seconds back-to-back physics frames are allowed to take.
HCEX(float, PhysFrameRepeatCap);
}


/// The Enterprise time system.
class Time
{
public:
	// Getters ------------------------------

	/// Gets the number of real-world seconds that have elapsed since application start.
	/// @return The number of seconds the application has been open.
	/// @note	The count is current to the top of the current frame.
	static float RunningTime();
	
	/// Gets the number of game-seconds that have passed since the last frame.
	/// @return The number of game-time seconds that have passed since the last frame.
	/// @note	This value scales with SetTimeScale.
	/// @note	In physics frames, the value is fixed.
	static float FrameDelta();

	/// Gets the number of real-world seconds that have passed since the last frame.
	/// @return The number of real seconds that have passed since the last frame.
	/// @note		This value does not stretch if you've applied a time scale factor.
	/// @remarks	This value is useful for stepping things which should not be affected by bullet-time
	///				effects, such as UI elements.
	static float RealDelta();
	
	/// Gets an interpolation value representing this frame's position through the current physics frame.
	/// @return A value in the range [0.0, 1.0], where 0.0 represents the start of the physics frame and
	/// 1.0 represents the end of it.
	///
	/// @remarks	This value can be used to smooth the motion of physics objects when being rendered.
	///				When a physics frame is finished calculating, physics objects will be simulated
	///				slightly into the future.  PhysPhase can be used to simulate the position of physics
	///				objects in between the start and end of the last physics frame.
	static float PhysPhase();

	// Setters ------------------------------

	/// Sets the rate of in-game time flow.  Useful for bullet-time effects.
	/// @param scalar The factor to apply to the flow of game time.
	/// @pre @p scalar cannot be negative.
	static void SetTimeScale(float scalar);

	// TODO: Add a lerping version of SetTimeScale.
	//inline static void SetTimeScale(float scalar, float lerpTime);

	// TODO: Add timer functions

private:
	friend class Application;

	/// Initializes timing functions.
	/// @note This is defined per platform.
	static void Init();

	/// Updates internal time storage and fills accumulators.
	/// @note This is called at the start of each frame and each physics frame.
	static void Tick();

	/// Calculates the time in seconds since Time::Init().
	/// @note This is defined per platform.
	static float GetRawTime();

	/// Checks the physics frame timer.
	/// @return Returns true when a physics frame is pending.
	/// @note	If this function doesn't return true in less than @c Constants::PhysFrameRepeatCap
	///			seconds, the timer is reset. Time will drop the unsimulated time to avoid death spirals.
	static bool PhysFrame();

	/// Updates time values for use in a new frame.
	static void FrameStart();

	/// Marks the end of a frame.
	/// @note This function resets the clock for the physics frame repeat check (death spiral abort timer).
	static void FrameEnd();
};

}
