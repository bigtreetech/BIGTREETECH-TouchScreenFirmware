# how to update firmware
copy firmware(.bin), fonts(TFT24/font) and icons(TFT24/bmp) to SD Card root directory, insert SD card to touch screen, reset touch screen and then it will be updated automatically.

classic menu![image](https://user-images.githubusercontent.com/38851044/69717274-b7f6a900-1146-11ea-94c4-4033f23215bb.png)
  
The structure of the files in SD Card should be as follows
![image](https://user-images.githubusercontent.com/25599056/61274462-75a77d80-a7de-11e9-8e86-04d3d2abfb4d.png)

- the **.bin** file is the firmware of BIGTREETECH TFT24, this file needs to be updated every time for update function
- the **TFT24** folder contains the icons and fonts, if you want to customize them, see [here](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/readme/English) and update the folder by SD Card
  - icons: BMP format, 24/32 bit depth  (don't need update every time)
  - fonts: this is a bitmap fonts of ASCII and unicode. (don't need update every time)
