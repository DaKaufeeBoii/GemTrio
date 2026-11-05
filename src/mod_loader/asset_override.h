#ifndef GEMTRIO_ASSET_OVERRIDE_H
#define GEMTRIO_ASSET_OVERRIDE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    void   *buffer;       // Pointer to raw asset bytes
    size_t  size;         // Total byte length
    bool    isFromMod;    // true if loaded dynamically from /mods/ (caller owns memory)
} ModAsset;

#ifdef __cplusplus
extern "C" {
#endif

// Primary Hook specified by requirement:
// Intercepts standard asset fetching by checking /mods/ before loading baked-in data
void *LoadAsset_Override(char *assetPath);

// Extended Hook providing explicit size output and baked-in ROM fallback:
ModAsset LoadAsset_OverrideEx(const char *assetPath, const void *bakedData, size_t bakedSize);

// Releases buffer if allocated by the mod loader
void FreeAsset_Override(ModAsset *asset);

// Direct helper to free a pointer returned by LoadAsset_Override()
void FreeAssetBuffer(void *buffer);

#ifdef __cplusplus
}
#endif

#endif // GEMTRIO_ASSET_OVERRIDE_H
