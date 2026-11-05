#ifndef GEMTRIO_VFS_H
#define GEMTRIO_VFS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define GEMTRIO_MOD_DIR       "./mods"
#define GEMTRIO_MAX_PATH      512
#define GEMTRIO_MAX_OVERRIDES 8192

typedef enum {
    ASSET_TYPE_SPRITE    = 0, // .png, .4bpp, .8bpp
    ASSET_TYPE_PALETTE   = 1, // .pal, .gbapal
    ASSET_TYPE_BINARY    = 2, // .bin, raw tilemaps
    ASSET_TYPE_SOUND     = 3, // .wav, .mid
    ASSET_TYPE_JSON_DATA = 4, // .json (base stats, encounters, trainers)
    ASSET_TYPE_UNKNOWN   = 255
} AssetType;

typedef struct {
    uint32_t  assetId;                     // FNV-1a hash of the asset key/name
    AssetType type;
    char      overridePath[GEMTRIO_MAX_PATH];
    size_t    fileSize;
    bool      isLoaded;
} VfsEntry;

// Core VFS API
void      VFS_Init(void);
VfsEntry *VFS_FindOverride(const char *assetPathOrName);
VfsEntry *VFS_FindOverrideById(uint32_t assetId);
uint32_t  VFS_HashAssetPath(const char *path);
void      VFS_Reload(void);
void      VFS_Shutdown(void);
int       VFS_GetOverrideCount(void);

#endif // GEMTRIO_VFS_H
