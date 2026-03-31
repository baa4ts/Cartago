// include/Memory/RvaOffset.hpp
#pragma once

#include <Windows.h>

DWORD RvaOffset(PIMAGE_NT_HEADERS32 &NT, DWORD RVA);
DWORD RvaOffset(PIMAGE_NT_HEADERS64 &NT, DWORD RVA);