# ESP8266 Automatic Firmware Upgrade

Automatically update the firmware on your ESP8266 using GitHub as the host server.

## How it works

This program is designed to download and flash the latest binary file available in your GitHub Repository whenever the firmware version of the Arduino code detects a newer version number from the "version" file in your repository.

## How to use

Follow these steps to use this OTA firmware upgrade system:

1. **Export Your Firmware as a Binary File**:
   Export your Arduino code as a binary (.bin) file and store it in your GitHub repository.

2. **Create a Version File**:
   Create a text file in your GitHub repository and name it "version". This file should contain your firmware version number.

3. **Get GitHub URLs**:
   - Copy the download link of the binary firmware file.
   - Copy the URL of the "version" text file in your GitHub repository.

4. **Edit the Arduino Code**:
   - Open the Arduino code for your ESP8266.
   - Paste the URLs you copied in the previous step:
     - Set the download link of the binary firmware file as the `HOST_URL`.
     - Set the URL of the "version" text file as the `VERS_URL`. Make sure these URLs point to the raw content of the files on GitHub.

5. **Ensure Internet Connectivity**:
   Ensure that your ESP8266 is connected to the internet so it can access the GitHub repository to check for firmware updates and perform OTA upgrades when necessary.

## Additional Information

- This program uses the DigiCert High Assurance EV Root CA for secure connections. Make sure your ESP8266 supports HTTPS if you use this CA certificate.
- Maintain a specific structure in your GitHub repository where the version number is stored in the "version" text file, and the binary firmware file is available for download.

Feel free to reach out to agnath18, for any questions or assistance.

---

**Note:** Always handle OTA firmware updates with caution. Ensure the security and reliability of your code and connections to prevent any unexpected behavior during updates.
