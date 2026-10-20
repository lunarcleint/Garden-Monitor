# ASU West Valley Garden Monitoring 

![West Valley Gardens Logo](./assets/west-valley-gardens-logo-halfsize.svg)

This contains prototyped code for the West Valley Garden Monitoring Sensors using a Arudino R4 WIFI. We measure Moisture, Tempreature, and NPK readings.

This project is designed to upload data directly into a Google Sheet to allow easy consupmtion and graphing of data collected, you can find the spreadsheet here.

[WILL BE UPDATED WHEN SPREADSHEET IS CREATED](https://google.com)

# Support the Garden
- Visit their website: [West Valley Garden Website](https://sites.google.com/asu.edu/wvgardens/home?pli=1&authuser=0)
- Join the collectives: [Garden Collectives Google Form](https://docs.google.com/forms/d/e/1FAIpQLSfNHjtANDLSJeDhMX6EAnx__SMTXAGMZ6BFAb1TGR2Uh8upAg/viewform)

# How to Build / Upload
You can use any arudino build tool like the IDE as long as you can install ArduinoJson, ArduinoHttpClient, and NTPClient.

### IMPORTANT NOTE:
WIFICredentials.h is not included for security reasons, create the file and paste this in:
```cpp
#define SECRET_WIFI_NAME "NAME" // Network Name
#define SECRET_WIFI_PASS "PASSWORD" // Network Password

#define SHEET_DB_ENDPOINT "sheetdb.io"
#define SHEET_DB_ROUTE "/api/v1/API_CODE"
#define NTP_SERVER "pool.ntp.org"
```
 
Arduino-cli is reccomened since it makes installing everything very easy to setup.

- Follow the instructions to download and [install arudino-cli](https://docs.arduino.cc/arduino-cli/installation/#download).
- Run these commands in a terminal: 
```sh
arduino-cli core update-index # Update Index
arduino-cli core install arduino:renesas_uno # Install R4 Wifi board
arduino-cli lib install ArduinoJson # Install JSON support
arduino-cli lib install ArduinoHttpClient # Install Http support
arduino-cli lib install NTPClient # Install NTP support
```
- Now to build open a shell in the root of this project and run: 
```sh
arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi . 
```
- Now to upload first find the port that your arudino is on: 
```sh
arduino-cli board list
```
Then run (make sure you fill in your port):
```sh
arduino-cli upload -p <YOUR PORT> --fqbn arduino:renesas_uno:unor4wifi .
```
For example:
```
> arduino-cli board list
Port       Protocol Type        Board Name FQBN Core
/dev/ttyS0 serial   Serial Port Unknown

> arduino-cli upload -p /dev/ttyS0 --fqbn arduino:renesas_uno:unor4wifi .
```
