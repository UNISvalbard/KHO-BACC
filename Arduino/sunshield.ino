// BACC servo control program
//
// Uses serial port (9600, 8, N) to control the position of a RC servo. The expected input value
// is the control pulse length in microseconds
//
// Mikko Syrjasuo/UNIS
//
// 2017-06-30 Improved servo handling
//  - the servo is detached after a timeout. This "relaxes" the servo and reduces used power
//
// 2017-05-26 First version
//

//#define DEBUG

#include <Servo.h>

#define MINPOS 300    // Pulse length in us
#define MAXPOS 3000
#define SERVOPIN 2   // This is the digital output pin that controls the RC-servo position
#define TIMEOUT 5

Servo myservo;  // create servo object to control a servo

unsigned int pos = MAXPOS;    // close the shutter at reset
String cmd;
unsigned int newpos=pos;
unsigned int timeoutcounter = TIMEOUT;

void setup() {
  myservo.attach(SERVOPIN);  // attaches the servo on pin 9 to the servo object

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Switch off the LED

  Serial.begin(9600);
  //  while (!Serial); // wait for serial port to connect, needed for native USB port only
#ifdef DEBUG
  Serial.println("BACC sunshield v0.2");
  Serial.flush();  // Wait for the outgoing message to be gone
#endif

// Open and then close
  if (myservo.attached()) {
    myservo.writeMicroseconds(MINPOS);
    delay(1000);
    myservo.writeMicroseconds(2280);
    delay(2000);
    myservo.detach();
  }

}

void loop() {
  if (myservo.attached()) {
    myservo.writeMicroseconds(pos);
  }
  delay(1000); // 1s delay used for shutting down the servo
  if (timeoutcounter > 0) {
#ifdef DEBUG
    Serial.println(timeoutcounter);
#endif
    timeoutcounter--;
    if (timeoutcounter == 0) {
      myservo.detach();
#ifdef DEBUG
      Serial.println("Timeout");
#endif
    }
  }

  //  myservo.writeMicroseconds(2250);
  //  delay(1000);

  // Clear the serial receive buffer by reading until there's nothing to read (maybe not, because this will jam the whole thing...
  //while (Serial.available())
  //  Serial.read();

  while (Serial.available()) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // Toggle LED when getting something from the serial line
    cmd = Serial.readString();
#ifdef DEBUG
    Serial.print("Received: ");
    Serial.println(cmd);
#endif DEBUG
    newpos = (unsigned int)cmd.toInt();
    if (newpos == 0) { // Timeout or nonsensible data
#ifdef DEBUG
      Serial.println("BACC sunshield v0.1 - restart due to timeout or weird data");
      Serial.flush();
#endif

    } else {
      if (newpos > MAXPOS) {
        newpos = MAXPOS;
      } else if (newpos < MINPOS) {
        newpos = MINPOS;
      }

#ifdef DEBUG
      Serial.print("Pos=");
      Serial.println(newpos);
      Serial.flush();
#endif
      pos = newpos;
      timeoutcounter = TIMEOUT; // Restart the timeout counter
      if (!myservo.attached()) {
        myservo.attach(SERVOPIN);
      }

    }
  }
}
