#pragma once

#include "memory/interface.h"

// modified impl to fit entity list
template<typename T>
class CHandle {
	int handleValue;

public:
	T* GetEnt() {
		return Interface::entities->GetFromEntityList<T>(this->handleValue);
	}

	int Get() {
		return this->handleValue;
	}

	void Set(int newValue) {
		this->handleValue = newValue;
	}

	bool IsInvalid() {
		return this->handleValue == -1;
	}
};