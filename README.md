### How to install vscode and platformio for compile

- Please download VSCode from [here](https://code.visualstudio.com/Download)
According to your computer operating system, to download the corresponding version, for example mine is Win7 64-bit operating system, so choose Windows 7, 8, 10 User Installer 64bit.

![image](https://user-images.githubusercontent.com/25599056/56638014-a4aac900-669f-11e9-8694-d9e4af6ad93a.png)

  After the download is complete, double-click the installation. After the installation is complete, open VSCode.
- You also need to install the PlatformIO plugin, please click on the ① in the picture below, enter PlatformIO in ② and click ③ Install to install. 

![image](https://user-images.githubusercontent.com/25599056/56638076-c5731e80-669f-11e9-82b9-4b21407df320.png)

After the download is complete, you still need Reload 

![image](https://user-images.githubusercontent.com/25599056/56638101-d328a400-669f-11e9-8406-1b2d479b8e9a.png)

After Reload, you will be prompted to install PlatformIO Core, which takes a little time.

![image](https://user-images.githubusercontent.com/25599056/56638125-dcb20c00-669f-11e9-82df-79c4581a43a8.png)

After the installation is successful, you need to reload again. After the Reload, the PlatformIO is installed.

![image](https://user-images.githubusercontent.com/25599056/56638133-e2a7ed00-669f-11e9-8951-b953b0b1bddb.png)

- In the lower left corner of the VSCode, you can see that there’s one more icon, please see the picture(①), which is the PlatformIO plugin, and then click ② Open Project to open the project.

![image](https://user-images.githubusercontent.com/25599056/56638145-ea679180-669f-11e9-9c1e-5cae6ce0d29f.png)


### how to compile firmware

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

### how to update firmware
copy [these files](https://github.com/Msq001/BIGTREETECH-TFT35-V1.2/tree/master/firmware/Copy%20to%20SD%20Card%20root%20directory%20to%20update) to SD Card root directory

The structure of the files in SD Card should be as follows
![firmware](https://user-images.githubusercontent.com/25599056/56635835-8cd04680-6699-11e9-8686-bc805a7f1060.jpg)


- the **.bin** file is the firmware of BIGTREETECH TFT35, this file needs to be updated every time for update function
- the **bmp** folder contains the icons, If you want to customize icons, see [here](https://github.com/bigtreetech/BIGTREETECH-TFT35-V1.2/blob/master/TFT%2035%20screen%20customize.pdf), and update the folder by SD Card (don't need update every time)
- the **font** folder contains the bitmap fonts of ASCII and GBK. The font is Song typeface (don't need update every time)
