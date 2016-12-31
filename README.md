# FinalProject30402 #

This project is my submission for the Final Project of UC Santa Cruz Silicon Valley Extension Course #30402, "Internet of Things: System Design and Prototyping", with instructor Anil Gathala, for Fall term 2016. Submission deadline is midnight Friday, Dec.30, 2016. 

## Contents ##

The main folder contains the IRPulseDemo.ino main sketch for the 32U4 micro controller. It also contains sanitized versions of the three Python scripts, removing private setup/authorization keys for the three services used.

The DemoVideo folder contains two .MOV files (originally one file, split due to Github 100MB limit) describing the operation of the gadget described here.

There is also a scanned PDF file containing slides, including the Block Diagram, Circuit Diagrams, and two Event Flows for Wand Trigger and Select Button.

## Operation ##

The gadget is capable of two functions, each of which has multiple side effects.

The main event flow, triggered by the Kymera wand or any Infrared Remote Control, will sample some data and upload it to the Cloud (Adafruit.io site) and also send a personal text message to my cellphone via Twilio.

The auxiliary event flow is triggered by pressing the Select button on the LCD Shield, and this takes a webcam picture and uploads it to my Dropbox account.

The operation of each flow comes from the main sketch loop() function, which polls the IR receiver and the LCD buttons, and if activity is detected, fires off the appropriate event responses. These include adjusting the LCD display, animating the NeoPixel LEDs, and sending Process scripts to the Linux side to run various Linux commands or Python scripts.

To do these scripts, the Linux side had to be prepared.

## Linux Processor Preparations ##

