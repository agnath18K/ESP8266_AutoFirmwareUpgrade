/*
  
# ESP8266_AutoFirmwareUpgrade
Automatic OTA Firmware Upgrade For ESP8266.
Using Github as host Server.

-> How it works?

* This program download and flash the lastest bin file available in your GitHub Repository whenever the Firmware_Version of the arduino code detect a newer version number from the "version" file of your repo.

-> How to use?

* Export your code to bin file and store it in your repository.
* Create a text file in your repo with your Firmware version number.
* Copy the download link of bin file as well as the "version" text file and paste the same in the HOST_URL & VERS_URL.


* Using DigiCert High Assurance EV Root CA : Valid Until 2031


  agnath18@gmail.com
*/


#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>
#include <CertStoreBearSSL.h>
BearSSL::CertStore certStore;
#include <time.h>

#define WIFI_SSID "X18"
#define WIFI_PASSWORD "S3I5@k6e7"

#define HOST_URL "raw.githubusercontent.com";
#define VERS_URL "https://raw.githubusercontent.com/agnath18K/ESP8266_AutoFirmwareUpgrade/main/bin/version"
#define Firm_URL "https://raw.githubusercontent.com/agnath18K/ESP8266_AutoFirmwareUpgrade/main/bin/firmware.bin"

double Firm_Ver = 1.02;
double Lat_Ver;

const char* host = HOST_URL;
const int httpsPort = 443;

const char trustRoot[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j
ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL
MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3
LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug
RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm
+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW
PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM
xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB
Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3
hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg
EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF
MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA
FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec
nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z
eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF
hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2
Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe
vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep
+OkuE6N36B9K
-----END CERTIFICATE-----
)EOF";
X509List cert(trustRoot);

void Error_Con() {
    Serial.print("\nESP Restart In 30Seconds\n");
    delay(30000);
    Serial.print("\n\nRestarting ESP\n\n");
    ESP.restart(); }

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");  // UTC

  Serial.print(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
  } }
      
void Firmware_Update() {
  
 BearSSL::WiFiClientSecure client;
 client.setTrustAnchors(&cert);
 Serial.print("\nConnection to host : ");

 
 if (!client.connect(host, httpsPort)) {
    Serial.println("\nFailed");
    Error_Con();
    return; }
  Serial.print("Success\n");

  String url = VERS_URL;
  Serial.print("\nCurrent Firmware Version : ");
  Serial.print(Firm_Ver);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break; }
     }
  String line = client.readStringUntil('\n');
  Lat_Ver = line.toDouble();
  Serial.print("\nLatest Firmware Version : ");
  Serial.println(Lat_Ver);

  if(Firm_Ver>=Lat_Ver)
  Serial.println("\nDevice Running On Latest Firmware");
  else
  {
  Serial.println("\nNew Firmware Found!");
  Serial.println("\nStarting Upgrade\n");
  ESPhttpUpdate.setLedPin(LED_BUILTIN, LOW); 
  t_httpUpdate_return ret = ESPhttpUpdate.update(client, Firm_URL);

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("FIRMWARE_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("FIRMWARE_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("FIRMWARE_UPDATE_OK");
        break; } } }

void setup() {
 Serial.begin(115200);
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 Serial.print("\n\nConnecting to Wi-Fi");
 while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
 Serial.println();
 Serial.print("Connected with IP: ");
 Serial.println(WiFi.localIP());
 Serial.println();
 setClock(); }

 void loop() {
 Firmware_Update();
 delay(10000); }
