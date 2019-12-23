# software version
### Vx.x.25
- change character encoding from GBK to UTF-8, compatible with new languages
- add "Preheat" menu for one button preheating(PLA, PETG, ABS)
- add command for Bltouch in "ABL"(automatic leveling) menu
- add More "menu" in printing (set RGB color, extrude for change filament, etc...)
- Rearranged the "Settings" menu
  - add silent mode(disable the buzzer)
  - add automatic shutdown after printing feature(default disable, enable in menu "Settings->Feature->AutoPower" or "Printing->Feature->AutoPower")
  - add smart filament detect feature (enable in menu "Settings->Feature->Smart ON", NEED Marlin to add "M114 E" command support, which will be merged into Mallin as soon as possible)
  - New languages, currently supporting 11 languages: English, simplified Chinese, Russian, Japanese, Armenian, German, Czech, Spanish, French, Portuguese, Italian
- Added a Custom gcode menu
- Added unified menu
  - [unified menu](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update%20-%20Unified%20Menu%20Material%20theme)
  - <img src="https://user-images.githubusercontent.com/38851044/69716869-efb12100-1145-11ea-9b4e-e50a215e2def.png" width="500"></img>
  - [classic menu](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update)
  - <img src="https://user-images.githubusercontent.com/38851044/69717274-b7f6a900-1146-11ea-94c4-4033f23215bb.png" width="500"></img>
  - Unified menu add status menu (hotend & bed temperature, fan speed, move speed, flow, XYZ position)
  - Gcode file under classic menu is in icon mode, and model preview feature is added(Use with this [tool](https://github.com/bigtreetech/BiQU_Tool))
  - Gcode file under unified menu is in list mode, unable to preview the model, but more friendly for long filename
- Fixed the bug that tft35 V3.0 could not connect to printer when switching from LCD12864 mode to touch screen mode
- Fixed the bug of printing from SD card on board
- Fixed print random stop when enable "keep alive" feature in Marlin
### Vx.x.24
- add some settings (Before that, update your icons folder)
- Long Press Touch Screen or Rotary Encoder entry mode selection(Marlin's mode is SPI mode, touch mode is UART mode)
- setting -> more set -> Change 12864 background color
- setting -> more set -> Change 12864 font color
- setting -> more set -> GCODE
### Vx.x.23
- add support TFT35 V3.0 (STM32F207)
- add filament runout on/off button
- replace Power off button with Rotate UI button
- improve the compatibility of smoothieware
- add manually leveling menu
- add multi-serial port function (Touch screen can be directly controlled by ESP3D)
### Vx.x.22
- clean up framework
- add support TFT35 V2.0 (SDIO)
- add function filament runout detect, power supply control
### Vx.x.21
- fix unable to print properly from U disk(Stop printing at random)
- fix busy popup with "Processing" in Printing and Stop Printing
### Vx.x.20
- The first version of the Integration Project


# How to install vscode and platformio for compile
- Install the compilation environment, We recommend vscode + platformio. If you have already installed it, please skip this step.  If not, please see [here](https://github.com/bigtreetech/Document/blob/master/How%20to%20install%20VScode+Platformio.md).
# how to compile firmware

- Please see the picture(①), which is the PlatformIO plugin, and then click ② Open Project to open the project.
![image](https://user-images.githubusercontent.com/25599056/56637513-6b258e00-669e-11e9-9fad-d0571e57499e.png)
 
- Find the BIGTREETECH TFT35 firmware source directory , then click Open
![image](https://user-images.githubusercontent.com/25599056/56637532-77115000-669e-11e9-809b-f6bc25412f75.png)

- After opening the project, go to the platformio.ini file and change the default environment .
the “env_default” can be “BIGTREE_TFT35_V1_2”, “BIGTREE_TFT35_V1_1”, and “BIGTREE_TFT35_V1_0”
it depends on your hardware.
![image](https://user-images.githubusercontent.com/25599056/56637542-7d073100-669e-11e9-9e9e-1efa6b73be5d.png)

- After the modification is complete, press Ctrl+Alt+B, and platformio will automatically download the compiled component and compile it.
![image](https://user-images.githubusercontent.com/25599056/56637550-809ab800-669e-11e9-99d3-6b502e294688.png)
 
- After the compilation is successful, a BIQU_TFT35_V1.x.x.bin file will be generated in the .pioenvs\BIGTREE_TFT35_V1_x directory. We need copy this file to the SD card, and insert this SD Card to TFT35 Touch Screen, then reset the Touch Screen,  the firmware will be updated automatically.
# how to config firmware
![image](https://user-images.githubusercontent.com/25599056/61276962-b229a800-a7e3-11e9-83fe-ec5e944f1463.png)
Modify the value of
> #define TOOL_NUM     1    //set in 1~6

> #define EXTRUDER_NUM 1    //set in 1~6

> #define FAN_NUM      1    //set in 1~6

to config hotend, extruder, fan number
# how to update firmware
copy firmware(.bin), fonts(TFT24/font) and icons(TFT24/bmp) to SD Card root directory, insert SD card to touch screen, reset touch screen and then it will be updated automatically.

unified menu in [here](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update%20-%20Unified%20Menu%20Material%20theme)

<img src="https://user-images.githubusercontent.com/38851044/69716869-efb12100-1145-11ea-9b4e-e50a215e2def.png" width="500"></img>

classic menu in [here](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update)

<img src="https://user-images.githubusercontent.com/38851044/69717274-b7f6a900-1146-11ea-94c4-4033f23215bb.png" width="500"></img>
  
The structure of the files in SD Card should be as follows
![image](https://user-images.githubusercontent.com/25599056/61274462-75a77d80-a7de-11e9-8e86-04d3d2abfb4d.png)

- the **.bin** file is the firmware of BIGTREETECH TFT24, this file needs to be updated every time for update function
- the **TFT24** folder contains the icons and fonts, if you want to customize them, see [here](https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/readme/English) and update the folder by SD Card
  - icons: BMP format, 24/32 bit depth  (don't need update every time)
  - fonts: this is a bitmap fonts of ASCII and unicode. (don't need update every time)