1. Downloaded new firmware from Arduino.cc main page (v1.5.3), then added the Yun to my home WiFi network.
2. Ran the sketch to unify the memory partitions on an added 16GB SD card for maximize HD space (see [here](https://www.arduino.cc/en/Tutorial/ExpandingYunDiskSpace))
3. Followed instructions to install GCC developer tools (optional, see [here](http://noblepepper.com/wp/blog/2014/10/22/gcc-g-on-an-arduino-yun/)). Some modifications were needed to get this to work:
	1. I did not need to use special refs to /mnt/sda1 due to step #2.
	2. During the step "download and compile xz", the wget should happen just before the tar (5th item), and should use --no-check-certificate.
4. Followed basic instructions for system preparation for Twilio Python package (see [here](https://twilioinc.wpengine.com/2015/02/send-sms-and-mms-from-your-arduino-yun.html)).
5. Followed instructions for system prep for Adafruit-io Python package (see [here](https://github.com/adafruit/io-client-python)).
6. Followed instructions for system prep for Dropbox Python package for the take picture feature. I also installed video and webcam tools (`fswebcam`, see [here](http://twilioinc.wpengine.com/2015/02/arduino-powered-photobooth-arduino-yun-a-webcam-and-dropbox.html)).
7. There are still errors about Python 2.7.3 having security problems. These were fixed in Python 2.7.9 provided with a newer OpenWRT firmware (Chaos Calmer), but installing it would void my warranty. Hopefully Arduino will fix this eventually. It does not affect script operation.
8. This allowed me to write, test, and debug the three Python scripts used in the project:
	- SMS text using Twilio (send-sms.py)
	- data to Adafruit.io (send-aio.py)
	- photo files to Dropbox (upload-to-dropbox.py)
9. It is useful to note that you can access and copy files to/from the Arduino Linux processor using 'scp' as follows:
	- Copy from PC to Arduino: `scp path_to_local_pc_file username@arduinoname.local:/path_to_remote_file`
	- Copy from Arduino to PC: `scp username@arduinoname.local:/path_to_remote_file path_to_local_pc_file`
	- `scp` uses the same security methods (and is based on) `ssh`, both of which can be called on Windows from the Git Bash shell.
	- for my setup, this meant that I used root@ardmike.local for both the above and ssh login, you may need arduino.local or linino.local on yours

## Hardware Setup ##

As shown in the block diagram, we need to wire up Adafruit NeoPixels (I used a single 8mm device and a 16-pixel ring), a TSOP-38238 tuned IR receiver device, a CdS photocell circuit (ambient light sensor, the data used), and the Adafruit LCD shield (16x2 LCD blue and white display stick with attached buttons and I2C bus operation). In addition, I plugged a Logitech 5000 webcam into the USB Host port (USB A connector) of the Arduino Yun. This all fit on a breadboard that was connected to the Arduino Yun, which was connected to my Windows 10 laptop (Microsoft Surface Pro 2) via the USB Micro-B port on the Yun. The LCD Shield was connected via 2-wire I2C bus to the Arduino SDA and SCL pins. These were the only pins required on the shield besides power and ground (5V power).

## Programming the Code ##

Some of the time I programmed directly over this port, but mostly used it for power, and downloaded sketch code via the WiFi link port into the Yun from my Windows 7 PC. I also used Git Bash `ssh username@arduinoname.local` to access the Linux side directly over WiFi, negating the need to use PuTTY for the SSH capability. This was so I could use my 30-inch monitor to make it easier to see to program. The Surface is great for portability to class, but just can't cut it as a great desktop machine, whether doing Arduino sketches or Linux command line.

## Software Architecture ##

The main software runs from the .ino sketch on the 32U4 microcontroller. It uses the Bridge and Process libraries to trigger Python scripts or Linux commands when needed to communicate over the Web or to the Cloud.

The design is a polled loop, as is typical for Arduino sketches. I poll the IR receiver to trigger the first event flow, and poll the LCD buttons to trigger the second picture-taking event flow.

Libraries used for the Arduino Yun sketch:

- The TSOP-38238 uses Bengtt Martensson's IR4Arduino library [here](https://github.com/bengtmartensson/Infrared4Arduino).
- The Adafruit NeoPixels use their provided library discussed [here](https://learn.adafruit.com/adafruit-neopixel-uberguide?view=all). (This includes additional circuit requirements. USB power was sufficient.)
- The Adafruit LCD Shield uses their provided library discussed [here](https://learn.adafruit.com/rgb-lcd-shield?view=all).  

## Additional Features ##

I added a power-save mode to the NeoPixel display that allowed it to turn down the color brightness of these devices when the ambient light sensor indicated lower lighting. See power requirements later for the results of this in actual use.

I added a feature to display a status indicator on the LCD shield as the last character of the first line. The codes shown for each state of the loop are as follows:

- ' ' - Idle (polling IR or buttons, LCD backlight on)
- 'R' - Flow 1 (data), reading sensor (happens too quick to read)
- 'P' - Flow 1 (data), animating the NeoPixels pattern
- 'T' - Flow 1 (data), running the send-sms script to send a text
- 'A' - Flow 1 (data), running the send-aio script to send data to the Adafruit.io cloud MQTT feed
- '*' - Flow 2 (pic), taking the webcam picture using `fswebcam` (Linux tool)
- 'U' - Flow 2 (pic), uploading the picture to my Dropbox account in the cloud

## Operating Power Requirements ##

I hooked a USB power monitor (Rui-Deng, no longer sold by Adafruit) in series with the main USB-A-to-Micro-B cable that was powering the Arduino and through it the rest of the project as well. This enabled me to measure current requirements during steady state usage of the project. Of course, the current fluctuated during use as the WiFi radio turned off and on, but I tried to choose the typical value. I classified these by status code, which determined whether the radio was operating and also whether the Neopixels were on. Since the Neopixels were left on while the WiFi communications was happening, this turned out to be the peak current usage. The picture taking and upload had approximately similar current, although during the former only the webcam is on and the latter only the WiFi.

- Status ' ' - Idle current value (no NeoPixels or WiFi): 0.59A
- Status 'P' - Neopixels on medium (Brightness = 341 out of 1023): 0.71A
- Status 'P' - Neopixels on high (Brightness = 877 out of 1023): 0.93A
- Status 'P' - Neopixels on low (Brightness = 70 out of 1023): 0.62A
- Status 'T' or 'A' - Neopixels on medium (Brightness = 341 out of 1023) with WiFi on: 0.75A
- Status 'T' or 'A' - Neopixels on high (Brightness = 877 out of 1023) with WiFi on: 0.99A
- Status 'T' or 'A' - Neopixels on low (Brightness = 70 out of 1023) with WiFi on: 0.66A
- Status '*' or 'U' - Neopixels off with WiFi on: 0.64A
