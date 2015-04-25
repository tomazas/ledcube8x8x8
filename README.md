About
---------
Source code and all files for the generic 8x8x8 3D LED Cube found on eBay 
(having STC 8051-based STC12C5A60S2 mcu), aka. 3D LightSquared 8x8x8 LED Cube DIY kit, ideasoft etc.

`Note`: as received from manufacturer with additional cleanups/optimizations.

Firmware v2
---------
The `firmware/v2` directory contains an optimized and more advanced version of the ledcube firmware.
All animations were removed except one and UART/serial based control was implemented. 
So the cube can be controlled with Arduino/Atmega or simple PC serial console.

`Note`: The ledcube UART control mode is activated once the first command comes over serial connection.

The new firmware can be flashed/written to STC MCU through UART/serial connection. 
Software such STC-ISP is required to transfer the hex file (machinecode) to the STc microcontroller. 

`Download here`: http://www.stcmcu.com/STCISP/stc-isp-15xx-v6.85.zip

##### Serial connection: 
* USB-2-TTL converter/adapter/module can be used to connect to PC (e.g. search eBay)
* Parameters: baud - 9600 bps, 1 stop bit, no parity
* Cube UART pins: VCC, GND, P30(RXD), P31(TXD)

##### Programming STC
* Connect serial module to ledcube serial pins
* Open STC-ISP and select STC12C5A60S2 and serial port
* Select HEX file: ledcube8.hex and press `Download/program`
* Reset power to MCU to start programming

![Alt text](/help/howto_stc.png "Programming the STC mcu")

![Alt text](/help/programming_ok.png "Programming successful")

##### Extra - C51 project
Keil uVision C51 project file is also available in the `firmware/v2` directory if compiling the source code is a must.
Keil C51 compiler tools are required to open the project.

`Download here`: https://www.keil.com/demo/eval/c51.htm
