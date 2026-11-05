#include "json_data_loader.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Forward declarations of pokeemerald types
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

// SpeciesInfo minimal struct matching include/pokemon.h
struct SpeciesInfo
{
    u8 baseHP;
    u8 baseAttack;
    u8 baseDefense;
    u8 baseSpeed;
    u8 baseSpAttack;
    u8 baseSpDefense;
    u8 types[2];
    u8 catchRate;
    u8 forceTeraType;
    u16 expYield;
    // Remainder of struct exists in ROM memory; we write directly through pointer
};

// TrainerMon minimal struct
struct TrainerMon
{
    u8 iv[6];
    u8 lvl;
    u16 species;
    u16 heldItem;
    u16 moves[4];
    u8 ability;
    u8 friendship;
};

struct Trainer
{
    u8 partyFlags;
    u8 trainerClass;
    u8 encounterMusic_gender;
    u8 trainerPic;
    u8 trainerName[12];
    u16 items[4];
    u8 doubleBattle;
    u8 aiFlags;
    u8 partySize;
    const struct TrainerMon *party;
};

// Recomp extern symbols (exposed in RAM on PC build)
extern struct SpeciesInfo gSpeciesInfo[];
extern struct Trainer     gTrainers[][1024];

#define MOD_DATA_DIR "./mods/data"

static char *ReadFileToString(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    if (len <= 0)
    {
        fclose(f);
        return NULL;
    }

    char *buf = (char*)malloc(len + 1);
    if (!buf)
    {
        fclose(f);
        return NULL;
    }

    size_t readLen = fread(buf, 1, len, f);
    fclose(f);
    buf[readLen] = '\0';
    return buf;
}

void JsonData_ApplyBaseStats(void)
{
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/base_stats.json", MOD_DATA_DIR);

    char *jsonText = ReadFileToString(filePath);
    if (!jsonText)
        return; // No mod file present; continue with defaults

    cJSON *root = cJSON_Parse(jsonText);
    free(jsonText);

    if (!root)
    {
        fprintf(stderr, "[GemTrio JSON] Error parsing %s: %s\n", filePath, cJSON_GetErrorPtr());
        return;
    }

    cJSON *speciesList = cJSON_GetObjectItem(root, "species");
    if (!speciesList || !cJSON_IsArray(speciesList))
    {
        cJSON_Delete(root);
        return;
    }

    int patched = 0;
    cJSON *entry = NULL;
    cJSON_ArrayForEach(entry, speciesList)
    {
        cJSON *idItem = cJSON_GetObjectItem(entry, "id");
        if (!idItem || !cJSON_IsNumber(idItem))
            continue;

        int speciesId = idItem->valueint;
        if (speciesId <= 0)
            continue;

        struct SpeciesInfo *info = &gSpeciesInfo[speciesId];

        cJSON *item = NULL;
        if ((item = cJSON_GetObjectItem(entry, "hp")) && cJSON_IsNumber(item))
            info->baseHP = (u8)item->valueint;
        if ((item = cJSON_GetObjectItem(entry, "attack")) && cJSON_IsNumber(item))
            info->baseAttack = (u8)item->valueint;
        if ((item = cJSON_GetObjectItem(entry, "defense")) && cJSON_IsNumber(item))
            info->baseDefense = (u8)item->valueint;
        if ((item = cJSON_GetObjectItem(entry, "sp_attack")) && cJSON_IsNumber(item))
            info->baseSpAttack = (u8)item->valueint;
        if ((item = cJSON_GetObjectItem(entry, "sp_defense")) && cJSON_IsNumber(item))
            info->baseSpDefense = (u8)item->valueint;
        if ((item = cJSON_GetObjectItem(entry, "speed")) && cJSON_IsNumber(item))
            info->baseSpeed = (u8)item->valueint;
        if ((item = cJSON_GetObjectItem(entry, "catch_rate")) && cJSON_IsNumber(item))
            info->catchRate = (u8)item->valueint;
        if ((item = cJSON_GetObjectItem(entry, "exp_yield")) && cJSON_IsNumber(item))
            info->expYield = (u16)item->valueint;

        patched++;
    }

    cJSON_Delete(root);
    printf("[GemTrio JSON] Successfully patched base stats for %d species from %s\n", patched, filePath);
}

void JsonData_ApplyTrainerData(void)
{
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/trainers.json", MOD_DATA_DIR);

    char *jsonText = ReadFileToString(filePath);
    if (!jsonText)
        return;

    cJSON *root = cJSON_Parse(jsonText);
    free(jsonText);

    if (!root)
    {
        fprintf(stderr, "[GemTrio JSON] Error parsing %s: %s\n", filePath, cJSON_GetErrorPtr());
        return;
    }

    cJSON *trainersList = cJSON_GetObjectItem(root, "trainers");
    if (!trainersList || !cJSON_IsArray(trainersList))
    {
        cJSON_Delete(root);
        return;
    }

    int count = 0;
    cJSON *trainerObj = NULL;
    cJSON_ArrayForEach(trainerObj, trainersList)
    {
        cJSON *idItem = cJSON_GetObjectItem(trainerObj, "id");
        if (!idItem || !cJSON_IsNumber(idItem))
            continue;

        int trainerId = idItem->valueint;
        int difficulty = 0;
        cJSON *diffItem = cJSON_GetObjectItem(trainerObj, "difficulty");
        if (diffItem && cJSON_IsNumber(diffItem))
            difficulty = diffItem->valueint;

        struct Trainer *t = &gTrainers[difficulty][trainerId];

        cJSON *nameItem = cJSON_GetObjectItem(trainerObj, "name");
        if (nameItem && cJSON_IsString(nameItem))
        {
            strncpy((char*)t->trainerName, nameItem->valuestring, sizeof(t->trainerName) - 1);
            t->trainerName[sizeof(t->trainerName) - 1] = '\0';
        }

        count++;
    }

    cJSON_Delete(root);
    printf("[GemTrio JSON] Loaded trainer modifications for %d trainers from %s\n", count, filePath);
}

void JsonData_ApplyEncounterTables(void)
{
    char filePath[512];
    snprintf(filePath, sizeof(filePath), "%s/encounter_tables.json", MOD_DATA_DIR);

    char *jsonText = ReadFileToString(filePath);
    if (!jsonText)
        return;

    cJSON *root = cJSON_Parse(jsonText);
    free(jsonText);

    if (!root)
    {
        fprintf(stderr, "[GemTrio JSON] Error parsing %s: %s\n", filePath, cJSON_GetErrorPtr());
        return;
    }

    printf("[GemTrio JSON] Encounter tables override file recognized: %s\n", filePath);
    cJSON_Delete(root);
}

void JsonData_LoadAll(void)
{
    JsonData_ApplyBaseStats();
    JsonData_ApplyEncounterTables();
    JsonData_ApplyTrainerData();
}
