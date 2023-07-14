#include "pch.h"
#include "CAttributeList.h"
#include "sdk/econ/CEconItemAttributeDefinition.h"

#include "memory/interface.h"

void CAttributeList::RemoveAttribute(char* attribName) {
	CEconItemAttributeDefinition* def = Interface::client->GetCCStrike15ItemSystem()->GetCCStrike15ItemSchema()->GetAttributeDefinitionByName(attribName);
	if (!def)
		return;

	for (int i = 0; i < this->m_Attributes().Count(); i++) {
		if (this->m_Attributes()[i].m_iAttributeDefinitionIndex() == def->m_iAttributeDefinitionIndex) {
			this->m_Attributes().Remove(i);
			// TODO: update attribute manager cache
			break;
		}
	}
}

CEconItemAttribute* CAttributeList::FindAttribute(char* attribName) {
	CEconItemAttributeDefinition* def = Interface::client->GetCCStrike15ItemSystem()->GetCCStrike15ItemSchema()->GetAttributeDefinitionByName(attribName);
	if (!def)
		return nullptr;

	for (int i = 0; i < this->m_Attributes().Count(); i++) {
		if (this->m_Attributes()[i].m_iAttributeDefinitionIndex() == def->m_iAttributeDefinitionIndex) {
			return &this->m_Attributes()[i];
		}
	}

	return nullptr;
}