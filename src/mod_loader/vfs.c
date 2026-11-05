#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

static VfsEntry s_overrides[GEMTRIO_MAX_OVERRIDES];
static int      s_overrideCount = 0;

uint32_t VFS_HashAssetPath(const char *path)
{
    // FNV-1a 32-bit hash
    uint32_t hash = 2166136261u;
    for (const char *p = path; *p; p++)
    {
        char c = *p;
        // Normalize backslashes to forward slashes for cross-platform compatibility
        if (c == '\\') c = '/';
        hash ^= (uint8_t)c;
        hash *= 16777619u;
    }
    return hash;
}

static AssetType GetAssetTypeFromExt(const char *ext)
{
    if (!ext) return ASSET_TYPE_UNKNOWN;
    if (!strcmp(ext, "png") || !strcmp(ext, "4bpp") || !strcmp(ext, "8bpp"))
        return ASSET_TYPE_SPRITE;
    if (!strcmp(ext, "pal") || !strcmp(ext, "gbapal"))
        return ASSET_TYPE_PALETTE;
    if (!strcmp(ext, "json"))
        return ASSET_TYPE_JSON_DATA;
    if (!strcmp(ext, "wav") || !strcmp(ext, "mid"))
        return ASSET_TYPE_SOUND;
    if (!strcmp(ext, "bin") || !strcmp(ext, "raw"))
        return ASSET_TYPE_BINARY;
    return ASSET_TYPE_UNKNOWN;
}

static void ScanDirectory(const char *dirPath)
{
#ifdef _WIN32
    char searchPattern[GEMTRIO_MAX_PATH];
    snprintf(searchPattern, sizeof(searchPattern), "%s\\*", dirPath);

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPattern, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
        return;

    do
    {
        if (findData.cFileName[0] == '.')
            continue;

        if (s_overrideCount >= GEMTRIO_MAX_OVERRIDES)
            break;

        char fullPath[GEMTRIO_MAX_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s\\%s", dirPath, findData.cFileName);

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            ScanDirectory(fullPath);
            continue;
        }

        // Extract file extension and asset stem name
        char assetName[GEMTRIO_MAX_PATH];
        snprintf(assetName, sizeof(assetName), "%s", findData.cFileName);

        char *dot = strrchr(assetName, '.');
        const char *ext = dot ? dot + 1 : "";
        if (dot) *dot = '\0';

        VfsEntry *entry = &s_overrides[s_overrideCount++];
        entry->assetId = VFS_HashAssetPath(assetName);
        entry->type = GetAssetTypeFromExt(ext);
        entry->isLoaded = false;
        snprintf(entry->overridePath, sizeof(entry->overridePath), "%s", fullPath);

        // Calculate file size
        LARGE_INTEGER size;
        size.LowPart = findData.nFileSizeLow;
        size.HighPart = findData.nFileSizeHigh;
        entry->fileSize = (size_t)size.QuadPart;

    } while (FindNextFileA(hFind, &findData));

    FindClose(hFind);
#else
    DIR *dir = opendir(dirPath);
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && s_overrideCount < GEMTRIO_MAX_OVERRIDES)
    {
        if (entry->d_name[0] == '.')
            continue;

        char fullPath[GEMTRIO_MAX_PATH];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        struct stat st;
        if (stat(fullPath, &st) == 0 && S_ISDIR(st.st_mode))
        {
            ScanDirectory(fullPath);
            continue;
        }

        char assetName[GEMTRIO_MAX_PATH];
        snprintf(assetName, sizeof(assetName), "%s", entry->d_name);

        char *dot = strrchr(assetName, '.');
        const char *ext = dot ? dot + 1 : "";
        if (dot) *dot = '\0';

        VfsEntry *e = &s_overrides[s_overrideCount++];
        e->assetId = VFS_HashAssetPath(assetName);
        e->type = GetAssetTypeFromExt(ext);
        e->isLoaded = false;
        snprintf(e->overridePath, sizeof(e->overridePath), "%s", fullPath);
        e->fileSize = (size_t)st.st_size;
    }
    closedir(dir);
#endif
}

void VFS_Init(void)
{
    memset(s_overrides, 0, sizeof(s_overrides));
    s_overrideCount = 0;

    ScanDirectory(GEMTRIO_MOD_DIR "/sprites");
    ScanDirectory(GEMTRIO_MOD_DIR "/palettes");
    ScanDirectory(GEMTRIO_MOD_DIR "/data");
    ScanDirectory(GEMTRIO_MOD_DIR "/sounds");
    ScanDirectory(GEMTRIO_MOD_DIR);

    printf("[GemTrio VFS] Initialized with %d mod override(s) from %s\n", s_overrideCount, GEMTRIO_MOD_DIR);
}

VfsEntry *VFS_FindOverride(const char *assetPathOrName)
{
    if (!assetPathOrName) return NULL;

    // Direct match by hashed name (ignoring path/extension)
    const char *stem = strrchr(assetPathOrName, '/');
    if (!stem) stem = strrchr(assetPathOrName, '\\');
    stem = stem ? stem + 1 : assetPathOrName;

    char key[GEMTRIO_MAX_PATH];
    strncpy(key, stem, sizeof(key) - 1);
    key[sizeof(key) - 1] = '\0';
    char *dot = strrchr(key, '.');
    if (dot) *dot = '\0';

    uint32_t id = VFS_HashAssetPath(key);
    return VFS_FindOverrideById(id);
}

VfsEntry *VFS_FindOverrideById(uint32_t assetId)
{
    for (int i = 0; i < s_overrideCount; i++)
    {
        if (s_overrides[i].assetId == assetId)
            return &s_overrides[i];
    }
    return NULL;
}

int VFS_GetOverrideCount(void)
{
    return s_overrideCount;
}

void VFS_Reload(void)
{
    VFS_Shutdown();
    VFS_Init();
}

void VFS_Shutdown(void)
{
    s_overrideCount = 0;
}
