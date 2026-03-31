// src/main.cpp

#include <iostream>
#include <Windows.h>
#include <filesystem>

#include "Memory/Memory.hpp"
#include "Exports/Exports.hpp"

int main(int argc, char *argv[])
{
    /**
     * Validar argumentos y archivo de entrada
     */

    if (argc < 3)
    {
        std::cout << "uso: cartago <flag> <ruta_dll>" << std::endl;
        return 1;
    }

    const char *flag = argv[1];
    const char *path = argv[2];

    if (!std::filesystem::exists(path) ||
        std::filesystem::path(path).extension() != ".dll")
    {
        std::cout << "error: archivo invalido" << std::endl;
        return 1;
    }

    /**
     * Mapear el archivo en memoria
     */

    PBYTE FILE = nullptr;
    if (!Mapear(path, FILE) || !FILE)
    {
        std::cout << "error: no se pudo mapear" << std::endl;
        return 1;
    }

    /**
     * Acceder a la primera cabecera del PE
     */

    // Aceder a la primera seccion DOS HEADER
    // https://cocomelonc.github.io/assets/images/18/pefile.png
    // Ref: https://onlyf8.com/pe-format#dos-headers
    PIMAGE_DOS_HEADER DOS = reinterpret_cast<PIMAGE_DOS_HEADER>(FILE);

    // Validacion
    if (DOS->e_magic != IMAGE_DOS_SIGNATURE) // "MZ"
    {
        std::cout << "Archivo inválido (no MZ)\n";
        delete[] FILE;
        return 1;
    }

    // Validacion de e_lfanew [!] Posibles corrupcion [!]
    if (DOS->e_lfanew <= 0 || DOS->e_lfanew > 0x100000) // límite defensivo
    {
        std::cout << "e_lfanew inválido\n";
        delete[] FILE;
        return 1;
    }

    // Mobernos al bloque NT HEADER           (BYTE + BYTE: Moverse)
    PIMAGE_NT_HEADERS NT = reinterpret_cast<PIMAGE_NT_HEADERS>(FILE + DOS->e_lfanew);

    // Validacion
    if (NT->Signature != IMAGE_NT_SIGNATURE) // "PE\0\0"
    {
        std::cout << "Archivo inválido (no PE)\n";
        delete[] FILE;
        return 1;
    }

    // Acceder al campo machine en el COFF
    // para identificar la arquitectura
    // https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#machine-types

    DWORD exportTableRVA = 0;
    bool is32 = false;

    if (
        // Si es x32
        NT->FileHeader.Machine == IMAGE_FILE_MACHINE_I386 &&
        NT->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    // https://0xrick.github.io/win-internals/pe4/
    {
        // Logica                         (recast)
        PIMAGE_OPTIONAL_HEADER32 POT32 = reinterpret_cast<PIMAGE_OPTIONAL_HEADER32>(&NT->OptionalHeader);

        // Obtener la direccion de la tabla de los export
        exportTableRVA = POT32->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

        is32 = true;
    }
    else if (
        // Si es x64
        NT->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 &&
        NT->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
        // https://0xrick.github.io/win-internals/pe4/
    {
        // Logica                            (recast)
        PIMAGE_OPTIONAL_HEADER64 POT64 = reinterpret_cast<PIMAGE_OPTIONAL_HEADER64>(&NT->OptionalHeader);

        // Obtener la direccion de la tabla de los export
        exportTableRVA = POT64->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    }
    else
    {
        // Si no es ninguna de las 2 arquitecturas preferidas
        std::cout << " Arquitectura no admintida. Actualmente solo x32/x64" << std::endl;
        delete[] FILE;
        return 1;
    }

    if (strcmp(flag, "-r") == 0)
    {
        if (is32)
            LeerExports(FILE, reinterpret_cast<PIMAGE_NT_HEADERS32>(NT), exportTableRVA);
        else
            LeerExports(FILE, reinterpret_cast<PIMAGE_NT_HEADERS64>(NT), exportTableRVA);
    }

    delete[] FILE;
    return 0;
}