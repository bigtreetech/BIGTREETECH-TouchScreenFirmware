# Dump of bootloader V3.0.5

This is the bootloader dumped from MKS TFT Controller V1.4. It is only suitable for TFTs with GD32F305VCT6 MCU.
Do not use on any other TFT !!!


## Usage

Require an ST-link tool attached to the SWD port of your TFT Controller (should be soldered).

#### Dump

Execute the command `st-flash read bootloader.bin 0x08000000 0x7000`

#### Flash

From this folder, execute the command `st-flash write MKSTFT28EVO.BIN 0x8000000`
