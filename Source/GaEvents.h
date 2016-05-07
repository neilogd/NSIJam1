#pragma once

#include "System/Os/OsEvents.h"

enum GaEvents {
	gaEVT_START_WAVE = osEVT_MAX,
	gaEVT_END_WAVE
};

struct GaEventWave : OsEventInput< GaEventWave >
{
	BcBool StartWave_;
};