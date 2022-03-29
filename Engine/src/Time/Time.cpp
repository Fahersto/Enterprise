#include "Enterprise/Time/Time.h"
#include "Enterprise/Graphics/Graphics.h"

using Enterprise::Time;

static uint64_t fixedTimestepInRealTicks;
#ifdef EP_CONFIG_RELEASE
static uint64_t fixedTimestepInGameTicks, maxFrameDeltaInRealTicks;
#endif

static double currentTimeScale = 1.0;

static uint64_t currentSysTimeInTicks = 0, previousSysTimeInTicks;
static uint64_t measuredRealTickDelta, measuredGameTickDelta;
static uint64_t realRunningTicks = 0, gameRunningTicks = 0;
static uint64_t unsimmedRealTicks = 0, unsimmedGameTicks = 0;

static bool inFixedTimestep_out = true;
static float realRunningTime_out, gameRunningTime_out;
static float realDelta_out, gameDelta_out;
static float unsimmedRealTime_out, unsimmedGameTime_out;
static float fixedFrameInterp_out;

static struct timeGlobalUBStruct
{
	glm::vec4 ep_time_real;
	glm::vec4 ep_time_game;
	glm::vec4 ep_time_sinreal;
	glm::vec4 ep_time_singame;
	glm::vec4 ep_time_cosreal;
	glm::vec4 ep_time_cosgame;
} timeGlobalUBData;
static Enterprise::Graphics::UniformBufferHandle timeGlobalUB;

bool Time::inFixedTimestep() { return inFixedTimestep_out; }
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

#ifdef EP_CONFIG_RELEASE
	maxFrameDeltaInRealTicks = SecondsToTicks(Constants::Time::MaxFrameDelta / currentTimeScale);
	fixedTimestepInRealTicks = SecondsToTicks(Constants::Time::FixedTimestep / currentTimeScale);
	fixedTimestepInGameTicks = SecondsToTicks(Constants::Time::FixedTimestep);
#endif

	timeGlobalUB = Graphics::CreateUniformBuffer(HN("EP_TIME"), sizeof(timeGlobalUBStruct));
}

void Time::Cleanup()
{
	Graphics::DeleteUniformBuffer(timeGlobalUB);
}


bool Time::ProcessFixedUpdate()
{
#ifdef EP_CONFIG_RELEASE
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
		inFixedTimestep_out = false;

		realRunningTime_out = TicksToSeconds(realRunningTicks);
		gameRunningTime_out = TicksToSeconds(gameRunningTicks);

		realDelta_out = TicksToSeconds(measuredRealTickDelta);
		gameDelta_out = TicksToSeconds(measuredGameTickDelta);

		unsimmedRealTime_out = TicksToSeconds(unsimmedRealTicks);
		unsimmedGameTime_out = TicksToSeconds(unsimmedGameTicks);

#ifdef EP_CONFIG_RELEASE
		fixedFrameInterp_out = double(unsimmedGameTicks) / double(fixedTimestepInGameTicks);
#else
		fixedFrameInterp_out = TicksToSeconds(unsimmedGameTicks) / Constants::Time::FixedTimestep;
#endif

		return false;
	}
}

void Time::Update()
{
	previousSysTimeInTicks = currentSysTimeInTicks;
	currentSysTimeInTicks = GetRawTicks();

#ifdef EP_CONFIG_RELEASE
	measuredRealTickDelta = std::min(currentSysTimeInTicks - previousSysTimeInTicks, maxFrameDeltaInRealTicks);
#else
	measuredRealTickDelta = std::min(currentSysTimeInTicks - previousSysTimeInTicks, SecondsToTicks(Constants::Time::MaxFrameDelta / currentTimeScale));
#endif
	measuredGameTickDelta = measuredRealTickDelta * currentTimeScale;

	realRunningTicks += measuredRealTickDelta;
	gameRunningTicks += measuredGameTickDelta;

	unsimmedRealTicks += measuredRealTickDelta;
	unsimmedGameTicks += measuredGameTickDelta;

	// Going to FixedUpdate()
	inFixedTimestep_out = true;

#ifdef EP_CONFIG_RELEASE
	realDelta_out = TicksToSeconds(fixedTimestepInRealTicks);
#else
	realDelta_out = TicksToSeconds(SecondsToTicks(Constants::Time::FixedTimestep / currentTimeScale));
#endif
	gameDelta_out = Constants::Time::FixedTimestep;

	unsimmedRealTime_out = 0.0f;
	unsimmedGameTime_out = 0.0f;
	fixedFrameInterp_out = 0.0f;

	// Update time variables in shader uniform buffer
	timeGlobalUBData.ep_time_real.x = realRunningTime_out / 20;
	timeGlobalUBData.ep_time_real.y = realRunningTime_out;
	timeGlobalUBData.ep_time_real.z = realRunningTime_out * 2;
	timeGlobalUBData.ep_time_real.w = realRunningTime_out * 3;
	timeGlobalUBData.ep_time_game.x = gameRunningTime_out / 20;
	timeGlobalUBData.ep_time_game.y = gameRunningTime_out;
	timeGlobalUBData.ep_time_game.z = gameRunningTime_out * 2;
	timeGlobalUBData.ep_time_game.w = gameRunningTime_out * 3;

	timeGlobalUBData.ep_time_sinreal.x = glm::sin(realRunningTime_out / 8);
	timeGlobalUBData.ep_time_sinreal.y = glm::sin(realRunningTime_out / 4);
	timeGlobalUBData.ep_time_sinreal.z = glm::sin(realRunningTime_out / 2);
	timeGlobalUBData.ep_time_sinreal.w = glm::sin(realRunningTime_out);
	timeGlobalUBData.ep_time_singame.x = glm::sin(gameRunningTime_out / 8);
	timeGlobalUBData.ep_time_singame.y = glm::sin(gameRunningTime_out / 4);
	timeGlobalUBData.ep_time_singame.z = glm::sin(gameRunningTime_out / 2);
	timeGlobalUBData.ep_time_singame.w = glm::sin(gameRunningTime_out);

	timeGlobalUBData.ep_time_cosreal.x = glm::cos(realRunningTime_out / 8);
	timeGlobalUBData.ep_time_cosreal.y = glm::cos(realRunningTime_out / 4);
	timeGlobalUBData.ep_time_cosreal.z = glm::cos(realRunningTime_out / 2);
	timeGlobalUBData.ep_time_cosreal.w = glm::cos(realRunningTime_out);
	timeGlobalUBData.ep_time_cosgame.x = glm::cos(gameRunningTime_out / 8);
	timeGlobalUBData.ep_time_cosgame.y = glm::cos(gameRunningTime_out / 4);
	timeGlobalUBData.ep_time_cosgame.z = glm::cos(gameRunningTime_out / 2);
	timeGlobalUBData.ep_time_cosgame.w = glm::cos(gameRunningTime_out);

	Graphics::SetUniformBufferData(timeGlobalUB, &timeGlobalUBData);
}


bool Time::isFixedUpdatePending()
{
#ifdef EP_CONFIG_RELEASE
	return unsimmedGameTicks / fixedTimestepInGameTicks;
#else
	return unsimmedGameTicks / SecondsToTicks(Constants::Time::FixedTimestep);
#endif
}

float Time::ActualRealDelta()
{
	return TicksToSeconds(measuredRealTickDelta);
}
