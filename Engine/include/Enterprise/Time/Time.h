#pragma once
#include "Enterprise/Core.h"
#include "Enterprise/Application/Application.h"

namespace Enterprise 
{

namespace Constants::Time
{
///// The length of the fixed timestep in game seconds.
//HCEX(double, FixedTimestep);
///// The maximum number of real seconds each frame is allowed to simulate.
//HCEX(double, MaxFrameDelta);

constexpr double FixedTimestep = 1.0 / 240.0;
constexpr double MaxFrameDelta = 1.0 / 15.0;
}


/// The Enterprise time system.
class Time
{
public:
	/// Check whether the current code path is in a FixedUpdate().
	/// @return @c true if in FixedUpdate(), @c false if in Update() or Draw().
	EP_API static bool inFixedTimestep();

	/// Get the number of real seconds that have passed since application launch.
	/// @return The number of real-world seconds between application launch and the start of the current frame.
	/// @note If called from FixedUpdate(), the value is current to the start of the current fixed timestep.
	EP_API static float RealTime();
	/// Get the number of game seconds that have passed since application launch.
	/// @return The number of game-world seconds between application launch and the start of the current frame.
	/// @note If called from FixedUpdate(), the value is current to the start of the current fixed timestep.
	EP_API static float GameTime();
	
	/// Get the number of real seconds that have passed since the previous frame.
	/// @return The number of real-world seconds that have passed since the previous frame.
	/// @note If called from FixedUpdate(), returns the fixed timestep's current length in real seconds.
	EP_API static float RealDelta();
	/// Get the number of game seconds that have passed since the previous frame.
	/// @return The number of game-world seconds that have passed since the previous frame.
	/// @note If called from FixedUpdate(), this value always equals Constants::Time::FixedTimestep.
	EP_API static float GameDelta();

	/// Get the number of real seconds that have not yet been processed by FixedUpdate().
	/// @return The number of real-world seconds that have passed since the previous FixedUpdate().
	/// @remarks This value is useful for projecting the world state prior to rendering a frame.
	/// It serves no purpose in FixedUpdate().
	EP_API static float RealRemainder();
	/// Get the number of game seconds that have not yet been processed by FixedUpdate().
	/// @return The number of game-world seconds that have passed since the previous FixedUpdate().
	/// @remarks This value is useful for projecting the world state prior to rendering a frame.
	/// It serves no purpose in FixedUpdate().
	EP_API static float GameRemainder();

	/// Get this frame's position between fixed updates, expressed as a value in the range [0.0, 1.0).
	/// @return An interpolation value in [0.0, 1.0), where 0.0 is the start of the fixed timestep and 1.0 is the end.
	/// @remarks This value is useful for interpolating between the last two world states calculated in FixedUpdate().
	/// It serves no purpose in FixedUpdate().
	EP_API static float FixedFrameInterp();

	/// Set the rate of in-game time flow.  Useful for time dilation effects.
	/// @param scalar The rate of game time over real time.
	/// @remarks The game world runs twice as fast when the time scale is set to @c 2.0, and half as fast when set to @c 0.5.
	EP_API static void SetTimeScale(double scalar);

private:
	friend class Application;
	friend class Input;

	static void Init();
	static void PlatformInit();
	static void Cleanup();

	static uint64_t GetRawTicks();
	static uint64_t SecondsToTicks(double seconds);
	static float TicksToSeconds(uint64_t ticks);

	static void Update();
	static bool ProcessFixedUpdate();

	// Special access for values needed by the Input system
	static float ActualRealDelta();
	static bool isFixedUpdatePending();
};

}
