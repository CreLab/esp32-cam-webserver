#include "esp_camera.h"
#include "src/jsonlib/jsonlib.h"
#include "access.h"

extern void flashLED(int flashtime);

extern char access_ssid[];
extern char access_password[];

/*
 * Useful utility when debugging...
 */

void dumpAccess(fs::FS &fs){
  if (fs.exists(ACCESS_FILE)) {
    // Dump contents for debug
    File file = fs.open(ACCESS_FILE, FILE_READ);
    int countSize = 0;
    while (file.available() && countSize <= ACCESS_MAX_SIZE) {
        Serial.print(char(file.read()));
        countSize++;
    }
    Serial.println("");
    file.close();
  } else {
    Serial.printf("%s not found, nothing to dump.\r\n", ACCESS_FILE);
  }
}

int loadAccess(fs::FS &fs){
  if (fs.exists(ACCESS_FILE)) {
    // read file into a string
    String access_conf;
    Serial.printf("Loading access from file %s\r\n", ACCESS_FILE);
    File file = fs.open(ACCESS_FILE, FILE_READ);
    if (!file) {
      Serial.println("Failed to open access file for reading, maybe corrupt, removing");
      removeAccess(SPIFFS);
      return 1;
    }
    size_t size = file.size();
    if (size > ACCESS_MAX_SIZE) {
      Serial.println("Access file size is too large, maybe corrupt, removing");
      removeAccess(SPIFFS);
      return 1;
    }
    while (file.available()) {
        access_conf += char(file.read());
        if (access_conf.length() > size) {
          // corrupted SPIFFS files can return data beyond their declared size.
          Serial.println("Access file failed to load properly, appears to be corrupt, removing");
          removeAccess(SPIFFS);
          return 1;
        }
    }

    String str;
    char charArray[64] = {0};
    int len = 0;
    char* token = NULL;

    // process camera settings
    Serial.println("ssid");
    str = jsonExtract(access_conf, "ssid");
    str.toCharArray(access_ssid, 32);
    Serial.println(access_ssid);

    Serial.println("password");
    str = jsonExtract(access_conf, "password");
    str.toCharArray(access_password, 32);
    Serial.println(access_password);

    // close the file
    file.close();
    dumpAccess(SPIFFS);
    return 0;
  } else {
    Serial.printf("Access file %s not found; using system defaults.\r\n", ACCESS_FILE);
    return 1;
  }
}

void saveAccess(fs::FS &fs){
  if (fs.exists(ACCESS_FILE)) {
    Serial.printf("Updating %s\r\n", ACCESS_FILE);
  } else {
    Serial.printf("Creating %s\r\n", ACCESS_FILE);
  }

  File file = fs.open(ACCESS_FILE, FILE_WRITE);
  static char json_response[1024];
  char * p = json_response;
  *p++ = '{';
  p+=sprintf(p, "\"ssid\":\"%s\",", access_ssid);
  p+=sprintf(p, "\"password\":%s,", access_password);
  *p++ = '}';
  *p++ = 0;
  file.print(json_response);
  file.close();
  dumpAccess(SPIFFS);
}

void removeAccess(fs::FS &fs) {
  if (fs.exists(ACCESS_FILE)) {
    Serial.printf("Removing %s\r\n", ACCESS_FILE);
    if (!fs.remove(ACCESS_FILE)) {
      Serial.println("Error removing access");
    }
  } else {
    Serial.println("No saved access file to remove");
  }
}
