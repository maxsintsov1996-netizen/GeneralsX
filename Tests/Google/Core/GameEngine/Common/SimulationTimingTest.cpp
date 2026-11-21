/* SPDX-License-Identifier: GPL-3.0-or-later */
#include <cmath>
#include <gtest/gtest.h>

#include "GameLogic/PhysicsTimeStep.h"
#include "GameLogic/WeaponTiming.h"

TEST(SimulationTiming, ModUnitsUseWallClockSeconds)
{
	const float steps = LOGICFRAMES_PER_SECONDS_REAL;
	EXPECT_FLOAT_EQ(ConvertDurationFromMsecsToFrames(10000.0f), steps * 10.0f); // oil derrick
	EXPECT_FLOAT_EQ(ConvertDurationFromMsecsToFrames(100.0f), steps * 0.1f); // poison damage
	EXPECT_FLOAT_EQ(ConvertVelocityInSecsToFrames(300.0f) * steps, 300.0f);
	EXPECT_FLOAT_EQ(ConvertAccelerationInSecsToFrames(450.0f) * steps * steps, 450.0f);
	EXPECT_FLOAT_EQ(logicFramesFromBaseFrames(3) / steps, 0.1f); // unit path request debounce
}

TEST(SimulationTiming, DragRetainsRetailVelocityOverOneSecond)
{
	const float authoredFractions[] = { -1.0f, 0.0f, 0.00001f, 0.01f, 0.15f, 0.65f, 0.8f, 0.99f, 3.0f };
	for (const float baseFriction : authoredFractions)
	{
		SCOPED_TRACE(baseFriction);
		for (const float minimum : { 0.0f, 0.01f })
		{
			const float stepLoss = physicsFrictionForStep(logicRateFromBaseRate(baseFriction), minimum, 0.99f);
			float remaining = 1.0f;
			for (int i = 0; i < LOGICFRAMES_PER_SECOND; ++i)
				remaining *= 1.0f - stepLoss;
			const float clamped = std::fmin(0.99f, std::fmax(minimum, baseFriction));
			EXPECT_NEAR(remaining, std::pow(1.0f - clamped, BaseFps), 1e-5f);
		}
	}
}

TEST(SimulationTiming, AutomaticFireKeepsSalvosAndExplicitDeadlines)
{
	const UnsignedInt fired = 100;
	const UnsignedInt nextRetailTick = fired + logicFramesFromBaseFrames(1);
	EXPECT_FALSE(weaponIsBetweenZeroDelayShots(fired, fired, fired)); // same-tick salvo
	EXPECT_FALSE(weaponIsBetweenZeroDelayShots(1, 0, 0)); // new weapon
	EXPECT_FALSE(weaponIsBetweenZeroDelayShots(fired + 1, fired, fired + 1)); // explicit cooldown or reload
	EXPECT_FALSE(weaponIsBetweenZeroDelayShots(nextRetailTick, fired, fired));
	for (UnsignedInt frame = fired + 1; frame < nextRetailTick; ++frame)
		EXPECT_TRUE(weaponIsBetweenZeroDelayShots(frame, fired, fired));
}
