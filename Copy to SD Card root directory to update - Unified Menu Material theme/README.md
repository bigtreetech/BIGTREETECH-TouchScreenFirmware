# how to Change Move Menu Layout
1. Copy and replace the folders TFT24 / TFT28 /TFT35 (compatible with your size) in "Copy to SD Card root directory to update" with folder from "Copy to SD Card root directory to update - Unified Menu Material theme".

2. Enable the Alternative Menu Layout in Configuration.h file

3. Update the firmware and update the new icons.

# how to update firmware
copy firmware(.bin), fonts(TFT24/font) and icons(TFT24/bmp) to SD Card root directory, insert SD card to touch screen, reset touch screen and then it will be updated automatically.

unified menu

<img src="https://user-images.githubusercontent.com/38851044/69716869-efb12100-1145-11ea-9b4e-e50a215e2def.png" width="500"></img>

The structure of the files in SD Card should be as follows
![image](https://user-images.githubusercontent.com/25599056/61274462-75a77d80-a7de-11e9-8e86-04d3d2abfb4d.png)

- the **.bin** file is the firmware of BIGTREETECH TFT24, this file needs to be updated every time for update function
- the **TFT24** folder contains the icons and fonts, if you want to customize them, see [here](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/readme/English) and update the folder by SD Card
  - icons: BMP format, 24/32 bit depth  (don't need update every time)
  - fonts: this is a bitmap fonts of ASCII and unicode. (don't need update every time)

