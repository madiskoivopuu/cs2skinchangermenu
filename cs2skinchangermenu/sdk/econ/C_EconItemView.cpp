#include "pch.h"
#include "C_EconItemView.h"
#include "memory/gamefuncs.h"

CCSWeaponBaseVData* C_EconItemView::GetCSWeaponDataFromItem() {
	return fn::GetCSWeaponDataFromItem(this);
}

void C_EconItemView::SetAttributeValueByName(const char* name, float value) {
	return fn::CEconItemView__SetAttributeValueByName(this, name, value);
}

void C_EconItemView::SetAttributeValueIntByName(const char* name, int value) {
	// hacky way to get around the issue of where some econ attributes need an int value instead of float
	float temp;
	memcpy(&temp, &value, sizeof(temp));

	return fn::CEconItemView__SetAttributeValueByName(this, name, temp);
}