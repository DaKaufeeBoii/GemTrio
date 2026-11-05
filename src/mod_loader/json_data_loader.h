#ifndef GEMTRIO_JSON_DATA_LOADER_H
#define GEMTRIO_JSON_DATA_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

// Loads all JSON configuration files found in /mods/data/
void JsonData_LoadAll(void);

// Specific loader modules
void JsonData_ApplyBaseStats(void);
void JsonData_ApplyEncounterTables(void);
void JsonData_ApplyTrainerData(void);

#ifdef __cplusplus
}
#endif

#endif // GEMTRIO_JSON_DATA_LOADER_H
