// src/Memory/Memory.cpp
#include "Memory/Memory.hpp"

bool Mapear(const char *dll, PBYTE &OUTPUT)
{
    HANDLE HandleFile = CreateFileA(
        dll,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (HandleFile == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error al abrir el archivo" << std::endl;
        return false;
    }

    HANDLE HandleMapping = CreateFileMappingA(
        HandleFile,
        NULL,
        PAGE_READONLY,
        0,
        0,
        NULL);

    if (!HandleMapping)
    {
        std::cout << "Error al mapear la dll" << std::endl;
        CloseHandle(HandleFile);
        return false;
    }

    PBYTE pByte = reinterpret_cast<PBYTE>(MapViewOfFile(
        HandleMapping,
        FILE_MAP_READ,
        0,
        0,
        0));

    if (!pByte)
    {
        std::cout << "Error al crear la vista del archivo" << std::endl;
        CloseHandle(HandleMapping);
        CloseHandle(HandleFile);
        return false;
    }

    DWORD size = GetFileSize(HandleFile, NULL);
    if (size == INVALID_FILE_SIZE)
    {
        std::cout << "Error al obtener tamaño del archivo" << std::endl;
        UnmapViewOfFile(pByte);
        CloseHandle(HandleMapping);
        CloseHandle(HandleFile);
        return false;
    }

    OUTPUT = new BYTE[size];
    memcpy(OUTPUT, pByte, size);

    // Cerrar los handle
    UnmapViewOfFile(pByte);
    CloseHandle(HandleMapping);
    CloseHandle(HandleFile);
    return true;
}