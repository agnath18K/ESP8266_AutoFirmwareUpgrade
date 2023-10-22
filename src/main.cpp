/*
# ESP8266_AutoFirmwareUpgrade
Automatic OTA Firmware Upgrade For ESP8266.
agnath18@gmail.com

Using Github as host Server.

-> How it works?

* This program download and flash the lastest bin file available in your GitHub Repository whenever the Firmware_Version of the arduino code detect a newer version number from the "version" file of your repo.

-> How to use?

* Export your code as bin file and store it in your repository.
* Create a text file in your repo with your Firmware version number.
* Copy the download link of bin file as well as the "version" text file and paste the same in the HOST_URL & VERS_URL.


* Using DigiCert High Assurance EV Root CA : Valid Until 2031

  agnath18 :) 

*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <CertStoreBearSSL.h>
#include <time.h>

BearSSL::CertStore certStore;
#include "TrustRoot.h" 

#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define HOST_URL "your-github-host-url" 
#define VERS_URL "https://raw.githubusercontent.com/your-username/your-repo/main/bin/version"
#define FIRM_URL "https://raw.githubusercontent.com/your-username/your-repo/main/bin/firmware.bin"

const char *host = HOST_URL;
const int httpsPort = 443;
const unsigned long UpdateInterval = 60000; 

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);
  Serial.println("\nStarting ESP8266 Auto Firmware Upgrade");
  ConnectWiFi();
  configTime(0, 0, "pool.ntp.org", "time.google.com");
}

void loop() {
  unsigned long currentMillis = millis();
  static unsigned long previousMillis = 0;

  if (currentMillis - previousMillis >= UpdateInterval) {
    previousMillis = currentMillis;
    CheckAndUpdateFirmware();
  }
}

void ConnectWiFi() {
  Serial.println("\nConnecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void CheckAndUpdateFirmware() {
  Serial.println("Checking for firmware updates...");

  if (client.connect(host, httpsPort)) {
    Serial.println("Connected to host");
    client.setTrustAnchors(&certStore);
    HTTPClient http;
    http.begin(client, VERS_URL);

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      double latestVersion = http.getString().toDouble();
      Serial.print("Current Firmware Version: ");
      Serial.println(Firm_Ver);
      Serial.print("Latest Firmware Version: ");
      Serial.println(latestVersion);

      if (latestVersion > Firm_Ver) {
        Serial.println("New firmware available. Updating...");
        PerformFirmwareUpdate();
      } else {
        Serial.println("Device is up-to-date.");
      }
    } else {
      Serial.println("Failed to retrieve version file.");
    }
    http.end();
  } else {
    Serial.println("Connection to host failed.");
  }
}

void PerformFirmwareUpdate() {
  Serial.println("Starting firmware update...");
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, FIRM_URL);

  if (ret == HTTP_UPDATE_OK) {
    Serial.println("Firmware updated successfully.");
    ESP.restart();
  } else {
    Serial.print("Firmware update failed. Error code: ");
    Serial.println(ret);
  }
}
