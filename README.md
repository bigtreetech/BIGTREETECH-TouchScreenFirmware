<!-- omit in toc -->

# BigTreeTech Touchscreen Firmware

![GitHub](https://img.shields.io/github/license/bigtreetech/bigtreetech-TouchScreenFirmware.svg)
[![GitHub contributors](https://img.shields.io/github/contributors/bigtreetech/bigtreetech-TouchScreenFirmware.svg)](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/graphs/contributors)
![GitHub Release Date](https://img.shields.io/github/release-date/bigtreetech/bigtreetech-TouchScreenFirmware.svg)
[![Build Status](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/workflows/Build%20Test/badge.svg?branch=master)](https://github.com/bigtreetech/bigtreetech-TouchScreenFirmware/actions)

Firmware for BigTreeTech's dual-mode touchscreen 3D printer controllers

<img width=500 src="https://user-images.githubusercontent.com/13375512/76691608-ae1b8780-6609-11ea-9ee1-421bcf09e538.png">

<!-- omit in toc -->

## Table of Contents

- [Connect the TFT to the Mainboard](#connect-the-tft-to-the-mainboard)
- [Menus and Themes](#menus-and-themes)
- [Update TFT Firmware](#update-tft-firmware)
- [Configuration](#configuration)
- [Customization](#customization)
  - [Bootscreen and Icons](#bootscreen-and-icons)
  - [Firmware](#firmware)
- [Troubleshooting](#troubleshooting)
- [Version History](#version-history)

## Connect the TFT to the Mainboard

**Do the following, to be able to use the touchmode of your screen.** 

1. Connect the 5pin serial cable according to the manual of your mainboard.
2. Define a serial port in Marlin, to activate the port used by the TFT.
3. Make sure the same BAUDRATE is defined in Marlin and the firmware of your TFT (config.ini)

**Do the following, to be able to use the Marlin emulation mode of your screen.** 

A: In case your TFT does **not** have an EXP connector at all (TFT28 v1 for example), you can not use the Marlin emulator mode.

B: In case your mainboard provides **EXP1 and EXP2**, you have to connect 2 ribbon cables connecting EXP1 and EXP2 of the mainboard to EXP1 and EXP2 of the TFT. In the Marlin firmware of your mainboard, make sure that **ONLY** the "REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER" is activated in Configuration.h and that all other controllers (especially the "CR10_STOCKDISPLAY") are **De**activated.

C: In case you have an **"E3" mainboard** which provides a **single EXP connector**, you have to connect 1 ribbon cable connecting EXP of the mainboard to **EXP3** of the TFT. In case your TFT does **not** provide an EXP3 connector but only two 10pin connectors (TFT24 v1.1 for example) you will need a "Y-split" cable with one 10pin connector on one side (for the mainboard) and two 10pin connectors on the other side (for the TFT). In the Marlin firmware of your mainboard, make sure that **ONLY** the "CR10_STOCKDISPLAY" is activated in Configuration.h and that all other controllers (especially the "REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER") are **De**activated.

## Menus

|                   Classic Menu Main Screen                   |                   Unified Menu Main Screen                   |
| :----------------------------------------------------------: | :----------------------------------------------------------: |
| ![Classic](https://user-images.githubusercontent.com/54359396/98740417-4d686880-23ab-11eb-873f-82c7d338a139.png) | ![Unified Material Main Screen](https://user-images.githubusercontent.com/54359396/98742038-03cd4d00-23ae-11eb-9552-36dc02fe66f4.png) |
| In config.ini define: General Settings<br/>UNIFIED MENU / CLASSIC MENU<br/># Select a UI Menu flavour<br/># Options: [Unified Menu: 1, Classic Menu: 0]<br/>**unified menu: 0** | In config.ini define: General Settings<br/>UNIFIED MENU / CLASSIC MENU<br/># Select a UI Menu flavour<br/># Options: [Unified Menu: 1, Classic Menu: 0]<br/>**unified menu: 1** |

## Themes

|                      Unified Menu Theme                      | Hybrid Red Material Theme by **[AntoszHUN](https://github.com/AntoszHUN)** | The Round Miracle Theme by **[Acenotass](https://github.com/Acenotass)** |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
| ![Unified Material Main Screen](https://user-images.githubusercontent.com/54359396/98742038-03cd4d00-23ae-11eb-9552-36dc02fe66f4.png) | ![Hybrid Red Menu Material Mainscreen](https://user-images.githubusercontent.com/54359396/98869176-a995c000-2471-11eb-94da-a0bc41abf3e9.png) | ![Round Miracle Main Screen](https://user-images.githubusercontent.com/54359396/99251566-d77e5a00-280d-11eb-9c7a-0e7c0111eedd.png) |
| Use firmware, icons, and fonts from the [`Copy to SD Card root directory to update - Unified Menu Material theme`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update%20-%20Unified%20Menu%20Material%20theme) folder | Use firmware, icons, and fonts from the [`Copy to SD Card root directory to update - Hybrid Red Material theme`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/5cc30fdc213646f9296f3c1c65b1c6be1b556478/Copy%20to%20SD%20Card%20root%20directory%20to%20update%20-%20Hybrid%20Red%20Menu%20Material%20theme) folder | Use firmware, icons, and fonts from the [`Copy to SD Card root directory to update - The Round Miracle theme`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/5cc30fdc213646f9296f3c1c65b1c6be1b556478/Copy%20to%20SD%20Card%20root%20directory%20to%20update%20-%20The%20Round%20Miracle) folder |

## Update TFT Firmware


TFT firmware updates are comprised of up to four elements:

1. The firmware binary (`BIGTREE_TFT*_V*.*.*.bin`). Example: `BIGTREE_TFT35_V3.0.26.1.bin`:
   - `BIGTREE_TFT_35`: model
   - `V3.0`: hardware version
   - `26.1`: software version
2. Fonts and Icons (`TFT*` folder):
   - `TFT*/font`: fonts
   - `TFT*/bmp`: icons
3. The config.ini file
4. **Optionally** a language file

Copy  the `BIGTREE_TFT*_V*.*.*.bin,the `TFT*` folder and the config.ini to the root of a blank SD card that is <8GB and formatted as FAT32:

![Firmware](https://user-images.githubusercontent.com/54359396/100600549-b6cffd00-3301-11eb-8b57-d56b7a4422f1.jpg)

**Optionally**, copy one or several language .ini file(s) from Copy to SD Card root directory to update/Language Packs [`Copy to SD Card root directory to update/Language Packs`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/8963aed84c40a3b8daf2768a7feb37aeeab71996/Copy%20to%20SD%20Card%20root%20directory%20to%20update%20) folder onto the SD card. Doing so will allow you to switch between English and the uploaded language(s), using the corresponding Language function of the TFT. We recommend to upload the minimum amount of languages, to keep the memory usage low. The language .ini file can be edited to change the text shown on the TFT.

![Language Pack](https://user-images.githubusercontent.com/54359396/100600564-b9caed80-3301-11eb-8997-d376f05323f6.jpg)


Place SD card with the `BIGTREE_TFT*_V*.*.*.bin`, the`TFT*` folder and the config.ini into the TFT's SD card reader and reset your TFT (or optionally - power cycle your printer) to start the update process.

<p align=center> ⚠️ Failing to update your icons &amp; fonts will result in missing icons and/or unreadable text ⚠️ </p>



## Update Process shown on TFT Screen

A successful update looks like this on the screen:

![Screenshot 2020-09-26 at 22 10 04](https://user-images.githubusercontent.com/54359396/94349526-5abcd400-0045-11eb-993a-afc5b241f4d7.png)

... and the name of the elements on the SD card changes to this:

![After Update](https://user-images.githubusercontent.com/54359396/94349755-dc156600-0047-11eb-9b1e-a1334bc5675f.png)

In case one or several parts of the update failed, an error will be shown. Follow the information on the screen to update the missing or outdated elements.

![Screenshot 2020-10-23 at 14 37 36](https://user-images.githubusercontent.com/54359396/97004894-002c7000-153e-11eb-9951-f493be46af3e.png)

<p align=center> ⚠️ Errors during the update can not be ignored and must be solved before using the TFT ⚠️ </p>

After the update is done and the files are renamed, it is possible to reuse them again. To do so, change the name of the element(s) to the pre-update name and start the update process again.

Remove the SD card from the TFT and restart the printer.




## Configuration

The Firmware can be configured using the **config.ini** file from:
[`Copy to SD Card root directory to update`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update)

### Editing configuration (config.ini) file

To edit the **config.ini** file follow the instruction here: [Detailed Instructions here](config_instructions.md)

### Updating Firmware Configuration

To update the Firmware configuration:

1. Edit the settings in **config.ini**.
2. Copy the **config.ini** file to the root of the SD card. (The SD card capacity should be less than or equal to 8GB and formatted as FAT32)
3. Insert the SD card into the TFT's SD card slot and restart the printer or press the reset buttion of the TFT.
4. The TFT will update and store the configuration from **config.ini** file.
5. Make sure to remove the SD card from the TFT and restart the printer.

The config.ini can be edited using a simple text editor (make sure to use UTF encoding) and uploaded again - without the need to upload the firmware or the TFT folder - as long as the firmware and the config.ini have the same version number.

## Customization

### Bootscreen and Icons

See [Customization guides](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/readme/) for detailed  information.

### Firmware

<details><summary>View full instructions</summary>
<ol>
<li>Setup Visual Studio Code with PlatformIO <a href="https://github.com/bigtreetech/Document/blob/master/How%20to%20install%20VScode+Platformio.md">instructions</a></li>
<li>Click on the PlatformIO icon (①) and then click on Open Project (②):
   <img src="https://user-images.githubusercontent.com/25599056/56637513-6b258e00-669e-11e9-9fad-d0571e57499e.png"></li>
<li>Find the BIGTREETECH  firmware source directory , then click Open:
   <img src="https://user-images.githubusercontent.com/25599056/56637532-77115000-669e-11e9-809b-f6bc25412f75.png"></li>
  <li>After opening the project, edit <a href="platformio.ini"><code>platformio.ini</code></a> and change the <code>default_envs</code> to one that matches your TFT model and version:
   <pre>;BIGTREE_TFT35_V1_0
;BIGTREE_TFT35_V1_1
;BIGTREE_TFT35_V1_2
;BIGTREE_TFT35_V2_0
;BIGTREE_TFT35_V3_0
;BIGTREE_TFT35_E3_V3_0
;BIGTREE_TFT28_V1_0
;BIGTREE_TFT28_V3_0
;BIGTREE_TFT24_V1_1
;MKS_32_V1_4
;MKS_32_V1_4_NOBL
;MKS_28_V1_0



[platformio]
src_dir      = TFT
boards_dir   = buildroot/boards
default_envs = BIGTREE_TFT35_V3_0</pre></li>
  <li>Click the check mark (✓) at the bottom of VSCode or press <code>Ctrl</code>+<code>Alt</code>+<code>B</code> (Windows) / <code>Ctrl</code>+<code>Option</code>+<code>B</code> (macOS) to compile.

<img src="https://user-images.githubusercontent.com/25599056/56637550-809ab800-669e-11e9-99d3-6b502e294688.png"></li>
<li>A <code>BIGTREE_TFT*_V*.*.*.bin</code> file will be generated in the <em>hidden</em> <code>.pio\build\BIGTREE_TFT*_V*_*</code> folder. Follow the update process outlined in the <a href="#about-tft-firmware">About TFT Firmware</a> section above to update your TFT to the latest version.</li>
</details>

## Troubleshooting

To reset the TFT's touch screen calibration, create a blank file named `reset.txt` and place in root folder of the sd card. Insert the SD card into the TFT's SD card reader and power cycle your printer to start the reset process.

## Version History

See [BIGTREETECH-TouchScreenFirmware/releases](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/releases) for a complete version history.