#include "asset_override.h"
#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *ReadFileToHeap(const char *path, size_t *outSize)
{
    FILE *file = fopen(path, "rb");
    if (!file)
        return NULL;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    if (size <= 0)
    {
        fclose(file);
        return NULL;
    }

    void *buffer = malloc((size_t)size);
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    size_t readBytes = fread(buffer, 1, (size_t)size, file);
    fclose(file);

    if (readBytes != (size_t)size)
    {
        free(buffer);
        return NULL;
    }

    if (outSize)
        *outSize = (size_t)size;

    return buffer;
}

void *LoadAsset_Override(char *assetPath)
{
    if (!assetPath)
        return NULL;

    // 1. Query the Virtual File System for an override in the PC's /mods/ directory
    VfsEntry *entry = VFS_FindOverride(assetPath);
    if (entry != NULL)
    {
        size_t size = 0;
        void *modData = ReadFileToHeap(entry->overridePath, &size);
        if (modData != NULL)
        {
            printf("[GemTrio MOD] Overriding asset '%s' with '%s' (%u bytes)\n",
                   assetPath, entry->overridePath, (unsigned int)size);
            return modData;
        }
        else
        {
            fprintf(stderr, "[GemTrio MOD] Failed reading override file '%s' for '%s'\n",
                    entry->overridePath, assetPath);
        }
    }

    // 2. Direct fallback check for explicit relative paths under /mods/
    char modDirectPath[GEMTRIO_MAX_PATH];
    snprintf(modDirectPath, sizeof(modDirectPath), "%s/%s", GEMTRIO_MOD_DIR, assetPath);
    size_t directSize = 0;
    void *directData = ReadFileToHeap(modDirectPath, &directSize);
    if (directData != NULL)
    {
        printf("[GemTrio MOD] Direct path override loaded for '%s' (%u bytes)\n", modDirectPath, (unsigned int)directSize);
        return directData;
    }

    // Return NULL so caller falls back to baked ROM data
    return NULL;
}

ModAsset LoadAsset_OverrideEx(const char *assetPath, const void *bakedData, size_t bakedSize)
{
    ModAsset result;
    result.buffer = NULL;
    result.size = 0;
    result.isFromMod = false;

    if (assetPath)
    {
        void *overrideData = LoadAsset_Override((char*)assetPath);
        if (overrideData != NULL)
        {
            VfsEntry *entry = VFS_FindOverride(assetPath);
            result.buffer = overrideData;
            result.size = entry ? entry->fileSize : 0;
            result.isFromMod = true;
            return result;
        }
    }

    // Fall back to original ROM baked asset
    result.buffer = (void*)bakedData;
    result.size = bakedSize;
    result.isFromMod = false;
    return result;
}

void FreeAsset_Override(ModAsset *asset)
{
    if (asset && asset->isFromMod && asset->buffer)
    {
        free(asset->buffer);
        asset->buffer = NULL;
        asset->size = 0;
        asset->isFromMod = false;
    }
}

void FreeAssetBuffer(void *buffer)
{
    if (buffer)
        free(buffer);
}
