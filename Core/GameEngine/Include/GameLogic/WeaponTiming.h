/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once

#include "Common/GameCommon.h"

// GeneralsX @feature Codex 22/11/2026 Limit automatic zero-delay fire between retail ticks.
// A salvo can fire repeatedly on the same tick. Positive cooldowns and explicit reloads have their own deadlines.
constexpr bool weaponIsBetweenZeroDelayShots(UnsignedInt frame, UnsignedInt lastShot, UnsignedInt readyFrame)
{
	return lastShot != 0 && readyFrame == lastShot && frame != lastShot &&
		frame - lastShot < static_cast<UnsignedInt>(logicFramesFromBaseFrames(1));
}
