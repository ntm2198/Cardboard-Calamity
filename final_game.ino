#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

//servo definitions
#define SERVOMIN  150   // pulse length count for 0 degrees (~500us)
#define SERVOMAX  450   // pulse length count for 180 degrees (~2500us)
#define SERVO_FREQ 50   // analog servos run at ~50 Hz updates

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

//instantiation of the objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// uint8_t myPins[] = {0,1,2};
int inputPins[] = {53,51,49,47,45};
int inputCount = 5;

bool readings[] = {0,0,0,0,0};
float angles[] = {0,0,0,0,0};
float speedFactor[] = {1.1,1.2,1,1.1,1};

bool failed = false;
int failedNum = -1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Kinda Button Input");

  for(int inPin = 0; inPin< inputCount; inPin++){
    pinMode(inputPins[inPin], INPUT);
    Serial.print("on ");
    Serial.println(inPin);
  }
  // pinMode(35, INPUT);
  // pinMode(37, INPUT);
  pinMode(13, OUTPUT);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);

  delay(10);

  // updateAllServo();

  Serial.println("GAME SETUP");
  gameStartVisuals();

}

void loop() {
  // if (failed) return;
  

  //Read state of the pins!
  // readState();
  //uncomment when ready
  //tickAngle(0.6,speedFactor);
  //
  // delay(80);

  // readStateMoveServo();
  tickAngle(1,speedFactor);

  if(failed == true){
    while(true){
      loseConVisuals(failedNum);
    }
    // lose
  }
  


  

}

void checkFail(){

}

void updateAllServo(){
  for(int i = 0; i<5;i++){
    setServoAngle(i,angles[i]);
  }
}

void tickAngle(float speedFactor, float majorFactor[]){
  readState();
  for(int i = 0; i<5;i++){
    if (readings[i] != true){
      angles[i] += speedFactor*majorFactor[i];
      // setServoAngle(i,angles[i]);
    }else{
      angles[i] = 0.0;
      
    }

    // setServoAngle(i,angles[i]);

      if(angles[i] >= 180){
        failed = true;
        failedNum = i;
        // Serial.println("FAILED");
      }
    // angles
  }
  updateAllServo();
  // Serial.println(angles[]);

  for (int i = 0; i < 5; i++) { 
    Serial.print(angles[i]); 
    Serial.print(", "); 
  }
  Serial.println();
  
}

void setServoAngle(uint8_t channel, float angle) {
  angle = constrain(angle, 0, 180);
  int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  pwm.setPWM(channel, 0, pulse);
}


void dance(){
  setServoAngle(0,1);
  delay(500);
  setServoAngle(1,20);
  delay(100);
  setServoAngle(0,90);
  setServoAngle(1,100);
  delay(1000);
  setServoAngle(0,179);
  setServoAngle(1,179);
  delay(500);
  setServoAngle(2,90);
  delay(1000);
  setServoAngle(2,140);
  delay(1000);
  setServoAngle(0,60);
  delay(500);
}

void servoTest(int servos){
  for(int i = 0; i<servos;i++){
    for(int n = 0;n<5;n++){
      setServoAngle(i,(n*30)+1);
      delay(1000);
    }
  }
}

// void updateState

void readState(){
  String state = "";
  bool readingsTemp[] = {0,0,0,0,0};

  for(int inPin = 0; inPin< inputCount; inPin++){
    readings[inPin] = digitalRead(inputPins[inPin]);
    state += readings[inPin] ? '1' : '0';
  }
  // Serial.println(reading ? "HIGH" : "LOW"+","+reading2);
  // int readings[] = {reading ? 1 : 0, reading2 ? 1 : 0};
  // Serial.print();
  // Serial.print("  ,  ");
  // Serial.println(String(readings[0])+","+String(readings[1]));
  Serial.println(state);
}

void readStateMoveServo(){
  String state = "";
  bool readingsTemp[] = {0,0,0,0,0};

  for(int inPin = 0; inPin< inputCount; inPin++){
    readings[inPin] = digitalRead(inputPins[inPin]);
    // Serial.println(typeStr(readings[inPin])
    state += readings[inPin] ? '1' : '0';

    if(readings[inPin] == 1){
      setServoAngle(inPin,180);
      angles[inPin] = 0;
    }else{
      setServoAngle(inPin,0);
    }
  }
  // Serial.println(reading ? "HIGH" : "LOW"+","+reading2);
  // int readings[] = {reading ? 1 : 0, reading2 ? 1 : 0};
  // Serial.print();
  // Serial.print("  ,  ");
  // Serial.println(String(readings[0])+","+String(readings[1]));
  Serial.println(state);
}

void updateState(bool newReadings[]) {
  for (int i = 0; i < inputCount; i++) {
    readings[i] = newReadings[i];
  }
}

void checkSerial() {
  while (Serial.available() >= 4) {
    int angle_1   = Serial.read();  // 180
    int channel_1 = Serial.read();  // 0
    int angle_2 = Serial.read(); 
    int channel_2 = Serial.read(); 

    setServoAngle(channel_1,angle_1);
    setServoAngle(channel_2,angle_2);
  }
}


void loseConVisuals(int n){
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(40,10);             // Start at top-left corner

  for(int i=0;i<20;i++){

    display.setCursor(40,15);
    display.clearDisplay();
    display.print("YOU");
    display.display();
    setServoAngle(n,10);

    delay(600);

    display.setCursor(40,15);
    display.clearDisplay();
    display.print("LOSE");
    display.display();
    setServoAngle(n,170);

    delay(600);
  }
}

void gameStartVisuals(){
    display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(40,10);             // Start at top-left corner

  for(int i=0;i<2;i++){

    display.setCursor(40,15);
    display.clearDisplay();
    display.print("GAME");
    display.display();
    // setServoAngle(0,10);

    delay(600);

    display.setCursor(40,15);
    display.clearDisplay();
    display.print("START");
    display.display();
    // setServoAngle(0,170);

    delay(600);
  }
}
