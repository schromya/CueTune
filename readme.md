# CueTune

This is the start of an ESP32 music/audio player inspired by [PhonieBox](https://github.com/MiczFlor/RPi-Jukebox-RFID)

## 1. Prerequisites 

### a. Aquire Materials

| Name                                                       | Number Units |
|------------------------------------------------------------|--------------|
| ESP32-WROOM-32D                                            | 1            |
| USB Micro Data Cables                                      | 1            |

More TODO

### b. Install required software
Make sure you have the Arduino IDE. You may also need to install a UART Driver if the Arduino IDE does not recognize your ESP32 in Step 1c.

* [Arduino IDE](https://www.arduino.cc/en/software)

* [UART Driver](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads)
    * On Windows, download/extract CP210x VCP Windows and run CP210xVCPInstaller_x64. 
    * On Mac, download/extract CP210x VCP Mac OSX Driver and run SiLabasUSBDriverDisk.dmg
    * On Ubuntu, you should not need a driver.

### c. Setup Arduino IDE
In the Arduino IDE, go to Tools > Board > Board Manger. Type in ESP32 and install esp32 by Espressif.  

Now plug in the ESP32 to your computer and click on the new port that is added under "Select Board" on the top bar. A window with “Select Other Board and Port” should pop up. Under boards, filter and select “ESP32-WROOOM-DA Module”. Then click okay.

## 2. Running
To run this code, open this repo in the Arduino IDE and press Verify (the check) and Upload (the arrow)


## Troubleshooting
* Arduino Serial Monitor/Plotter Error: command 'open' failed: Permission denied. Cood not connect to <YOUR_PORT> serial port.
    * Ubuntu Solution: Run `sudo chmod a+rw <YOUR_PORT>`

*  ModuleNotFoundError: No module named 'serial'
    * Ubuntu solution: Run `pip install pyserial`


## Progress
Hello world program that prints "Hello World!" to serial monitor

## Resources
* ESP32
    * https://randomnerdtutorials.com/getting-started-with-esp32/