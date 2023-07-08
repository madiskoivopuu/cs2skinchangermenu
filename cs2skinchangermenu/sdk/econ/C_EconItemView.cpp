#include "pch.h"
#include "C_EconItemView.h"
#include "memory/gamefuncs.h"

CWeaponCSBaseVData* C_EconItemView::GetCSWeaponDataFromItem() {
	return fn::GetCSWeaponDataFromItem(this);
}

void C_EconItemView::SetAttributeValueByName(const char* name, float value) {
	return fn::CEconItemView__SetAttributeValueByName(this, name, value);
}