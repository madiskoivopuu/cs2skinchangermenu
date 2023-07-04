#pragma once
#include "sdk/gameclasses/CCSPlayerController.h"

namespace globals {
	extern CCSPlayerController** ppLocalPlayer; // pointer to address that points to local player
}

bool InitializeGlobals();