## Bootloader for BTT GD TFT35 E3 V3.0.1 - GD32F205 and GD32F207

For programming you can use GD32 all in one programmer with a TTL Adapter (USB to UART like CH340 or CP210x) or GD USB Adaptor.
If you want to use TTL Adapter just know that you need to put the mcu into isp mode. (Boot0 pin needs to be connected to 3.3V), you can use the guide on V2.0 board. Just remember the USART0 pins are on ESP8266 header.

Process is super easy:
1-Provide power to your board, through 5V connector or 3.3V headers.
2-Connect the programming port to you board, either through SWD port or USART (ESP8266 RX Pin to TTL Adaptor TX Pin, ESP8266 TX Pin to TTL RX Pin)
3-If you are using TTL Adapters, Make sure the Boot0 pin is connected to 3.3V (there is a 10k resistor on the left side of mcu, connect the top side of the resistor to bottom side of the capacitor above it), either permanently or temporary with a tweezers. No need for SWD programmmer
4-Click the reset button on the front side of the board
5-Select correct programmer and programming port in the software and click on connect
6-If software detected your chip, erase and download the bootloader on your chip, If it gave you any error try again...
7-Afte you are done programming the board, If necessary Upload Graphics, Font, Language and ....

Thank you @skopusdotorg




