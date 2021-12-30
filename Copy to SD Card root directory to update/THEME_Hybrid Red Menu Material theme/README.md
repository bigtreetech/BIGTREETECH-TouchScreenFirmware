### From version 26.1 the firmware for classic Menu and Unified Menu is merged.

**Now Classic Menu or Unified Menu can be enabled using the config.ini file without the need to update the firmware**

# V1 - TFT35_V3_E3*****.bin

## Update TFT Firmware

TFT firmware updates are comprised of two parts:

1. The firmware binary (`BIGTREE_TFT*_V*.*.*.bin`). Example: `BIGTREE_TFT35_V3.0.25.2.bin`:
     - `BIGTREE_TFT_35`: model
     - `V3.0`: hardware version
     - `25.2`: software version
2. Fonts and Icons (`TFT*` folder):
   - `TFT*/font`: fonts
   - `TFT*/bmp`: icons

Copy both the `BIGTREE_TFT*_V*.*.*.bin` and `TFT*` folder to the root of a blank SD card that is <8GB and formatted as FAT32:

![image](https://user-images.githubusercontent.com/13375512/76688998-3e989e80-65ef-11ea-93df-29a8216475a6.png)

Place SD card with `BIGTREE_TFT*_V*.*.*.bin` &amp; `TFT*` folder into the TFT's SD card reader and power cycle your printer to start the update process.

<p align=center> ⚠️ Failing to update your icons &amp; fonts will result in missing icons and/or unreadable text ⚠️ </p>

## Configuration
The Firmware can be configured using the **config.ini** file from from one of these folders:
[`Copy to SD Card root directory to update`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update) or
[`Copy to SD Card root directory to update - Unified Menu Material theme`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update%20-%20Unified%20Menu%20Material%20theme)

### Editing configuration (config.ini) file
To edit the **config.ini** file follow the instruction here: [Detailed Instructions here](config_instructions.md)
