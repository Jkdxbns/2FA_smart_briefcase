/*
 * You will get characters on output screen
 * 
 * 0 - Setup done
 * 1 - Stage1 fingerprint authorized
 * 2 - Stage2 keypad is taking values
 * K - keypad done taking values
 * C - low fingerprint confidence
 * A - user authorised
 * L - bag is locked
 * I - incorrect otp. Sends new OTP
 * 
 */

/* Add Servo*/


#include <Adafruit_Fingerprint.h>
#include "Adafruit_Keypad.h"
#if(defined(__AVR__) || defined(ESP8266)) || !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(8, 9);
#else
#define mySerial Serial1
#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
bool fingerprint = false;
bool fingerr = false;
char InBytes;
int buz = 7;
String incoming_otp;
const byte ROWS = 4;                      // Define rows of keypad
uint8_t id;
const byte COLS = 4;                      // Define columns of keypad
byte rowPins[ROWS] = {5, 4, 3, 2};        // Define pins of rows of keypad
byte colPins[COLS] = {13, 12, 11, 10};    // Define pins of columns of keypad
//char global_arr[5];                       // Create an array of length = 5
char python_sent_otp[5] = {};
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},                   // Pattern of keypad
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);





void incorrect_finger_buz()
{
  digitalWrite(buz, LOW);
  delay(1000);
  digitalWrite(buz, HIGH);
}

void sms_sent_buz()
{
  bool state = true;
  for (int i=0; i<7; i++)
  {
  digitalWrite(buz, state);
  state = !state;
  delay(200);  
  }
}

void incorrect_otp_buz()
{
   bool state = true;
   for (int i = 0; i<21; i++)
   {
      digitalWrite(buz, state);
      state = !state;
      delay(100);  
   }
}

void bag_lock_buz()
{
  digitalWrite(buz, LOW);
  delay(5000);
  digitalWrite(buz, HIGH);
}

void bag_opened_buz()
{
  digitalWrite(buz, LOW);
  delay(2500);
  digitalWrite(buz, HIGH);
}







void setup() {
  Serial.begin(9600);
  pinMode(buz, OUTPUT);
  digitalWrite(buz, HIGH);
  Serial.println('0');
  fingerprint_setup();                //begins fingerprintln sensor
  keypad_setup();                     //begins keypad
}


void loop() {
    getFingerprintID();                           //gets fingerprintln and checks in stored templates
    delay(50);
}



//-----------Check fingerprint----------//
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print('.');
      return p;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK: break;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println('1');
    int finger_confidence = finger.confidence;
    if ((int) finger_confidence < 100) {
      Serial.println('C');
    }
    else if ((int) finger_confidence > 100) {
      fingerr = true;
      python_send_otp();
      get_keypad_values();
      while (true)
      {
        if (Serial.available())
        {
          InBytes = Serial.read();
          if (InBytes == 'A')
          {
            bag_opened_buz();
            //unlock_bag();
          }
          else if (InBytes == 'I')
          {
            incorrect_otp_buz();
            get_keypad_values();
          }
          else if (InBytes == 'L')
          {
            bag_lock_buz();
          }
        }
      }
    }
  }
  else if (p == FINGERPRINT_NOTFOUND)   {
    Serial.println('E');
    incorrect_finger_buz();
    return p;
  }
}



//----------Fingerprint sensor setup----------//
void fingerprint_setup() {
  finger.begin(57600);
  delay(5);
  while (!finger.verifyPassword()) {
    Serial.println('F');
  }
}

void check_fingerprint() {
  getFingerprintID();
  delay(50);
}


void keypad_setup() {
  customKeypad.begin();
}

void get_keypad_values() {
  Serial.print('2');
  char key_arr[6];                                                // Creates new aray everytime this function is called
  int i = 0;
  while (true) {
    customKeypad.tick();
    if (customKeypad.available()) {
      keypadEvent  e = customKeypad.read();
      if ((char)e.bit.KEY && e.bit.EVENT == KEY_JUST_PRESSED) {
        key_arr[i] = (char)e.bit.KEY;                             // Store pressed key into the array created
        i++;
      }
    }
    if (i == 5) {
      Serial.print('K');
      delay(50);
      for (int i = 0; i<5; i++)
      {
        Serial.print(key_arr[i]);
        delay(100);
      }
      break;
    }
  }
  i = 0;
}


void python_send_otp() {
  while (true)
  {
    if (Serial.available())
    {
      InBytes = Serial.read();
      Serial.print('1');
      sms_sent_buz();
      break;
      
    }
    delay(100);
  }
}
