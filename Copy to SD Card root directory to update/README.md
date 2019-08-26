# how to update firmware
copy [these files](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update) to SD Card root directory

The structure of the files in SD Card should be as follows
![image](https://user-images.githubusercontent.com/25599056/61274462-75a77d80-a7de-11e9-8e86-04d3d2abfb4d.png)

- the **.bin** file is the firmware of BIGTREETECH TFT24, this file needs to be updated every time for update function
- the **TFT24** folder contains the icons and fonts
  - icons: If you want to customize icons, see [here](https://github.com/bigtreetech/BIGTREETECH-TFT35-V1.2/blob/master/TFT%2035%20screen%20customize.pdf), and update the folder by SD Card (don't need update every time)
  - fonts: this is a bitmap fonts of ASCII and GBK. The font is Song typeface (don't need update every time)

# software version
### Vx.x.20
- The first version of the Integration Project
### Vx.x.21
- fix unable to print properly from U disk(Stop printing at random)
- fix busy popup with "Processing" in Printing and Stop Printing
### Vx.x.22
- clean up framework
- add support TFT35 V2.0 (SDIO)
- add function filament runout detect, power supply control
