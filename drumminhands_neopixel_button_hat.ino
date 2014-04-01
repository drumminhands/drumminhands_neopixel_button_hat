/* 
  Modified by combining these tutorials
    Blink without Delay  http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
    State change detection (edge detection) http://arduino.cc/en/Tutorial/ButtonStateChange
    Adafruit Strandtest https://github.com/adafruit/Adafruit_NeoPixel
    http://www.arduino.cc/en/Tutorial/Debounce
    https://github.com/EternalCore/NeoPixel_Cylon_Scrolling_Eye
 */
#include <Adafruit_NeoPixel.h>

// constants won't change. 
const int buttonPin = 0; // the number of the pushbutton pin
#define PIN 1 //the number of the neopixel pic
#define numPixelsInStrip 8 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixelsInStrip, PIN, NEO_GRB + NEO_KHZ800);

// Variables will change:
int ledState = HIGH; // the current state of the output pin
int buttonState; // the current reading from the input pin
int lastButtonState = LOW; // the previous reading from the input pin
long lastDebounceTime = 0; // the last time the output pin was toggled
long debounceDelay = 50; // the debounce time; increase if the output flickers
int programState = 0; // current program seleted -- buttonpresses rotate between programs
int numOfPrograms = 7; // how many programs are there total, not including the off state

//unsigned long currentMillis = 0;  //for programs
long previousMillis = 0;        // will store last time LED was updated
int neoPixelToChange = 0; //track which neoPixel to change
int neoPixel_j = 0; //stores values for program cycles
int defaultBrightness = 64;

//cylon variables
int fadeDirection = -1;//change sigen to fade up or down
boolean cylonDirection = true; //keeps track of the direction the pixels should swipe
boolean cylonPause = false; //keeps track of the pause inbetween swipes
long delayMillis = 0; // will store the last time the cylon swipe was paused

void setup() {
  pinMode(buttonPin, INPUT);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(defaultBrightness); // initialize brightness
}

void loop() {
  
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      
      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {        
        //change the program
        programState = (programState + 1) % (numOfPrograms + 1); //include numOfPrograms + 1, since there is an off state
      }
    }
  }

  // save the reading.  Next time through the loop
  lastButtonState = reading;
  
  //run the chosen program
  switch (programState) {
    case 0:
      allColor(strip.Color(0,0,0)); //off
      break; 
    case 1:
      blinkColor(strip.Color(255,0,0),100); // red
      break;        
    case 2:
      colorWipeCycle(strip.Color(0,255,0),500); // green
      break;
    case 3:
      softBlink(strip.Color(0,0,255),defaultBrightness, 50); // blue
      break;
    case 4:
      strip.setBrightness(defaultBrightness); // initialize brightness
      rainbow(20);
      break;
    case 5:
      rainbowCycle(20);
      break;
    case 6:
      cylonEye(40, 500); //not sure the pause time is actually working ???????????????????????
      break;
    default: 
      // if nothing else matches, do the default 
      allColor(strip.Color(0,0,0)); //off
  } //NOTE if you change the number of programs in the switch statement above, be sure to change the variable numOfPrograms at the top
}

// Fill all the dots with one color
void allColor(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
  }
} // note for a random color, use:
  // allColor((unsigned long)random(0x01000000)); // random color
  
void blinkColor(uint32_t c, uint8_t wait) {
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > wait) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;   

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW){
      ledState = HIGH;
      allColor(c); 
    } else {
      ledState = LOW;
      allColor(strip.Color(0,0,0)); // off
    }
  } 
}  
  
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  
  unsigned long currentMillis = millis();
  
  //only do this if some of the pixels still need to be lit
  if (neoPixelToChange <= strip.numPixels()){
    
    if(currentMillis - previousMillis > wait) {
      
      // save the last time you changed a NeoPixel 
      previousMillis = currentMillis;  
    
      //change a pixel
      strip.setPixelColor(neoPixelToChange, c);
      strip.show();
      neoPixelToChange++;
    }
  }
}

