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
BearSSL::CertStore certStore;
#include <time.h>

#define WIFI_SSID "agnath18"
#define WIFI_PASSWORD "123456789"

#define HOST_URL "raw.githubusercontent.com";
#define VERS_URL "https://raw.githubusercontent.com/agnath18K/ESP8266_AutoFirmwareUpgrade/main/bin/version"
#define Firm_URL "https://raw.githubusercontent.com/agnath18K/ESP8266_AutoFirmwareUpgrade/main/bin/firmware.bin"

double Firm_Ver = 1.00;
const unsigned long Update_Interval = 60000;
const int httpsPort = 443;

unsigned long counter = 0;
double Lat_Ver;
int Error_Count=1;
const char* host = HOST_URL;

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

void Error_Con();
void Connect_WiFi();
void setClock();

void Error_Con() {
  Serial.print("\nError Detected.\nError Count : ");
  Serial.print(Error_Count);
  Error_Count+=1;
  if(Error_Count>3)  {
    Serial.print("\nProceeding ESP Restart In 60 Seconds\n");
    Error_Count=0;
    delay(60000);
    Serial.print("\n\nRestarting ESP\n\n");
    ESP.restart(); }
  else {
    Serial.println("\nRestarting WiFi Connection");
    Connect_WiFi(); } }

void Connect_WiFi() {
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 Serial.print("\n\nConnecting to Wi-Fi");
 int try_count=0;
 while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
    try_count+=1;
    if(try_count>=100) {
    Serial.println("\nConnection taking too long.");
    Error_Con(); }
  }
 Serial.println();
 Serial.print("Connected with IP: ");
 Serial.println(WiFi.localIP());
 Serial.println();
 setClock(); }

void setClock() {
  int ntp_count=0;
  configTime(0, 0, "pool.ntp.org", "time.google.com"); 
  Serial.print(F("\nWaiting for NTP time sync: "));
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
    ntp_count+=1;
    if(ntp_count>=100) {
    Serial.println("\nConnection taking too long.");
    Error_Con(); } } }
      
void Firmware_Update() {
 
 BearSSL::WiFiClientSecure client;
 client.setTrustAnchors(&cert);
 Serial.print("\nConnection to host : ");

 if (!client.connect(host, httpsPort)) {
    Serial.println("Failed");
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
  Serial.print("\nLatest Firmware Version   : ");
  Serial.println(Lat_Ver);
  if(Lat_Ver==404.00)
  Serial.println("\nFailed to retrieve Version file.\nSkipping Firmware Check\n");
  else {
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
        break; } } } }

void setup()
{
 Serial.begin(115200);
 Serial.print("\n");
 Connect_WiFi();
 setClock();
 }

 void loop()
 {
  if(millis() - counter >= Update_Interval) {
  Firmware_Update();
  counter = millis(); }
  }
