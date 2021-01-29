# ESP8266_AutoFirmwareUpgrade
Automatic OTA Firmware Upgrade For ESP8266.
Using Github as host Server.

-> How it works?

* This program download and flash the lastest bin file available in your GitHub Repository whenever the Firmware_Version of the arduino code detect a newer version number from the "version" file of your repo.

-> How to use?

* Export your code as bin file and store it in your repository.
* Create a text file in your repo with your Firmware version number.
* Copy the download link of bin file as well as the "version" text file and paste the same in the HOST_URL & VERS_URL.


* Using DigiCert High Assurance EV Root CA : Valid Until 2031

NOTE : 
Current code won't let you inside of loop() untill the WiFi & Host is connected successfully.
If your code contain a Offline part or if you don't have continuos Internet access then you have to make some modifications in the Error_Conn() function.
Delete the ESP.restart in the Error_Conn() and divert the code to your loop() function.
This way you can use the code for Offline activities.


  angath18 :)                                                        agnath18@gmail.com
