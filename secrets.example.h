#ifndef SECRETS_H
#define SECRETS_H

// Copy this file to secrets.h and fill in your values.
// secrets.h is listed in .gitignore and will never be committed.

// Wi-Fi credentials
const char *ssid     = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

// OTA update password (leave empty string to disable password protection).
// Must match what you enter in Arduino IDE / arduino-cli when uploading OTA.
#define OTA_PASSWORD "YOUR_OTA_PASSWORD"

#endif // SECRETS_H
