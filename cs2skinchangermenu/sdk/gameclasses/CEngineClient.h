#pragma once
class CEngineClient {
public:
	// 32 virtual func (starting from 1)
	int IsInGame() {
		using Fn = int (__fastcall*)();
		Fn call = (*reinterpret_cast<Fn**>(this))[106];
		return call();
	}
};