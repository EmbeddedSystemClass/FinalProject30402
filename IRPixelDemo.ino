// This sketch demonstrates the IrReceiverPoll.
// It requires a demodulating sensor connected to pin RECEIVE_PIN.

#include <Arduino.h>
#include <IrReceiverPoll.h>
#include <Adafruit_NeoPixel.h>
#include <Bridge.h>
#include <Process.h>

// include the LCD shield library code:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>


// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

///////////// END LCD SECTION

#define RECEIVE_PIN 4U
#define BUFFERSIZE 200U
#define BAUD 115200

// END IR LIB SECTION

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define NEOPIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      17

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIN, NEO_RGB + NEO_KHZ800);

int delayval = 500; // delay for half a second

/////////// END NEOPIXEL SECTION

// this is the analog pin that the LDR divider is hooked to
#define LDR_PIN         A4
#define MAX_BRIGHTNESS  1023.0
#define AMBIENT_THRESHOLD 200.0

//////// END ANALOG SECTION

IrReceiver *receiver;

int fround(float n) {
  if (n<0.0)
  return int(n - 0.5);
  else return int(n + 0.5);
}

void sendColorToAllPixels(int red, int green, int blue) {

  for(int i=0;i<NUMPIXELS;i++){

    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(
        red, green, blue
        ) );
  }

  pixels.show(); // This sends the updated pixel color to the hardware.
}

// this fires the pixel pattern at a given brightness level (10-bit number 0-1023)
void firePixelPattern(int brightness) {
  // show pattern of pixels
  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  static uint8_t colors[][3] = {
    { 250, 0, 0 },
    { 0, 250, 0 },
    { 0, 0, 250 },
    { 128, 128, 0 },
  };
  static int NUM_PATTERNS = sizeof(colors)/sizeof(colors[0]);
  float scale = brightness / MAX_BRIGHTNESS;

  for (int pat=0; pat<NUM_PATTERNS; pat++) {
    sendColorToAllPixels(fround(scale * colors[pat][0]),
        fround(scale * colors[pat][1]),
        fround(scale * colors[pat][2]));
    delay(delayval); // Delay for a period of time (in milliseconds).
  }
}

int readAmbientBrightness() {
  return analogRead(LDR_PIN);
}

void sendSMS(const String& message_p) {
  Process p;
  String message = '"' + message_p + '"';
  p.begin("python");
  p.addParameter("/arduino/send-sms.py");
  p.addParameter(message);
  Serial.print("Sending SMS<");
  Serial.print(message);
  Serial.print(">...");
  int result = p.run();
  if (result == 0) {
    Serial.println("Done.");
  } else {
    Serial.print("Error: ");
    while (p.available()) {
      char c = p.read();
      Serial.print(c);
    }
    Serial.println("");
  }
}

void sendAIO(int value_10bit) {
  Process p;
  String feed = "magic-30402-ldr";
  p.begin("python");
  p.addParameter("/arduino/send-aio.py");
  p.addParameter(feed);
  String valueStr = String(value_10bit);
  p.addParameter(valueStr);
  Serial.print("Sending to Adafruit.io<");
  Serial.print(feed);
  Serial.print(",");
  Serial.print(valueStr);
  Serial.print(">...");
  int result = p.run();
  if (result == 0) {
    Serial.println("Done.");
  } else {
    Serial.print("Error: ");
    while (p.available()) {
      char c = p.read();
      Serial.print(c);
    }
    Serial.println("");
  }
}

String makeTextMessage(int brightness, size_t timestamp) {
  String result = "B=";
  result += String(brightness);
  result +=  ",T=";
  result +=  String(timestamp);
  return result;
}

void takePicture() {
  Process p;
  p.begin("fswebcam");
  p.addParameter("/arduino/pic.jpg");
  p.addParameter("-r 640x480");
  p.run();
}

void uploadPicture() {
  Process p;
  p.begin("python");
  p.addParameter("/arduino/upload-to-dropbox.py");
  p.run();
}

void updateLCDStatus(char c) {
  lcd.setCursor(15,0);
  lcd.print(c);
}

/////////  PROGRAM STARTS HERE

void setup() {
    Serial.begin(BAUD);
    receiver = new IrReceiverPoll(BUFFERSIZE, RECEIVE_PIN);
    Bridge.begin();
    pixels.begin(); // This initializes the NeoPixel library.
    sendColorToAllPixels(0, 0, 0);
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.setBacklight(WHITE);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("It's Magic!");
}

void loop() {

    // check the IR receiver for any response (causes delay timeout)
    receiver->receive(); // combines enable, loop, disable

    if (receiver->getDataLength() > 2) {
        // PROPER RESPONSE LENGTH - process the IR command
        
        // read sensors, generate debug output including IR sensor dump
        updateLCDStatus('R');
        int ambient = readAmbientBrightness();
        size_t timestamp = millis();
        Serial.print("IR Detection @ BRT=");
        Serial.print(ambient);
        Serial.print(", T=");
        Serial.print(timestamp);
        Serial.print(" -> ");
        receiver->dump(Serial);

        // fire up the Neopixels
        updateLCDStatus('P');
        firePixelPattern(ambient);

        // prepare the response text
        String text = makeTextMessage(ambient, timestamp);

        // update the LCD response
        lcd.setCursor(0, 1);
        lcd.print(text);

        // send the text message if room is bright enough
        if (ambient > AMBIENT_THRESHOLD) {
          updateLCDStatus('T');
          sendSMS(text);
        }

        // send the data to Adafruit.io in the cloud
        updateLCDStatus('A');
        sendAIO(ambient);

        // shut off Neopixels to save power
        updateLCDStatus(' ');
        sendColorToAllPixels(0, 0, 0);
    } else if (!receiver->isEmpty()) {
        // the IR library has a tendency to trigger falsely with 2-byte data that we should ignore
        Serial.print("False positive skipped...");
        receiver->dump(Serial);
    }

    // also, check for any buttons pressed (need to hold at least as long as the timeout delay)
    uint8_t buttons = lcd.readButtons();
    if (buttons) {
      if (buttons & BUTTON_SELECT) {
        Serial.print("Smile.. you're on candid camera ... ");
        updateLCDStatus('*');
        takePicture();
        Serial.print("uploading ...");
        updateLCDStatus('U');
        uploadPicture();
        Serial.println("Done. Check your dropbox.");
        updateLCDStatus(' ');
      }
    }
    //  delay(50); // extra msec delay not needed due to IR timeout
}
