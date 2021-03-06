<!-- omit in toc -->

# BigTreeTech TFT Touchscreen

![GitHub](https://img.shields.io/github/license/bigtreetech/bigtreetech-TouchScreenFirmware.svg)
[![GitHub contributors](https://img.shields.io/github/contributors/bigtreetech/bigtreetech-TouchScreenFirmware.svg)](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/graphs/contributors)
![GitHub Release Date](https://img.shields.io/github/release-date/bigtreetech/bigtreetech-TouchScreenFirmware.svg)
[![Build Status](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/workflows/Build%20Test/badge.svg?branch=master)](https://github.com/bigtreetech/bigtreetech-TouchScreenFirmware/actions)

Important information related to BigTreeTech's TFT touchscreen 3D printer controllers

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

- [Appendix](#appendix)

  - [Setup of BTT TFT35 E3 V3.0 with MKS SGEN_L Mainboards](#Setup-of-BTT-TFT35-E3-V3.0-with-MKS-SGEN-L-Mainboards)
  - [Show more statistics at the end of the print](#show-more-statistics-at-the-end-of-the-print)

  

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

|                    Status Screen DISABLED                    |                    Status Screen ENABLED                     |
| :----------------------------------------------------------: | :----------------------------------------------------------: |
| ![status screen 0](https://user-images.githubusercontent.com/54359396/103319145-09035b80-4a31-11eb-91d0-dd761a48b6f5.png) | ![Unified Material Main Screen](https://user-images.githubusercontent.com/54359396/98742038-03cd4d00-23ae-11eb-9552-36dc02fe66f4.png) |
| In config.ini define: General Settings<br/>Enable Status Screen<br/># Select the Main Screen flavour<br/># Options: [Enable: 1, Disable: 0]<br/>**status_screen: 0** | In config.ini define: General Settings<br/>Enable Status Screen<br/># Select the Main Screen flavour<br/># Options: [Enable: 1, Disable: 0]<br/>**status_screen: 1** |



## Themes

|                      Unified Menu Theme                      | Hybrid Red Material Theme by **[AntoszHUN](https://github.com/AntoszHUN)** | The Round Miracle Theme by **[Acenotass](https://github.com/Acenotass)** |
| :----------------------------------------------------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
| ![Unified Material Main Screen](https://user-images.githubusercontent.com/54359396/98742038-03cd4d00-23ae-11eb-9552-36dc02fe66f4.png) | ![Hybrid Red Menu Material Mainscreen](https://user-images.githubusercontent.com/54359396/98869176-a995c000-2471-11eb-94da-a0bc41abf3e9.png) | ![Round Miracle Main Screen](https://user-images.githubusercontent.com/54359396/99251566-d77e5a00-280d-11eb-9c7a-0e7c0111eedd.png) |
| Use firmware, icons, and fonts from the [`Copy to SD Card root directory to update - Unified Menu Material theme`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update/THEME_Unified%20Menu%20Material%20theme) folder | Use firmware, icons, and fonts from the [`Copy to SD Card root directory to update - Hybrid Red Material theme`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update/THEME_Hybrid%20Red%20Menu%20Material%20theme) folder | Use firmware, icons, and fonts from the [`Copy to SD Card root directory to update - The Round Miracle theme`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update/THEME_The%20Round%20Miracle%20Menu%20Material%20theme) folder |



## Update TFT Firmware


The TFT firmware update is done in three steps described below and includes up to four elements:

**ELEMENTS:**

**element1:** The firmware binary (`BIGTREE_TFT*_V*.*.*.bin`). Example: `BIGTREE_TFT35_V3.0.26.1.bin`:

- `BIGTREE_TFT_35`: model
- `V3.0`: hardware version
- `26.1`: software version

For the MKS TFT28 the binary file is `MKSTFT28.bin`.


**element2:** Fonts and Icons (in the `TFT*` or `MKS` folder):

For BTT TFTs the ROOT folder for fonts and icons is TFT*, where * is the size of the TFT (example: TFT24, TFT35, TFT50, etc).
Fonts and icons folder structure:
- `TFT*/font`: fonts
- `TFT*/bmp`: icons

For MKS TFTs the ROOT folder for fonts and icons is "MKS".
Fonts and icons folder structure:
- `MKS/font`: fonts
- `MKS/bmp`: icons

**element3:** The config.ini file

**element4:**  One or several language file(s) **(optionally)**



**STEPS:**

**step1:** Copy your self compiled firmware or the BIGTREE_TFT*_V*.*.*.bin, plus the TFT*` folder and the config.ini to the root of a blank SD card that is <8GB and formatted as FAT32:

![Firmware](https://user-images.githubusercontent.com/54359396/100600549-b6cffd00-3301-11eb-8b57-d56b7a4422f1.jpg)

**Optionally**, copy one or several language.ini file(s) from [`Copy to SD Card root directory to update/Language Packs`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update/Language%20Packs) folder onto the SD card. Doing so will allow you to switch between English and the uploaded language(s), using the corresponding Language function of the TFT. We recommend to upload the minimum amount of languages, to keep the memory usage low. The language.ini file can be edited to change the text shown on the TFT.

![Language Pack](https://user-images.githubusercontent.com/54359396/100600564-b9caed80-3301-11eb-8997-d376f05323f6.jpg)


**step2:** Place SD card with the `BIGTREE_TFT*_V*.*.*.bin`, the`TFT*` folder and the config.ini into the TFT's SD card reader and reset your TFT (or optionally - power cycle your printer) to start the update process.

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

**step3:** Remove the SD card from the TFT and restart the printer.




## Configuration

The Firmware can be modified by changing the **config.ini** file from: [`Copy to SD Card root directory to update`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update) using a simple text editor (make sure to use UTF encoding).

Once saved, the config.ini can be uploaded without the need to upload the firmware or the TFT folder again, as long as the firmware and the config.ini are from the same version.

### Editing configuration (config.ini) file

To edit the **config.ini** file follow the instruction here: [Detailed Instructions here](config_instructions.md)

### Updating Firmware Configuration

To update the Firmware configuration:

1. Edit the settings in **config.ini**.

2. Copy the **config.ini** file to the root of the SD card. (The SD card capacity should be less than or equal to 8GB and formatted as FAT32)

3. Insert the SD card into the TFT's SD card slot and restart the printer or press the reset buttion of the TFT.

4. The TFT will update and store the configuration from **config.ini** file.

5. Make sure to remove the SD card from the TFT and restart the printer.

6. On the TFT click on Menu - Settings - Feature and navigate to the last page. Click on

   "Reset default settings ..."

7. Restart the printer to finish the update of the config.ini



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

## Appendix

### Setup of BTT TFT35 E3 V3.0 with MKS SGEN_L Mainboards

The following mainboards are covered by this document.

MKS GEN_L v1.0, MKS SGEN (LPC1769), MKS SGEN_L v1.0 (LPC1768) and MKS SGEN_L v2.0 (LPC1769)

**Connections**

The TFT35 E3 V3.0 has 3 cables to connect to the mainboard. Two 10 pin ribbon cables and one 5 pin serial cable. The 2 ribbon cables connect to the EXP1 and the EXP2 connections on both the TFT35 E3 V3.0 and the MKS mainboards.

The RS232 cable is connected to the RS232 connection on the touchscreen, with the other end connecting to the AUX1 connection on the mainboard. The RS232 cable has 5 wires. One end has a single 5 wire connector that goes to the RS232 connector on the touchscreen, and the other end has two connectors, one has 4 wires, and the second one has one wire. That single wire is for the Reset and is not used on these MKS mainboards. The 4-pin connector plugs into the AUX1 connection. It must connect to the top row of pins when looking at the socket with the notch facing away from the mainboard and must be also plugged in with the 5v+ wire connected to the first pin in the upper left corner of the socket. The RESET wire is not connected to anything.

![Serial](https://user-images.githubusercontent.com/54359396/103489161-acc68000-4e12-11eb-8ee8-cb6376f3589a.png)

NOTE: On the MKS mainboards there is an issue that involves at least the MKS GEN_L, MKS SGEN, and MKS SGEN_L models. The EXP1 and EXP2 connections have the socket shell installed wrong way around. The notch that indexes the cable should be facing towards the mainboard. If you get a blank screen on the TFT35 E3 V3.0 touchscreen after connecting the two EXP cables and turning the printer on, turn printer off and disconnect the 10 pin cables from either the touch screen or the mainboard and using small diagonal cutters trim the tab down to be as close to flush as you can get on both cables (and only on one end) and plug them back in with the trimmed tab now facing the mainboard.

![EXP](https://user-images.githubusercontent.com/54359396/103489164-b2bc6100-4e12-11eb-9210-685e8735e040.png)

The second workaround for this issue is to carefully pry the two shells surrounding the pins on the mainboard upwards until they clear the pins. Do NOT use a metal tool for this, use a sturdy plastic or whalebone prying tool. Turn the shell 180 degrees and align the pins with the holes in the shells and push the shells back on with your thumb. Do not push the shell back on with something that could cause damage if it were to slip. Once the shells are installed you can use the stock (unaltered) cables as they are.

**Firmware**

Edit the configuration.h file and enable the line that says: `#define REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER`. Rebuild and deploy the Marlin firmware to your 3D Printer.



### Show more statistics at the end of the print

Statistics as filament length, filament weight and filament cost can be embedded into the gCode. After the print is finished there will be an infobox that you can click and a popup will present you the printed filename (limited to the first 25 characters), the time needed for the print, the filament length used, the filament weight and its cost. In the case of multi-filament usage the statistics will show the sum of all individual data (sum of length, sum of weight, sum of cost).

The statistic data in the gCode must have the following format (a good practice would be to include this at the beginning of the gCode):
* `M118 P0 filament_data L:{12.3456}m`	L: represents the length in meters
* `M118 P0 filemant_data W:{1.23456}g`	W: represents the weight in grams
* `M118 P0 filament_data C:{0.1234}`		C: represents the cost without a unit

The values of every filament data can be in a brackets, parentheses, apostrophes, etc. or without them, measurement units can be there or not.
So `M118 P0 filament_data L:(12.3456)m`, `M118 P0 filament_data L:12.3456meters`, `M118 P0 filament_data L:[12.3456]` and so on are all valid formats.
For multi-filament print statistics the data for each used filament should be written, they can be separated by comma, space, asterix, whatever, except ";" and ".".
Examples for multi-filament:
* `M118 P0 filament_data L:(12.3456, 8.2520974)m`
* `M118 P0 filament_data W: 24.87652 15.568264 gramm`
* `M118 P0 filament_data C:[1.3456], [0.56024]`

The inclusion of the filament data into the gCode can be automated. In Cura all you have to do is to insert the following into the Start G-Code:
* `M118 P0 filament_data L:{filament_amount}m`
* `M118 P0 filament_data W:{filament_weight}g`
* `M118 P0 filament_data C:{filament_cost}`

In case the gCode file has been generated using the  [BTT 3D Plug-In Suit](https://github.com/bigtreetech/Bigtree3DPluginSuit), the data is automatically added.

In case filament data is not present in the gCode, the filament length data is calculated during print. Length is calculated regardless of using the TFT USB, TFT SD or the onboard SD. Calculations are done in both absolute or relative extrusion mode. Filament data takes into account the flow rate also but with a caveat. It has to be the same flow rate during the entire time of the printing, because the end result is calculated based on the flow rate at the time the print has finished. If flow rate changes during the print the results will not be accurate anymore.