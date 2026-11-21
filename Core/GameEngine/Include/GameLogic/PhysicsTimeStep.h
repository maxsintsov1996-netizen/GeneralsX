/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once

#include "Common/GameCommon.h"
#include "WWMath/wwmath.h"

// GeneralsX @feature Codex 22/11/2026 Preserve the fraction of velocity lost to drag over one retail frame.
inline Real physicsFrictionForStep(Real frictionPerStep, Real baseMinimum, Real baseMaximum)
{
	Real baseFriction = frictionPerStep * logicFramesFromBaseFrames(1);
	if (baseFriction < baseMinimum) baseFriction = baseMinimum;
	if (baseFriction > baseMaximum) baseFriction = baseMaximum;
#if defined(RTS_EXPERIMENTAL_60HZ_SIMULATION)
	// Two 60 Hz steps must retain the same velocity as one 30 Hz step.
	// f / (1 + sqrt(1 - f)) is stable even for small f.
	return baseFriction / (1.0f + WWMath::Sqrt(1.0f - baseFriction));
#else
	return baseFriction;
#endif
}
