#include <Arduino.h>
#include <AccelStepper.h>
// 1000pas=3,5cm in real life
// vitesse lien serie USB
#define SERIAL_RATE 9600
// taille max tampon donnees lien serie
#define STRING_SIZE 255

// All the wires needed for full functionality
#define STEP 9
#define DIR 8
#define DISBL 7
#define FIN_COURSE1 1
#define FIN_COURSE2 2
#define BOUTON_STOP 3
// vitesse rotation moteur cible;
#define MAX_SPEED 2400
#define ACCELERATION 1000

// garder le moteur alimente pour tenir la position
AccelStepper stepper (1, STEP, DIR);

// Serial vars
unsigned int inputCharCount = 0;
String inputString = "";  // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// pas a faire tourner
double pas = 0;


void setup() {
  Serial.begin(SERIAL_RATE);
  inputString.reserve(STRING_SIZE);
  while (!Serial) {;}
  Serial.println("Parametrez le moniteur serie pour l'envoi du caractere 'Nouvelle Ligne'\n La distance que vous voulez faire doit etre en mm");
  stringComplete = false;
  pinMode(DISBL, OUTPUT); // disable coil pin
  pinMode(FIN_COURSE1, INPUT_PULLUP);
  pinMode(FIN_COURSE2, INPUT_PULLUP);
  pinMode(BOUTON_STOP, INPUT_PULLUP);
  digitalWrite(DISBL, LOW); // disable coils
  attachInterrupt(digitalPinToInterrupt(BOUTON_STOP), arret_urgence, FALLING);
  attachInterrupt(digitalPinToInterrupt(FIN_COURSE1), arret_course2, FALLING);
  attachInterrupt(digitalPinToInterrupt(FIN_COURSE2), arret_course1, FALLING);
  stepper.setMaxSpeed(MAX_SPEED); // Set target motor RPM.
}


void loop() {
  serialEvent();
  if (stringComplete) {
    Serial.print("serial message received:");
    Serial.println(inputString);
    if (inputString.startsWith("-")) {
      pas = 0 - (inputString.substring(1).toInt()/0.035);
    } else {
      pas = (inputString.toInt()/0.035);
    }
    inputCharCount = 0;
    inputString = "";
    stringComplete = false;
  }
  else if (pas != 0) {
    Serial.print("moving steps:");
    Serial.println(pas);
    stepper.setAcceleration (ACCELERATION);
    stepper.move(pas); // move avec un nombre en step
    pas = 0;
    digitalWrite(DISBL, HIGH); // active coils
  }
  stepper.run();
  if (stepper.speed() == 0) {
    digitalWrite(DISBL, LOW); // disable coils
  }
}

void arret_urgence() {
  Serial.println("arret urgence \n");
  stepper.setSpeed(0);
  stepper.moveTo(stepper.currentPosition());
  // stepper.stop(); // slow stop, deccelerate before stop ...
  pas = 0;
}
void arret_course1() {
  Serial.println("arret course1 \n");
  stepper.setSpeed(0);
  stepper.moveTo(stepper.currentPosition());
  // stepper.stop(); // slow stop, deccelerate before stop ...
  pas = 0;
}
void arret_course2() {
  Serial.println("arret course2 \n");
  stepper.setSpeed(0);
  stepper.moveTo(stepper.currentPosition());
  // stepper.stop(); // slow stop, deccelerate before stop ...
  pas = 0;
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputCharCount ++;
    if (inputCharCount < STRING_SIZE) {
      inputString += inChar;
      if (inChar == '\n') {
        stringComplete = true;
      }
    } else {
      stringComplete = true;
    }
  }
}

