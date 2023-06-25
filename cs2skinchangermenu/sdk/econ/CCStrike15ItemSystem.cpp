#include "pch.h"
#include "CCStrike15ItemSystem.h"

CCStrike15ItemSchema* CCStrike15ItemSystem::GetCCStrike15ItemSchema() {
	using Fn = CCStrike15ItemSchema* (__thiscall*)(CCStrike15ItemSystem*);
	Fn call = (*reinterpret_cast<Fn**>(this))[0];
	return call(this);
}