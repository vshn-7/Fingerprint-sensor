#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

const int Switch    = 4; 
const int PIN_RED   = 5;
const int PIN_GREEN = 6;
const int PIN_BLUE  = 7;
int buttonState      =0;
int lastButtonState  =0;
unsigned long startTime     = 0;
unsigned long endTime       = 0;
unsigned long holdTime      = 0;
unsigned long idleTime      = 0; 

SoftwareSerial mySerial(2, 3);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;
void setup()
{
pinMode(Switch,INPUT);  
Serial.begin(9600);
while (!Serial); // For Yun/Leo/Micro/Zero/…
delay(100);

Serial.println("fingertest");
pinMode(12, OUTPUT);
pinMode(11, OUTPUT);
pinMode(PIN_RED,  OUTPUT);
pinMode(PIN_GREEN,OUTPUT);
pinMode(PIN_BLUE, OUTPUT);
// set the data rate for the sensor serial port
finger.begin(57600);

if (finger.verifyPassword()) {
Serial.println("Found fingerprint sensor!");
} else {
Serial.println("Did not find fingerprint sensor ");
while (1) {
delay(1);
}
}

finger.getTemplateCount();
Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
Serial.println("Waiting for valid finger…");
}
uint8_t readnumber(void) {
uint8_t num = 0;

while (num == 0) {
while (! Serial.available());
num = Serial.parseInt();
}
return num;
}

void loop() // run over and over again

{
  buttonState = digitalRead(Switch);
if(buttonState != lastButtonState){
  updateState();
}
lastButtonState = buttonState;
  if((holdTime>=0)&&(holdTime<5000))
{getFingerprintIDez();
delay(50); //don’t ned to run this at full speed.      //verification call function
digitalWrite(12, LOW);
digitalWrite(11, LOW);}

else if((holdTime>=5000)&&(holdTime<=10000)){
  Serial.println("Ready to enroll a fingerprint!");
Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as…");
id = readnumber();
if (id == 0) {// ID #0 not allowed, try again!
return;
}
Serial.print("Enrolling ID #");
Serial.println(id);

while (! getFingerprintEnroll() );                    //enrollment call function

}
else{
  Serial.println("Type in the ID # you want delete...");
  uint8_t id = 0;
  while (true) {
    while (! Serial.available());
    char c = Serial.read();
    if (! isdigit(c)) break;
    id *= 10;
    id += c - '0';
  }
  Serial.print("deleting ID #");
  Serial.println(id);
  
  deleteFingerprint(id);                     //deletion call function
}
}
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
    Colour(0,240,0);
    delay(700);
    Colour(0,0,0);
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    Colour(240,0,0);
    delay(700);
    Colour(0,0,0);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    Colour(240,0,0);
    delay(700);
    Colour(0,0,0);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    Colour(240,0,0);
    delay(700);
    Colour(0,0,0);
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    Colour(240,0,0);
    delay(700);
    Colour(0,0,0);
    return p;
  }   
}

uint8_t getFingerprintID() {
if((holdTime>=0)&&(holdTime<5000)){
uint8_t p = finger.getImage();
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image taken");
break;
case FINGERPRINT_NOFINGER:
Serial.println("No finger detected");
Colour(240,0,0);
delay(700);
Colour(0,0,0);
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
Colour(240,0,0);
delay(700);
Colour(0,0,0);
return p;
case FINGERPRINT_IMAGEFAIL:
Serial.println("Imaging error");
Colour(240,0,0);
delay(700);
Colour(0,0,0);
return p;
default:
Serial.println("Unknown error");
Colour(240,0,0);
delay(700);
Colour(0,0,0);
return p;
}

// OK success!

p = finger.image2Tz();
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image converted");
break;
case FINGERPRINT_IMAGEMESS:
Serial.println("Image too messy");
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
return p;
case FINGERPRINT_FEATUREFAIL:
Serial.println("Could not find fingerprint features");
return p;
case FINGERPRINT_INVALIDIMAGE:
Serial.println("Could not find fingerprint features");
return p;
default:
Serial.println("Unknown error");
return p;
}

// OK converted!
p = finger.fingerFastSearch();
if (p == FINGERPRINT_OK) {
Serial.println("Found a print match!");

} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
Serial.println("Communication error");
Colour(240,0,0);
delay(700);
Colour(0,0,0);
return p;
} else if (p == FINGERPRINT_NOTFOUND) {
Serial.println("Did not find a match");
Colour(240,0,0);
delay(700);
Colour(0,0,0);
return p;
} else {
Serial.println("Unknown error");
Colour(240,0,0);
delay(700);
Colour(0,0,0);
return p;
}
{digitalWrite(11, HIGH);
delay(3000);
digitalWrite(11, LOW);
Serial.print("Not Found");
Serial.print("Error");
return finger.fingerID;
}

