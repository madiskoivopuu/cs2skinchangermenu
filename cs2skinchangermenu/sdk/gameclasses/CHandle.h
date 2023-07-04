#pragma once

#include "memory/interface.h"

// modified impl to fit entity list
template<typename T>
class CHandle {
	int handleValue;

public:
	T* Get() {
		return Interface::entities->GetFromEntityList<T>(this->handleValue);
	}
};