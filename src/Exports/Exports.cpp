// src/Exports/Exports.cpp
#include "Exports/Exports.hpp"

// Implementación del template
template <typename T>
void LeerExports(PBYTE FILE, T NT, DWORD exportTableRVA)
{
    DWORD DirectorioExportacionOFFSET = RvaOffset(NT, exportTableRVA);
    if (!DirectorioExportacionOFFSET)
    {
        printf("No se pudo resolver el Export Directory\n");
        return;
    }

    PIMAGE_EXPORT_DIRECTORY expDir =
        (PIMAGE_EXPORT_DIRECTORY)(FILE + DirectorioExportacionOFFSET);

    DWORD expDirRVA = NT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    DWORD expDirSize = NT->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

    DWORD *funciones = (DWORD *)(FILE + RvaOffset(NT, expDir->AddressOfFunctions));
    DWORD *nombres = (DWORD *)(FILE + RvaOffset(NT, expDir->AddressOfNames));
    WORD *ordinals = (WORD *)(FILE + RvaOffset(NT, expDir->AddressOfNameOrdinals));

    std::set<WORD> nombreSET;
    for (DWORD i = 0; i < expDir->NumberOfNames; i++)
        nombreSET.insert(ordinals[i]);

    for (DWORD i = 0; i < expDir->NumberOfFunctions; i++)
    {
        if (!funciones[i])
            continue;

        WORD ordinal = (WORD)(expDir->Base + i);

        bool esForward = (funciones[i] >= expDirRVA &&
                          funciones[i] < expDirRVA + expDirSize);

        const char *nombre = nullptr;

        if (nombreSET.count(i))
        {
            for (DWORD j = 0; j < expDir->NumberOfNames; j++)
            {
                if (ordinals[j] == i)
                {
                    nombre = (const char *)(FILE + RvaOffset(NT, nombres[j]));
                    break;
                }
            }
        }

        if (esForward)
        {
            const char *destino =
                (const char *)(FILE + RvaOffset(NT, funciones[i]));

            printf("[%4u]  %s = %s\n",
                   ordinal,
                   nombre ? nombre : "NONAME",
                   destino);
        }
        else if (nombre)
            printf("[%4u]  %s\n", ordinal, nombre);
        else
            printf("[%4u]  NONAME_%u\n", ordinal, i);
    }
}

template void LeerExports<PIMAGE_NT_HEADERS32>(PBYTE, PIMAGE_NT_HEADERS32, DWORD);
template void LeerExports<PIMAGE_NT_HEADERS64>(PBYTE, PIMAGE_NT_HEADERS64, DWORD);