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

- [Menus and Themes](#menus-and-themes)
- [Update TFT Firmware](#update-tft-firmware)
- [Configuration](#configuration)
- [Customization](#customization)
  - [Bootscreen and Icons](#bootscreen-and-icons)
  - [Firmware](#firmware)
- [Troubleshooting](#troubleshooting)
- [Version History](#version-history)

## Menus and Themes

|                Classic Menu &amp; Icon Theme                 |            Unified Menu &amp; Material Icon Theme            |      |      |
| :----------------------------------------------------------: | :----------------------------------------------------------: | ---- | ---- |
| ![](https://user-images.githubusercontent.com/54511555/77016371-ad9a2e00-6934-11ea-9e57-23c2ab2cad58.png) | ![](https://user-images.githubusercontent.com/54511555/77016415-d3273780-6934-11ea-8c61-a184fa55b420.png) |      |      |
| Use firmware, icons, and fonts from the [`Copy to SD Card root directory to update`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update) folder | Use firmware, icons, and fonts from the [`Copy to SD Card root directory to update - Unified Menu Material theme`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update%20-%20Unified%20Menu%20Material%20theme) folder |      |      |

## Update TFT Firmware


TFT firmware updates are comprised of three elements:

1. The firmware binary (`BIGTREE_TFT*_V*.*.*.bin`). Example: `BIGTREE_TFT35_V3.0.26.1.bin`:
   - `BIGTREE_TFT_35`: model
   - `V3.0`: hardware version
   - `26.1`: software version
2. Fonts and Icons (`TFT*` folder):
   - `TFT*/font`: fonts
   - `TFT*/bmp`: icons
3. The config.ini file

Copy  the `BIGTREE_TFT*_V*.*.*.bin,the `TFT*` folder and the config.ini to the root of a blank SD card that is <8GB and formatted as FAT32:


![Copy](https://user-images.githubusercontent.com/54359396/94800953-a1178780-03e5-11eb-9d2f-6a8ea908297b.png)


Place SD card with the `BIGTREE_TFT*_V*.*.*.bin`, the`TFT*` folder and the config.ini into the TFT's SD card reader and power cycle your printer to start the update process.

<p align=center> ⚠️ Failing to update your icons &amp; fonts will result in missing icons and/or unreadable text ⚠️ </p>



## Update Process shown on TFT Screen

A successful update looks like this on the screen:

![Screenshot 2020-09-26 at 22 10 04](https://user-images.githubusercontent.com/54359396/94349526-5abcd400-0045-11eb-993a-afc5b241f4d7.png)

... and the name of the elements on the SD card changes to this:

![After Update](https://user-images.githubusercontent.com/54359396/94349755-dc156600-0047-11eb-9b1e-a1334bc5675f.png)

<p align=center> ⚠️ Errors during the update can not be ignored and must be solved before using the TFT ⚠️ </p>

After the update is done and the files are renamed, it is possible to reuse them again. To do so, change the name of the element(s) to the pre-update name and start the update process again.


## Configuration

The Firmware can be configured using the **config.ini** file from from one of these folders:
[`Copy to SD Card root directory to update`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update) or
[`Copy to SD Card root directory to update - Unified Menu Material theme`](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update%20-%20Unified%20Menu%20Material%20theme)

### Editing configuration (config.ini) file

To edit the **config.ini** file follow the instruction here: [Detailed Instructions here](config_instructions.md)

### Updating Firmware Configuration

To update the Firmware configuration:

1. Edit the settings in **config.ini**.
2. Copy the **config.ini** file to the root of the SD card. (The SD card capacity should be less than or equal to 8GB and formatted as FAT32)
3. Insert the SD card in the TFT's SD card slot and restart the TFT by pressing the reset buttion or disconnecting and connecting the power cable.
4. The TFT will update and store the configuration form **config.ini** file.

The config.ini can be edited using a simple text editor (make sure to use UTF encoding) and uploaded again - without the need to upload the firmware or the TFT folder - as long as the firmware is the same version number.

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