// Fill the dots one after the other with a color
void colorWipeCycle(uint32_t c, uint8_t wait) {
  
  unsigned long currentMillis = millis();
  
  //only do this if some of the pixels still need to be lit
  if (neoPixelToChange <= strip.numPixels()){
    
    if(currentMillis - previousMillis > wait) { 
      
      // save the last time you changed a NeoPixel 
      previousMillis = currentMillis;  
    
      //change a pixel
      strip.setPixelColor(neoPixelToChange, c);
      strip.show();
      neoPixelToChange++;
      
    } 
    
  } else {
    // reset the cycle
    previousMillis = 0;  
    neoPixelToChange = 0;
    allColor(strip.Color(0,0,0));
  }
}

void rainbow(uint8_t wait) {
    
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > wait) {
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+neoPixel_j) & 255));
    }
    strip.show();
    neoPixel_j = (neoPixel_j + 1) % 255; //increment j until all colors are used, then start over
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > wait) {
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 

    //change the colors of the pixels
    uint16_t i;

    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + neoPixel_j) & 255));
    }
    strip.show();
    neoPixel_j = (neoPixel_j + 1) % 1279; // 5 cycles of all colors on wheel, then start over
  }
}

void softBlink(uint32_t c, uint8_t brightness, uint8_t wait) {
  
  unsigned long currentMillis = millis();

  if(currentMillis - previousMillis > wait) {
  
    //set the color of all pixels
    allColor(c); // is there a way to set this only once per program ????????????????????
    
    // save the last time you changed a NeoPixel 
    previousMillis = currentMillis; 
    
    uint16_t i;
    int b = (neoPixel_j * brightness) / brightness;
    strip.setBrightness(b);
    strip.show(); 
    // sometime figure out how to get this to fade down too instead of just restarting //////////////////////
    neoPixel_j = (neoPixel_j + 1) % brightness;
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void cylonEye(uint8_t swipeSpeed, uint8_t pauseTime) {

  if (cylonPause) { //we are on a pause break from swipes

    unsigned long currentPauseMillis = millis();

    //check to see if we've waited long enough
    if ((currentPauseMillis - delayMillis) > pauseTime) { /////////////////////////////// this is not getting called. Why????????????????
      // save the last time you checked the pause
      delayMillis = currentPauseMillis;
      cylonPause = false; //end the pause
    }

  } else {

    //if needed, change directions
    if (neoPixelToChange > numPixelsInStrip) {
      cylonDirection = false;
    }
    if (neoPixelToChange < 0) {
      cylonDirection = true;
      cylonPause = true; //take a break from the swipe
      //turn all pixels off
      for (uint16_t i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
      strip.show();
      delayMillis = millis();
    }

    //run the swipe
    if (cylonDirection) {
      cylonUp(strip.Color(255, 0, 0), strip.Color(72, 0, 0), swipeSpeed); // red
    } else {
      cylonDown(strip.Color(255, 0, 0), strip.Color(72, 0, 0), swipeSpeed); // red
    }
  }
}

void cylonUp(uint32_t c0, uint32_t c1, uint8_t wait) {

  unsigned long currentMillis = millis();

  //neoPixelToChange * wait
  if (currentMillis - previousMillis > wait) {

    //turn all pixels off
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }

    // save the last time you changed a NeoPixel
    previousMillis = currentMillis;

    //change a pixel
    strip.setPixelColor(neoPixelToChange, c0); //primary color
    strip.setPixelColor(neoPixelToChange - 1, c1); //secondary color
    strip.setPixelColor(neoPixelToChange + 1, c1); //secondary color
    strip.show();
    neoPixelToChange++;
  }
}

void cylonDown(uint32_t c0, uint32_t c1, uint8_t wait) {

  unsigned long currentMillis = millis();

  //neoPixelToChange * wait
  if (currentMillis - previousMillis > wait) {

    //turn all pixels off
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }

    // save the last time you changed a NeoPixel
    previousMillis = currentMillis;

    //change a pixel
    strip.setPixelColor(neoPixelToChange, c0); //primary color
    strip.setPixelColor(neoPixelToChange - 1, c1); //secondary color
    strip.setPixelColor(neoPixelToChange + 1, c1); //secondary color
    strip.show();
    neoPixelToChange--; //is there any way to combine this with cylonUp, since this is the only line that is different?
  }
}
