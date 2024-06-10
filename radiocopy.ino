#include <Bounce2.h> 

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

// pins radio led
#define PIN        2 // On Trinket or Gemma, suggest changing this to 1
#define PIN2        3 
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 13 // Popular NeoPixel ring size


//setting radio leds
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


//--------------------------------------------------
// Etape 5: "4 Boutons" - flag Etape_5
// Preconditions:
// - L'étape Board_Nano.Lecture_L1 est terminée avec L1_RELAY_PIN à HIGH


#define FOURBUTTONS_B1_PIN 4 //bouton egalement connect au Nano Board pour callback loop RFID
#define FOURBUTTONS_B2_PIN 5
#define FOURBUTTONS_B3_PIN 6
#define FOURBUTTONS_B4_PIN 7
#define FOURBUTTONS_OUTLED_PIN 33
#define DELAYVAL 50
#define NUMPIXELS 1

Adafruit_NeoPixel pixels3(NUMPIXELS, FOURBUTTONS_OUTLED_PIN, NEO_GRB + NEO_KHZ800);



Bounce2::Button button1 = Bounce2::Button();
Bounce2::Button button2 = Bounce2::Button();
Bounce2::Button button3 = Bounce2::Button();
Bounce2::Button button4 = Bounce2::Button();


int counter = 0;
int flag1 = 0;
int flag2 = 1;
int flag3 = 0;
int flag4 = 1;

int cpt1 = 0;
int cpt2 = 0;
int cpt3 = 0;
int cpt4 = 0;

//--------------------------------------------------
// Etape 6: "Hexagones" - flag Etape_6
// Preconditions:
// - Informations venues de l'étape 5 - 4boutons: 1 chiffre


#define HEX_POUR_PIN 22
#define HEX_CONTRE_PIN 23
#define HEX_ABST_PIN 24
#define INDICATOR_LED_PIN 25
#define COVER_RELAY_PIN 26
#define FLAP_RELAY_PIN 27
const int OPEN_DELAY = 10; // delai pour ouvrir le couvercle

//--------------------------------------------------
// Etape 7: "Couvercle-Coffre fort" - flag Etape_7
#define ALARM_PIN 28
#define JACK_ALARM_PIN 30 // interrupteur de l'alarme
#define BUTTON_TEST_PIN 8 // simule le COVER_RELAY_PIN de "Hexagones" en amont
#define NUM3_LED_PIN 32

Bounce2::Button button_test = Bounce2::Button(); //bouton de test



//--------------------------------------------------


void setup() {

  //start serial connection
  Serial.begin(9600);

  // volume
  pinMode(radioVolumeSwitchPin, INPUT_PULLUP);

  // internal led
  pinMode(internalLed, OUTPUT);

  //leds radio setup
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif
    pixels.begin(); 
    pixels2.begin();
    pixels3.begin();


    //--------------------------------------------------
// Etape 5: "4 Boutons" - flag Etape_5

  button1.attach (FOURBUTTONS_B1_PIN,INPUT_PULLUP); 
  // ce bouton est partagé avec Board_Nano.button_Mega1; il fermera le solénoide lorsqu'il sera pressé la première fois
  // Il force le Board_Nano à l'étape Etape_L2
  button1.interval(5);
  button1.setPressedState(LOW); 

  button2.attach (FOURBUTTONS_B2_PIN,INPUT_PULLUP);
  button2.interval(5);
  button2.setPressedState(LOW); 

  button3.attach (FOURBUTTONS_B3_PIN,INPUT_PULLUP);
  button3.interval(5);
  button3.setPressedState(LOW); 

  button4.attach (FOURBUTTONS_B4_PIN,INPUT_PULLUP);
  button4.interval(5);
  button4.setPressedState(LOW); 

//--------------------------------------------------
// Etape 6: "Hexagones" - flag Etape_6
// 

  pinMode(COVER_RELAY_PIN, OUTPUT);
  digitalWrite(COVER_RELAY_PIN, LOW);
  pinMode(FLAP_RELAY_PIN, OUTPUT);
  digitalWrite(FLAP_RELAY_PIN, LOW);
  pinMode(INDICATOR_LED_PIN,OUTPUT);
  digitalWrite(INDICATOR_LED_PIN, LOW);

//--------------------------------------------------

}

//step
int step = 1;

