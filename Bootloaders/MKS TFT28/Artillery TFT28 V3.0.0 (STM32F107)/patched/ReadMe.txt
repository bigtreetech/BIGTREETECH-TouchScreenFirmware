This is a bootloader patcher for the MKSTFT28. It is only suitable for TFTs with STM23F107VC MCU.
Do not use on any other TFT !!!

This patcher uploads a bootloader that changes the long beep of the bootloader on startup to a short "chirp", nothing more, nothing less.

Usage:
Put it on the SD card as any other regular FW file and boot your printer with the SD inserted.
During the update you'll see a red dot and a progress bar. The progress bar will let you know when the upload of the patched bootloaders is finished. Also, at the end of the patched bootloader flash procedure the red dot will change into a blinking green dot. That's when you can eject the SD card and copy to it your favorite FW file to be flashed over this bootloader patcher FW, reinsert the SD Card into the TFT and reboot it.

Attention!!! This is not a functional FW, it's only for uploading the patched bootloader, you must reupload your regular favorite FW after the bootloader patching FW has done its job.

Enjoy!

Yours truly
@kisslorand
