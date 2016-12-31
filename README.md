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

Some of the time I programmed directly over the USB COM port, but mostly used it for power, and downloaded sketch code via the WiFi link port into the Yun from my Windows 7 PC. I also used Git Bash `ssh username@arduinoname.local` to access the Linux side directly over WiFi, negating the need to use PuTTY for the SSH capability. This was so I could use my 30-inch monitor to make it easier to see to program. The Surface is great for portability to class, but just can't cut it as a great desktop machine, whether doing Arduino sketches or Linux command line.

By downloading sketch code directly using the WiFi port from the Arduino IDE, I was able to do most of my programming on the Windows 7 desktop, and left the direct COM port line for when I wanted to see the Serial debugging output I had included in the sketch. There did not seem to be a way to get output from the Python scripts back into the sketch though. Perhaps this bears further investigation.

## Software Architecture ##

The main software runs from the .ino sketch on the 32U4 microcontroller. It uses the Bridge and Process libraries to trigger Python scripts or Linux commands when needed to communicate over the Web or to the Cloud.

The design is a polled loop, as is typical for Arduino sketches. I poll the IR receiver to trigger the first event flow, and poll the LCD buttons to trigger the second picture-taking event flow.

Libraries used for the Arduino Yun sketch:

- The TSOP-38238 uses Bengtt Martensson's IR4Arduino library [here](https://github.com/bengtmartensson/Infrared4Arduino).
- The Adafruit NeoPixels use their provided library discussed [here](https://learn.adafruit.com/adafruit-neopixel-uberguide?view=all). (This includes additional circuit requirements. USB power was sufficient.)
- The Adafruit LCD Shield uses their provided library discussed [here](https://learn.adafruit.com/rgb-lcd-shield?view=all). 

----------
Usage requirements for sketch binary:
  
Sketch uses 19,840 bytes (69%) of program storage space. Maximum is 28,672 bytes.
Global variables use 1,164 bytes (45%) of dynamic memory, leaving 1,396 bytes for local variables. Maximum is 2,560 bytes.

----------
Linux results for `df -h`:

root@ardmike:~# df -h
Filesystem                Size      Used Available Use% Mounted on
rootfs                   10.4G    510.6M      9.4G   5% /
/dev/root                 7.5M      7.5M         0 100% /rom
tmpfs                    29.8M    100.0K     29.7M   0% /tmp
tmpfs                   512.0K         0    512.0K   0% /dev
/dev/sda2                10.4G    510.6M      9.4G   5% /overlay
overlayfs:/overlay       10.4G    510.6M      9.4G   5% /
/dev/sda1                 4.0G     12.0K      4.0G   0% /mnt/sda1

----------

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

## Suggestions for Improvement ##

1. I really don't try to differentiate IR signals (remote control codes). The wand can send 13 different signals, but the timing variances make the classification problem difficult. The IR4Arduino library didn't seem to have (or I couldn't find) a simple function set that would return a particular normalized timing signal that represented an ideal code. Perhaps it exists, perhaps not. The idea that most often came to mind while studying this, was that this seemed to be a good use case for the Arduino 101 (new replacement for Arduino Uno), with its Intel Curie module and the 128-neuron Pattern Recognition hardware. A neuron could be trained for each signal needed, and different wand commands could be triggered. This seemed to be overkill for my demo situation, however.
2. I really didn't delve much into Neopixel programming. My "pattern" just sent the same color to all 17 pixels, and changed it 3 times. This could be augmented with spatially-interesting patterns as well, such as rotating an every-3rd-pixel pattern in 3 positions, with the other pixels left dark. I also thought about using random colors for each pixel and just shifting them a few times. Also, I considered various geometric or algorithmic designs, such as each pixel is a fraction brighter or dimmer than the last, all in the same basic color. But this would work best with some other color scheme, such as HSV with a converter to RGB. I didn't feel like writing all this, or exploring whether another library would support this mode better.
3. A different sensor could take the place of the LDR's simple analog input. I briefly considered using a more complicated light sensor, such as the TSL2561 which I had previously bought, or the required DHT-11 digital temperature/humidity sensor, or the Bosch BME-280 which also had pressure. (I used this in my FinalProject5381 for another course.)
4. Just using the pic in Dropbox to send an MMS text via Twilio, instead of the data in a string for SMS. This would take about a minute to complete though.
5. Cloud data processing or AI for the uploaded pic file (see IBM Watson usage in Bluemix cloud for typical possibilities). Image recognition, object classification, many other things are possible.
6. Twilio can cause scripted things to happen when making a phone call. I toyed with calling the phone number provided and playing the Harry Potter theme to the caller, or something similar.
7. Causing a shutter click sound when the pic is taken. This would require a way to download and play audio files from the gadget. Possibly easier to do on the Linux side if a USB audio device can be controlled. The webcam I used (Logitech 5000) has a mic but no speaker. Since there is only one USB port, a hub would be needed. Or perhaps a way to play the file on the MCU side would be workable. But data is limited there, even in flash PROGMEM.
8. Try to fix the Linux Python 2.7.3 issues with TLS security calls. There are some suggestions offered, but when I first tried them, they failed because I hadn't installed the compiler. But now that I have that, I may pursue this solution, rather than wait for Arduino folks to release new firmware.

