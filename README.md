# NUSTARS Foundation 2018/19
For remote telemetry and data processing.

## Avionics
Language(s): C++  
Board: Feather M0  
Sensors:  
- BNO055  
- BMP280  
- Adafruit Ultimate GPS  
- ADXL377
Radio: LoRa RFM95W @915MHz
Commercial Altimeters: Raven 4

An ADS is controlled by interpreting signals from the commercial altimeters. General telemetry information is read from custom avionics suite, and is packaged and sent to ground station (as well as being saved to flash). At some point TBD, data will be retransmitted from flash (in the likely event of loss during the flight).

## Ground display and processing
Language(s): Python  
Radio: Feather M0 w/ LoRa RFM95W

The ground station is responsible for interpreting and displaying both real-time and asynchronous data from the avionics package.
Further information TBD
