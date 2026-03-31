// src/Memory/RvaOffset.cpp
#include "Memory/RvaOffset.hpp"

static DWORD _RvaOffset(PIMAGE_SECTION_HEADER seccion, DWORD cantidadSecciones, DWORD RVA)
{
    // Recorrer cada seccion de la tabla
    // Referencia: https://0xrick.github.io/images/wininternals/pe2/1.png
    // Referencia: https://skr1x.github.io/assets/portable-executable-format/pe_file.png
    for (WORD i = 0; i < cantidadSecciones; i++)
    {
        // Direccion de inicio y final de cada seccion de la tabla
        DWORD SeccionInicio = seccion[i].VirtualAddress;
        DWORD SeccionFinal = SeccionInicio + seccion[i].Misc.VirtualSize;

        // Verificar si el RVA esta dentro de esta seccion de la tabla
        if (RVA >= SeccionInicio && RVA < SeccionFinal)
            return seccion[i].PointerToRawData + (RVA - SeccionInicio);
    }

    return 0;
}

// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-image_section_header
// https://blog.deephacking.tech/es/posts/anatomia-del-formato-portable-executable/#nt-headers-image_nt_headers
DWORD RvaOffset(PIMAGE_NT_HEADERS32 &NT, DWORD RVA)
{
    return _RvaOffset(IMAGE_FIRST_SECTION(NT), NT->FileHeader.NumberOfSections, RVA);
}

DWORD RvaOffset(PIMAGE_NT_HEADERS64 &NT, DWORD RVA)
{
    return _RvaOffset(IMAGE_FIRST_SECTION(NT), NT->FileHeader.NumberOfSections, RVA);
}