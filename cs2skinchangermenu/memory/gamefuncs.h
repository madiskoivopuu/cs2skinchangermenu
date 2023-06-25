#pragma once

#include "sdk/econ/CCStrike15ItemSystem.h"

// In-game function typedefs
using fGetCCStrike15ItemSystem = CCStrike15ItemSystem * (__fastcall*)(); // CSource2Client::

namespace fn {
	extern fGetCCStrike15ItemSystem CSource2Client__GetCCStrike15ItemSystem;
}

bool InitializeFunctions();