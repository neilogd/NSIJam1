#pragma once

#include "System/Os/OsEvents.h"

enum GaEvents {
	gaEVT_START_WAVE = osEVT_MAX,
	gaEVT_END_WAVE,
	gaEVT_SHIP_DESTROYED
};

struct GaEventWave : OsEventInput< GaEventWave >
{
	BcBool StartWave_;
};

struct GaShipDestroyedEvent : OsEventInput< GaShipDestroyedEvent >
{
	BcBool IsPlayer_;
};