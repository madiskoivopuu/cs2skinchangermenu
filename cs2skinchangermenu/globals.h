#pragma once
#include "sdk/gameclasses/CCSPlayerController.h"

#define TEAM_CT -1
#define TEAM_T -1

namespace globals {
	extern CCSPlayerController** ppLocalPlayer; // pointer to address that points to local player
}

bool InitializeGlobals();