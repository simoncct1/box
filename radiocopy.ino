#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        2 // On Trinket or Gemma, suggest changing this to 1
#define PIN2        3 
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 13 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUMPIXELS, PIN2, NEO_GRB + NEO_KHZ800);


// PI message
String nextPiMessage = "";

// radio volume
int radioVolume = 0;
int radioVolumeSwitchPin = 13;
int radioVolumePotPin = A0;
int radioVolumePotValue = 0;
int radioVolumePotMaxValue = 1000;
int radioVolumePoxMinValue = 0;
int radioPower = LOW;

// radio Freq
int radioFreq = 0;
int radioFreqPotPin = A1;
int radioFreqPotValue = 0;
int radioFreqPotMaxValue = 1000;
int radioFreqPoxMinValue = 0;

int internalLed = 11; // May be different on your board






void setup() {

  //start serial connection
  Serial.begin(9600);

  // volume
  pinMode(radioVolumeSwitchPin, INPUT_PULLUP);

  // internal led
  pinMode(internalLed, OUTPUT);


  //leds
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin(); 
}



void loop() {

  if (digitalRead(radioVolumeSwitchPin) == HIGH) {
    radioPower = LOW;
  } else {
    radioPower = HIGH;
  }


  digitalWrite(internalLed, radioPower);
  //TODO :: add radio led stuff HERE

  // Volume
  radioVolumePotValue = analogRead(radioVolumePotPin); 
  radioVolume = map(radioVolumePotValue, radioVolumePotMaxValue, radioVolumePoxMinValue, 0, 100);

  // Frequence
  radioFreqPotValue = analogRead(radioFreqPotPin);
  // This line is maybe useless ...
  radioFreq = map(radioFreqPotValue, radioFreqPotMaxValue, radioFreqPoxMinValue, radioFreqPoxMinValue, radioFreqPotMaxValue);

  // Output
  setNextPiMessage("radio -- p:"+String(radioPower)+" v:"+String(radioVolume)+" f:"+String(radioFreq));

  // How to play a sound: 
  // setNextPiMessage("sound -- SoundId");
  // Replace SoundId by the id in the sounds array (radio.py:29)
 
    if (digitalRead(radioVolumeSwitchPin) == 1) {
    for(int i = 0; i <= 13 ; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
    pixels.show(); 
    pixels2.setPixelColor(i, pixels.Color(0,0,0));
    pixels2.show();  // Send the updated pixel colors to the hardware.
    }
  } else if(radioFreq == 815){
    for(int i = 0; i <= 13 ; i++){
    pixels.setPixelColor(i, pixels.Color(100, 30, 0));
    pixels.show(); 
    pixels2.setPixelColor(i, pixels.Color(100, 30, 0));
    pixels2.show();   // Send the updated pixel colors to the hardware.
    }
  }
  sendToPi();
  delay(100);


}

void setNextPiMessage(String message){
  nextPiMessage = message;
}

void sendToPi() {
  Serial.println(nextPiMessage);
  nextPiMessage = "";
}
