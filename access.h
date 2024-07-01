#include "FS.h"
#include "SPIFFS.h"

#define FORMAT_SPIFFS_IF_FAILED true
#define ACCESS_MAX_SIZE 500

#define ACCESS_FILE "/esp32cam-access.json"

extern void dumpAccess(fs::FS &fs);
extern int loadAccess(fs::FS &fs);
extern void removeAccess(fs::FS &fs);
extern void saveAccess(fs::FS &fs);

extern void filesystemStart();