void loop() {
  Serial.println(step);
  //steps
  if(step==1){
  
    fourButtons();
  }
  if(step==2){
    hexagones();
  }
  // radio
  radioManagment();
  //radio leds
  radioLedsManagment();
  //send to pi
  sendToPi();
  
}

void hexagones(){
      int valPours = digitalRead(HEX_POUR_PIN);
    //valContres = analogRead(HEX_CONTRE_PIN);
     int valContres = digitalRead(HEX_CONTRE_PIN);
    //valAbstentions = analogRead(HEX_ABST_PIN);
     int valAbstentions = digitalRead(HEX_ABST_PIN);
  
    if (valPours  == LOW && valContres == LOW && valAbstentions == LOW) {
      Serial.println("Magnet Detected!");
      digitalWrite(COVER_RELAY_PIN, HIGH);
//      progressBar(OPEN_DELAY); 
      // step = 3;
      pixels3.setPixelColor(NUMPIXELS-1, pixels.Color(0, 0, 0)); 
      pixels3.show();
    } else {
      Serial.println("No Magnet.");
      digitalWrite(COVER_RELAY_PIN, LOW);
    }

    delay(500); // Petit délai pour éviter une lecture trop rapide
}

void progressBar(int time_delay) {
    for (int i=0; i < time_delay; i++) {
      Serial.print(".");
      delay(1000);
    }
    Serial.println();
}

void fourButtons(){
  // Serial.println("bb");
    button1.update();
    button2.update();
    button3.update();
    button4.update();
    if ( button1.pressed() ) {
      cpt1++;
      counter++;
      Serial.println("Bouton 1");
      if (cpt1 == 4){
        flag1 = 1;
      }
    }
    if ( button2.pressed() ) {
      cpt2++;
      counter++;
      Serial.println("Bouton 2");
      if (cpt2 == 0){
        flag2 = 1;
      }else{
        flag2 = 0;
      }
    }
    if ( button3.pressed() ) {
      cpt3++;
      counter++;
      Serial.println("Bouton 3");
      if (cpt3 == 4 && flag1 == 1 && flag2 == 1){
        flag3 = 1;
      }
    }
    if ( button4.pressed() ) {
     
      cpt4++;
      counter++;
      Serial.println("Bouton 4");
      if (cpt4 == 0){
        flag4 = 1;
      }else{
        flag4 = 0;
      }
    }

    if (flag1 == 1 && flag2 == 1 && flag3 == 1 && flag4 == 1){
      Serial.println("Good!");
      counter = 0;
      cpt1 = 0; flag1 = 0;
      cpt2 = 0; flag2 = 1;
      cpt3 = 0; flag3 = 0;
      cpt4 = 0; flag4 = 1;
      pixels3.setBrightness(50);
      pixels3.show();
      pixels3.setPixelColor(NUMPIXELS-1, pixels.Color(200, 60, 0)); 
      pixels3.show();
     
      // changement vers la boucle d'ouverture "Hexagones"
      step = 2;
    }
    else if (counter == 8){
      Serial.println("sound -- message4");
      counter = 0;
      cpt1 = 0; flag1 = 0;
      cpt2 = 0; flag2 = 1;
      cpt3 = 0; flag3 = 0;
      cpt4 = 0; flag4 = 1;
    }
}

void radioManagment(){

    if (digitalRead(radioVolumeSwitchPin) == HIGH) {
    radioPower = LOW;
  } else {
    radioPower = HIGH;
  }

  digitalWrite(internalLed, radioPower);

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

    delay(100);

}

void radioLedsManagment(){
    if (digitalRead(radioVolumeSwitchPin) == 1) {
    for(int i = 0; i <= 13 ; i++){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
    pixels.show(); 
    pixels2.setPixelColor(i, pixels2.Color(0,0,0));
    pixels2.show();  // Send the updated pixel colors to the hardware.
    }
  } else if(radioFreq <= 820 && radioFreq >= 810){
    for(int i = 0; i <= 13 ; i++){
    pixels.setPixelColor(i, pixels.Color(200, 60, 0));
    pixels.show(); 
    pixels2.setPixelColor(i, pixels2.Color(200, 60, 0));
    pixels2.show();   // Send the updated pixel colors to the hardware.
    }
  }
}

void setNextPiMessage(String message){ 
  nextPiMessage = message;
}

void sendToPi() {
  Serial.println(nextPiMessage);
  nextPiMessage = "";
}
