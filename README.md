# FinalProject30402 #

This project is my submission for the Final Project of UC Santa Cruz Extension Course 30402, "Internet of Things: System Design and Prototyping", with instructor Anil Gathala. 

## Contents ##

The main folder contains the IRPulseDemo.ino main sketch for the 32U4 micro controller. It also contains sanitized versions of the three Python scripts, removing private setup/authorization keys for the three services used.

The DemoVideo folder contains two .MOV files (originally one file, split due to Github 100MB limit) describing the operation of the gadget described here.

There is a scanned PDF file containing slides, including the Block Diagram, Circuit Diagrams, and two Event Flows for Wand Trigger and Select Button.

## Operation ##

The gadget is capable of two functions, each of which has multiple side effects.

The main event flow, triggered by the Kymera wand or any Infrared Remote Control, will sample some data and upload it to the Cloud (Adafruit.io site) and also send a personal text message to my cellphone via Twilio.

The auxiliary event flow is triggered by pressing the Select button on the LCD Shield, and this takes a webcam picture and uploads it to my Dropbox account.

The operation of each flow comes from the main sketch loop() function, which polls the IR receiver and the LCD buttons, and if activity is detected, fires off the appropriate event responses. These include adjusting the LCD display, animating the NeoPixel LEDs, and sending Process scripts to the Linux side to run various Linux commands or Python scripts.

To do these scripts, the Linux side had to be prepared.

## Linux Processor Preparations ##

1. Downloaded new firmware from Arduino.cc main page (v1.5.3), then added the Yun to my home WiFi network.
2. Ran the sketch to unify the memory partitions on an added 16GB SD card for maximize HD space (see [here](https://www.arduino.cc/en/Tutorial/ExpandingYunDiskSpace))
3. Followed instructions here to install GCC developer tools (optional, see [here](http://noblepepper.com/wp/blog/2014/10/22/gcc-g-on-an-arduino-yun/)). Some modifications were needed to get this to work:
	1. I did not need to use special refs to /mnt/sda1 due to step #2.
	2. During the step "download and compile xz", the wget should happen just before the tar (5th item), and should use --no-check-certificate.
4. Followed basic instructions for system preparation for Twilio Python package (see [here](https://twilioinc.wpengine.com/2015/02/send-sms-and-mms-from-your-arduino-yun.html)).
5. Followed instructions for system prep for Adafruit-io Python package (see [here](https://github.com/adafruit/io-client-python)).
6. Followed instructions for system prep for Dropbox Python package for the take picture feature. I also installed video and webcam tools (see [here](http://twilioinc.wpengine.com/2015/02/arduino-powered-photobooth-arduino-yun-a-webcam-and-dropbox.html)).
7. There are still errors about Python 2.7.3 having security problems. These were fixed in Python 2.7.9 provided with a newer OpenWRT firmware (Chaos Calmer), but installing it would void my warranty. Hopefully Arduino will fix this eventually.
8. This allowed me to write, test, and debug the three Python scripts used in the project:
	- SMS text using Twilio (send-sms.py)
	- data to Adafruit.io (send-aio.py)
	- photo files to Dropbox (upload-to-dropbox.py)


