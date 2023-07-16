#include "pch.h"

#include "netvars.h"
#include "memory/interface.h" // we will need the client interface

#include "fnvhash.h"
#include <format>
#include <iostream>

#define LIBCXX_ENABLE_INCOMPLETE_FEATURES
//#define DUMP_NETVARS

#ifdef DUMP_NETVARS
#include <fstream>
#endif

namespace Netvars {
	// key -> Class::m_Netvar string hashed
	// value -> offset from base class
	std::unordered_map<uint64_t, uint32_t> list;
}

bool InitializeNetvars() {
	if (!Interface::schema)
		return false;

#ifdef DUMP_NETVARS
	std::ofstream myfile;
	myfile.open("C:/Users/madis/source/repos/cs2skinchangermenu/netvars.txt");
#endif

	// go through all binaries (.dll)
	for (int i = 0; i < Interface::schema->m_type_scopes_.Count(); i++) {
		CSchemaSystemTypeScope* typeScope = Interface::schema->m_type_scopes_[i]; // .dll
		const char* scopeName = reinterpret_cast<const char*>(typeScope->m_name_.data());

		// go through all declared classes in binary
		for (CSchemaClassBinding* binding : typeScope->m_classes_.GetElements()) {
			const char* className = binding->m_binary_name; // C_... or C...
			if (className == nullptr)
				return false;

			CSchemaClassInfo* classInfo = typeScope->FindDeclaredClass(className);

			// iterate over fields, store their offsets
			for (int fieldNum = 0; fieldNum < classInfo->m_align; fieldNum++) {
				SchemaClassFieldData_t* field = &classInfo->m_fields[fieldNum];
				if (!field)
					continue;

				const char* fieldName = field->m_name;

				// build our netvar string for lookup
				std::string classField = std::format("{}!{}->{}", scopeName, className, fieldName);
				Netvars::list[fnv::Hash(classField.c_str())] = field->m_single_inheritance_offset;

#ifdef DUMP_NETVARS
				myfile << classField << " " << std::hex << field->m_single_inheritance_offset << std::dec << " " << field->m_type->m_name_ << std::endl;
#endif
			}
		}
	}

#ifdef DUMP_NETVARS
	myfile.close();
#endif

	return true;
}