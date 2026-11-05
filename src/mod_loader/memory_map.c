#include "memory_map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fallback host buffers if running independently of recomp runtime
static uint8_t s_DefaultEwram[GBA_EWRAM_SIZE];
static uint8_t s_DefaultIwram[GBA_IWRAM_SIZE];

uint8_t *g_RecompEwramBase = s_DefaultEwram;
uint8_t *g_RecompIwramBase = s_DefaultIwram;
uint8_t *g_RecompRomBase   = NULL;

// Static offset table anchoring key pokeemerald symbols to traditional GBA addresses
static const RecompMemorySymbol s_SymbolTable[] = {
    { "gSaveBlock2Ptr",       0x03005D8C, NULL, 4 },
    { "gSaveBlock1Ptr",       0x03005D90, NULL, 4 },
    { "gPokemonStoragePtr",   0x03005D94, NULL, 4 },
    { "gMain",                0x030022C0, NULL, 0x438 },
    { "gBattleOutcome",       0x02024E04, NULL, 1 },
    { "gBattleTypeFlags",     0x02022FAC, NULL, 4 },
    { "gSpeciesInfo",         0x083203CC, NULL, 0x10000 },
    { "gTrainers",            0x08310000, NULL, 0x20000 },
    { "gWildMonHeaders",      0x0845EE10, NULL, 0x8000 }
};

static const int s_SymbolTableCount = sizeof(s_SymbolTable) / sizeof(s_SymbolTable[0]);

void GBA_InitMemoryMap(uint8_t *ewram, uint8_t *iwram, uint8_t *rom)
{
    if (ewram) g_RecompEwramBase = ewram;
    if (iwram) g_RecompIwramBase = iwram;
    if (rom)   g_RecompRomBase   = rom;

    printf("[GemTrio Memory] Initialized host address translator: EWRAM=%p, IWRAM=%p, ROM=%p\n",
           (void*)g_RecompEwramBase, (void*)g_RecompIwramBase, (void*)g_RecompRomBase);
}

void *GBA_ToHostAddress(uint32_t gbaAddress)
{
    // EWRAM (0x02000000 - 0x0203FFFF)
    if (gbaAddress >= GBA_EWRAM_BASE && gbaAddress < (GBA_EWRAM_BASE + GBA_EWRAM_SIZE))
    {
        uint32_t offset = gbaAddress - GBA_EWRAM_BASE;
        return (void*)(g_RecompEwramBase + offset);
    }

    // IWRAM (0x03000000 - 0x03007FFF)
    if (gbaAddress >= GBA_IWRAM_BASE && gbaAddress < (GBA_IWRAM_BASE + GBA_IWRAM_SIZE))
    {
        uint32_t offset = gbaAddress - GBA_IWRAM_BASE;
        return (void*)(g_RecompIwramBase + offset);
    }

    // ROM (0x08000000 - 0x09FFFFFF)
    if (gbaAddress >= GBA_ROM_BASE && gbaAddress < (GBA_ROM_BASE + GBA_ROM_MAX_SIZE))
    {
        if (!g_RecompRomBase) return NULL;
        uint32_t offset = gbaAddress - GBA_ROM_BASE;
        return (void*)(g_RecompRomBase + offset);
    }

    return NULL;
}

const RecompMemorySymbol *GBA_LookupSymbol(uint32_t gbaAddress)
{
    for (int i = 0; i < s_SymbolTableCount; i++)
    {
        if (s_SymbolTable[i].gbaVirtualAddress == gbaAddress)
            return &s_SymbolTable[i];
    }
    return NULL;
}

bool GBA_ApplyGameSharkCode(uint32_t gbaAddress, uint32_t value, int byteWidth)
{
    void *hostPtr = GBA_ToHostAddress(gbaAddress);
    if (!hostPtr)
    {
        fprintf(stderr, "[GemTrio GS] Failed to translate GBA address 0x%08X to host pointer\n", gbaAddress);
        return false;
    }

    switch (byteWidth)
    {
        case 1:
            *(uint8_t*)hostPtr = (uint8_t)(value & 0xFF);
            break;
        case 2:
            *(uint16_t*)hostPtr = (uint16_t)(value & 0xFFFF);
            break;
        case 4:
            *(uint32_t*)hostPtr = value;
            break;
        default:
            memcpy(hostPtr, &value, (size_t)byteWidth);
            break;
    }

    return true;
}

int GBA_LoadCheatsFile(const char *cheatsFilePath)
{
    FILE *f = fopen(cheatsFilePath, "r");
    if (!f) return 0;

    char line[128];
    int applied = 0;

    while (fgets(line, sizeof(line), f))
    {
        // Skip comment lines and blanks
        if (line[0] == '#' || line[0] == ';' || line[0] == '\r' || line[0] == '\n')
            continue;

        uint32_t addr = 0;
        uint32_t val = 0;
        int width = 2; // Default 16-bit

        // Format: ADDR VALUE [WIDTH]
        if (sscanf(line, "%x %x %d", &addr, &val, &width) >= 2)
        {
            if (GBA_ApplyGameSharkCode(addr, val, width))
                applied++;
        }
    }

    fclose(f);
    printf("[GemTrio GS] Applied %d GameShark memory patch(es) from %s\n", applied, cheatsFilePath);
    return applied;
}
