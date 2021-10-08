#include "EP_PCH.h"
#include "Time.h"

using Enterprise::Time;

static uint64_t fixedTimestepInRealTicks;
#ifdef EP_CONFIG_DIST
static uint64_t fixedTimestepInGameTicks, maxFrameDeltaInRealTicks;
#endif

static double currentTimeScale = 1.0;

static uint64_t currentSysTimeInTicks = 0, previousSysTimeInTicks;
static uint64_t measuredRealTickDelta, measuredGameTickDelta;
static uint64_t realRunningTicks = 0, gameRunningTicks = 0;
static uint64_t unsimmedRealTicks = 0, unsimmedGameTicks = 0;

static float realRunningTime_out, gameRunningTime_out;
static float realDelta_out, gameDelta_out;
static float unsimmedRealTime_out, unsimmedGameTime_out;
static float fixedFrameInterp_out;

float Time::RealTime() { return realRunningTime_out; }
float Time::GameTime() { return gameRunningTime_out; }
float Time::RealDelta() { return realDelta_out; }
float Time::GameDelta() { return gameDelta_out; }
float Time::RealRemainder() { return unsimmedRealTime_out; }
float Time::GameRemainder() { return unsimmedGameTime_out; }
float Time::FixedFrameInterp() { return fixedFrameInterp_out; }

void Time::SetTimeScale(double scalar)
{
	if (scalar >= 0.0f)
	{
		currentTimeScale = scalar;
	}
	else
	{
		EP_WARN("Time::SetTimeScale(): 'scalar' set to negative value.  Set to 0.0 instead.");
		currentTimeScale = 0.0f;
	}

	fixedTimestepInRealTicks = SecondsToTicks(Constants::Time::FixedTimestep / currentTimeScale);
}

void Time::Init()
{
	PlatformInit();

#ifdef EP_CONFIG_DIST
	maxFrameDeltaInRealTicks = SecondsToTicks(Constants::Time::MaxFrameDelta / currentTimeScale);
	fixedTimestepInRealTicks = SecondsToTicks(Constants::Time::FixedTimestep / currentTimeScale);
	fixedTimestepInGameTicks = SecondsToTicks(Constants::Time::FixedTimestep);
#endif
}


void Time::Update()
{
	previousSysTimeInTicks = currentSysTimeInTicks;
	currentSysTimeInTicks  = GetRawTicks();

#ifdef EP_CONFIG_DIST
	measuredRealTickDelta  = std::min(currentSysTimeInTicks - previousSysTimeInTicks, maxFrameDeltaInRealTicks);
#else
	measuredRealTickDelta = std::min(currentSysTimeInTicks - previousSysTimeInTicks, SecondsToTicks(Constants::Time::MaxFrameDelta / currentTimeScale));
#endif
	measuredGameTickDelta  = measuredRealTickDelta * currentTimeScale;

	realRunningTicks += measuredRealTickDelta;
	gameRunningTicks += measuredGameTickDelta;

	unsimmedRealTicks += measuredRealTickDelta;
	unsimmedGameTicks += measuredGameTickDelta;

	// Going to FixedUpdate()
#ifdef EP_CONFIG_DIST
	realDelta_out = TicksToSeconds(fixedTimestepInRealTicks);
#else
	realDelta_out = TicksToSeconds(SecondsToTicks(Constants::Time::FixedTimestep / currentTimeScale));
#endif
	gameDelta_out = Constants::Time::FixedTimestep;

	unsimmedRealTime_out = 0.0f;
	unsimmedGameTime_out = 0.0f;
	fixedFrameInterp_out = 0.0f;

	// TODO: Update time variables in shader uniform buffer
}

bool Time::FixedUpdatePending()
{
#ifdef EP_CONFIG_DIST
	if (unsimmedGameTicks >= fixedTimestepInGameTicks)
	{
		// Going to FixedUpdate()
		unsimmedRealTicks -= fixedTimestepInRealTicks;
		unsimmedGameTicks -= fixedTimestepInGameTicks;
#else
	if (unsimmedGameTicks >= SecondsToTicks(Constants::Time::FixedTimestep))
	{
		// Going to FixedUpdate()
		unsimmedRealTicks -= SecondsToTicks(Constants::Time::FixedTimestep / currentTimeScale);
		unsimmedGameTicks -= SecondsToTicks(Constants::Time::FixedTimestep);
#endif

		realRunningTime_out = TicksToSeconds(realRunningTicks - unsimmedRealTicks);
		gameRunningTime_out = TicksToSeconds(gameRunningTicks - unsimmedGameTicks);

		return true;
	}
	else
	{
		// Going to Update()
		realRunningTime_out = TicksToSeconds(realRunningTicks);
		gameRunningTime_out = TicksToSeconds(gameRunningTicks);

		realDelta_out = TicksToSeconds(measuredRealTickDelta);
		gameDelta_out = TicksToSeconds(measuredGameTickDelta);

		unsimmedRealTime_out = TicksToSeconds(unsimmedRealTicks);
		unsimmedGameTime_out = TicksToSeconds(unsimmedGameTicks);

#ifdef EP_CONFIG_DIST
		fixedFrameInterp_out = double(unsimmedGameTicks) / double(fixedTimestepInGameTicks);
#else
		fixedFrameInterp_out = TicksToSeconds(unsimmedGameTicks) / Constants::Time::FixedTimestep;
#endif

		return false;
	}
}

