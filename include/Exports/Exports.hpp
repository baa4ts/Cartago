// include/Exports/Exports.hpp
#pragma once

#include <Windows.h>
#include <cstdio>
#include <set>
#include "Memory/RvaOffset.hpp"

// Declaración del template
template<typename T>
void LeerExports(PBYTE FILE, T NT, DWORD exportTableRVA);