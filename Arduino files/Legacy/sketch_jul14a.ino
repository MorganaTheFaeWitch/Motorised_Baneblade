//Libs
#include <ArduinoBLE.h>
// #include <BLEGamepadClient.h>

//assigning pins
int directionPin_L = 12;
int pwmPin_L = 3;
int brakePin_L = 9;
int directionPin_R = 13;
int pwmPin_R = 11;
int brakePin_R = 8;

bool directionState;

// bluetooth services // characteristics
BLEService TankControll("6feb780f-bf34-44d0-82ab-35166f1d26ae");
BLEUnsignedIntCharacteristic LeftTrack("L100", BLERead | BLEWrite);
BLEUnsignedIntCharacteristic RightTrack("R100", BLERead | BLEWrite);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial);

  // begin ble initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  // set the local name peripheral advertises
  BLE.setLocalName("BaneBlade");
  // set the UUID for the service this peripheral advertises:
  BLE.setAdvertisedService(TankControll);
  TankControll.addCharacteristic(LeftTrack);
  TankControll.addCharacteristic(RightTrack);
  // LeftTrack.addDescriptor("Controlls the left tank track, 0 = break, -1|-10 reverse, 1|10 forward");
  // RightTrack.addDescriptor("Controlls the right tank track, 0 = break, -1|-10 reverse, 1|10 forward");
  // adding the service to the BLe class
  BLE.addService(TankControll);
  // setting the values
  LeftTrack.writeValue(0);
  RightTrack.writeValue(0);

  // defining event handlers
  BLE.setEventHandler(BLEConnected, ConnectionHandler);
  BLE.setEventHandler(BLEDisconnected, DisconnectionHandler);
  RightTrack.setEventHandler(BLEWritten, RightMotorHandler);
  LeftTrack.setEventHandler(BLEWritten, LeftMotorHandler);

  // defining controll pins.
  pinMode(directionPin_L, OUTPUT);
  pinMode(pwmPin_L, OUTPUT);
  pinMode(brakePin_L, OUTPUT);
  pinMode(directionPin_R, OUTPUT);
  pinMode(pwmPin_R, OUTPUT);
  pinMode(brakePin_R, OUTPUT);

  // advertising the BLE.
  BLE.advertise();
  Serial.println("Device setup, waiting for connection");
}

//uses some of the same names as the global variables, bad practice, however obvious naming helps
void MotorAssign(int motorvalue, int dirpin, int pwmpin, int breakpin){
  // 1 forward, 2 reverse
  //relesing breaks
  if(motorvalue!=0){
    digitalWrite(breakpin, LOW);
      // forward
    if(motorvalue < 100){
      digitalWrite(dirpin, LOW);
    }
    // backward
    else{
      digitalWrite(dirpin, HIGH);
    }
    analogWrite(pwmpin, motorvalue % 100);
  }

  
  // break
  else{
    analogWrite(pwmpin, 0);
    digitalWrite(breakpin, HIGH);
  }
}

void LeftMotorHandler(BLEDevice central, BLECharacteristic characteristic){
  // unused parameters
  (void)central;
  (void)characteristic;
  Serial.print("Left track event: ");
  Serial.println(LeftTrack.value());
  MotorAssign(LeftTrack.value(), directionPin_L, pwmPin_L, brakePin_L);
}
void RightMotorHandler(BLEDevice central, BLECharacteristic characteristic){
  // unused parameters
  (void)central;
  (void)characteristic;
  Serial.print("Right track event: ");
  Serial.println(RightTrack.value());
  MotorAssign(RightTrack.value(), directionPin_R, pwmPin_R, brakePin_R);
}


void ConnectionHandler(BLEDevice central){
  Serial.println("Connected to: ");
  Serial.println(central.localName());
  digitalWrite(LED_BUILTIN, HIGH);
}
void DisconnectionHandler(BLEDevice central){
  Serial.println("Disconnected from: ");
  Serial.println(central.localName());
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  BLE.poll();
}
