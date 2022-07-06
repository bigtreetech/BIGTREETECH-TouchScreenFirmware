## Bootloader for BTT TFT70 V3.0 - STM32F407VG

You will need a programmer ST-Link

First erase the MCU completely then program the attached file at address 0x08000000
After it's done you should see something like "Illegal app" on display.
Grab the latest firmware from:
https://github.com/bigtreetech/BIGTREETECH-TouchScreenFirmware/tree/master/Copy%20to%20SD%20Card%20root%20directory%20to%20update
and copy it to a formatted SD card.
Connect the SD card to your board while it's off, Power the board up. You'll see the update message.
After firmware update is done you might need to update the graphics too.
Grab them from the same folder I linked, Copy TFT70 folder and Config.ini to your SD card and update your board.
After you are done with your board, your display should be usable again.

Thank you @skopusdotorg

Flash program to download here:
https://wiki.st.com/stm32mpu/wiki/STM32CubeProgrammer
https://uloz.to/file/N8qY2gSUhn3t/en-stm32cubeprog-v2-3-0-zip




