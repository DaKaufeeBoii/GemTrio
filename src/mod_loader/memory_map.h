#ifndef GEMTRIO_MEMORY_MAP_H
#define GEMTRIO_MEMORY_MAP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Standard GBA address space boundaries
#define GBA_EWRAM_BASE       0x02000000u
#define GBA_EWRAM_SIZE       0x00040000u // 256 KB
#define GBA_IWRAM_BASE       0x03000000u
#define GBA_IWRAM_SIZE       0x00008000u // 32 KB
#define GBA_IO_BASE          0x04000000u
#define GBA_VRAM_BASE        0x06000000u
#define GBA_ROM_BASE         0x08000000u
#define GBA_ROM_MAX_SIZE     0x02000000u // 32 MB

// Emerald Recomp host memory bases
extern uint8_t *g_RecompEwramBase;
extern uint8_t *g_RecompIwramBase;
extern uint8_t *g_RecompRomBase;

// Memory mapping table entry
typedef struct {
    const char *symbolName;
    uint32_t    gbaVirtualAddress;
    void       *hostVirtualAddress;
    size_t      sizeBytes;
} RecompMemorySymbol;

// Translates a 32-bit GBA memory address into a valid host PC virtual pointer
void *GBA_ToHostAddress(uint32_t gbaAddress);

// Registers host pointers for EWRAM, IWRAM, and ROM
void  GBA_InitMemoryMap(uint8_t *ewram, uint8_t *iwram, uint8_t *rom);

// GameShark / Action Replay memory code applier
// Parses and executes standard GBA Action Replay / GameShark code lines:
// Format: XXXXXXXX YYYYYYYY
bool  GBA_ApplyGameSharkCode(uint32_t gbaAddress, uint32_t value, int byteWidth);

// Reads and applies batch cheats from a plain text file (e.g. /mods/data/cheats.txt)
int   GBA_LoadCheatsFile(const char *cheatsFilePath);

// Look up a symbol by GBA address
const RecompMemorySymbol *GBA_LookupSymbol(uint32_t gbaAddress);

#ifdef __cplusplus
}
#endif

#endif // GEMTRIO_MEMORY_MAP_H