// found a match!
Serial.print("Found ID #"); Serial.print(finger.fingerID);
Serial.print(" with confidence of "); Serial.println(finger.confidence);

return finger.fingerID;
}}
uint8_t getFingerprintEnroll() {

int p = -1;
Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
while (p != FINGERPRINT_OK) {
p = finger.getImage();
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image taken");
break;
case FINGERPRINT_NOFINGER:
Serial.println(".");
break;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
break;
case FINGERPRINT_IMAGEFAIL:
Serial.println("Imaging error");
break;
default:
Serial.println("Unknown error");
break;
}
}

// OK success!

p = finger.image2Tz(1);
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image converted");
break;
case FINGERPRINT_IMAGEMESS:
Serial.println("Image too messy");
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
return p;
case FINGERPRINT_FEATUREFAIL:
Serial.println("Could not find fingerprint features");
return p;
case FINGERPRINT_INVALIDIMAGE:
Serial.println("Could not find fingerprint features");
return p;
default:
Serial.println("Unknown error");
return p;
}

Serial.println("Remove finger");
delay(2000);
p = 0;
while (p != FINGERPRINT_NOFINGER) {
p = finger.getImage();
}
Serial.print("ID "); Serial.println(id);
p = -1;
Serial.println("Place same finger again");
while (p != FINGERPRINT_OK) {
p = finger.getImage();
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image taken");
break;
case FINGERPRINT_NOFINGER:
Serial.print(".");
break;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
break;
case FINGERPRINT_IMAGEFAIL:
Serial.println("Imaging error");
break;
default:
Serial.println("Unknown error");
break;
}
}

// OK success!

p = finger.image2Tz(2);
switch (p) {
case FINGERPRINT_OK:
Serial.println("Image converted");
break;
case FINGERPRINT_IMAGEMESS:
Serial.println("Image too messy");
return p;
case FINGERPRINT_PACKETRECIEVEERR:
Serial.println("Communication error");
return p;
case FINGERPRINT_FEATUREFAIL:
Serial.println("Could not find fingerprint features");
return p;
case FINGERPRINT_INVALIDIMAGE:
Serial.println("Could not find fingerprint features");
return p;
default:
Serial.println("Unknown error");
return p;
}

// OK converted!
Serial.print("Creating model for #"); Serial.println(id);

p = finger.createModel();
if (p == FINGERPRINT_OK) {
Serial.println("Prints matched!");
} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
Serial.println("Communication error");
Colour(240,0,0);
delay(700);
Colour(0,0,0);
return p;
} else if (p == FINGERPRINT_ENROLLMISMATCH) {
Serial.println("Fingerprints did not match");
Colour(240,0,0);
delay(700);
Colour(0,0,0);
return p;
} else {
Serial.println("Unknown error");
return p;
}

Serial.print("ID "); Serial.println(id);
p = finger.storeModel(id);
if (p == FINGERPRINT_OK) {
Serial.println("Stored!");
Colour(0,240,0);
delay(700);
Colour(0,0,0);
} else if (p == FINGERPRINT_PACKETRECIEVEERR) {
Serial.println("Communication error");
return p;
} else if (p == FINGERPRINT_BADLOCATION) {
Serial.println("Could not store in that location");
return p;
} else if (p == FINGERPRINT_FLASHERR) {
Serial.println("Error writing to flash");
return p;
} else {
Serial.println("Unknown error");
return p;
}
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  if((holdTime>=0)&&(holdTime<5000)){
uint8_t p = finger.getImage();
if (p != FINGERPRINT_OK) return -1;

p = finger.image2Tz();
if (p != FINGERPRINT_OK) return -1;

p = finger.fingerFastSearch();
if (p != FINGERPRINT_OK) return -1;

// found a match!

{
Colour(0,240,0);  
digitalWrite(12, HIGH);
delay(2000);
Colour(0,0,0);
digitalWrite(12, LOW);
Serial.print("Found ID #"); Serial.print(finger.fingerID);
Serial.print(" with confidence of "); Serial.println(finger.confidence);

} }}
void Colour(int R,int G,int B)
{ analogWrite(PIN_RED,  R);
  analogWrite(PIN_GREEN,G);
  analogWrite(PIN_BLUE, B);
}
void updateState(){
 if(buttonState==HIGH){
  startTime = millis();
  idleTime = startTime-endTime;
 }
 else{
 endTime = millis();
 holdTime=endTime-startTime;
 }
}
