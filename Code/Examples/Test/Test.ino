#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303.h>

#define INDICATOR_LED 12
#define DATA_ACCEL 0
#define DATA_MAG 1
#define DEADZONE_ACCEL 1000

Adafruit_LSM303 lsm;

float accelData[6] = {};
float magData[6] = {};

void setup(){
  #ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  #endif
  pinMode(INDICATOR_LED, OUTPUT);
  Serial.begin(9600);

  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin()){
    Serial.println("Oops ... unable to initialize the LSM303. Check your wiring!");
    while (1){
    }
  }
}

void dataUpdate_New();
void dataUpdate_Old();
void dataPrint(int dataToPrint);

void loop(){
  digitalWrite(INDICATOR_LED, HIGH);

  dataUpdate_New();
//   for (int count = 0; count <= 3; count++){
//     //     if (accelData[0] != accelData[3]){
//     //       dataPrint(DATA_ACCEL);
//     //     }
//   }
  if (accelData[0] < accelData[3] - DEADZONE_ACCEL || accelData[0] > accelData[3] + DEADZONE_ACCEL){
    dataPrint(DATA_ACCEL);
//     Serial.print("\tAccel new:\tX: "); Serial.print((int)accelData[0]); Serial.print("\n");
//     Serial.print("\tAccel old:\tX: "); Serial.print((int)accelData[3]); Serial.print("\n\n");
  }
  if (accelData[1] < accelData[4] - DEADZONE_ACCEL || accelData[1] > accelData[4] + DEADZONE_ACCEL){
    dataPrint(DATA_ACCEL);
//     Serial.print("\tAccel new:\tY: "); Serial.print((int)accelData[1]); Serial.print("\n");
//     Serial.print("\tAccel old:\tY: "); Serial.print((int)accelData[4]); Serial.print("\n\n");
  }
  if (accelData[2] < accelData[5] - DEADZONE_ACCEL || accelData[2] > accelData[5] + DEADZONE_ACCEL){
    dataPrint(DATA_ACCEL);
//     Serial.print("\tAccel new:\tZ: "); Serial.print((int)accelData[2]); Serial.print("\n");
//     Serial.print("\tAccel old:\tZ: "); Serial.print((int)accelData[5]); Serial.print("\n\n");
  }

  //   dataPrint(DATA_ACCEL);
  //   dataPrint(DATA_MAG);


//   delay(1000);
  dataUpdate_Old();

}

void dataPrint(int dataToPrint){
  if (dataToPrint == DATA_ACCEL){
    Serial.print("\tAccel new:\tX: "); Serial.print((int)accelData[0]); Serial.print(" ");
    Serial.print("Y: ");           Serial.print((int)accelData[1]); Serial.print(" ");
    Serial.print("Z: ");           Serial.print((int)accelData[2]); Serial.print("\n");
    Serial.print("\tAccel old:\tX: "); Serial.print((int)accelData[3]); Serial.print(" ");
    Serial.print("Y: ");           Serial.print((int)accelData[4]); Serial.print(" ");
    Serial.print("Z: ");           Serial.print((int)accelData[5]); Serial.print("\n\n");
  }
  if (dataToPrint == DATA_MAG){
    Serial.print("\tMag:\tX: ");   Serial.print((int)magData[0]);   Serial.print(" ");
    Serial.print("Y: ");           Serial.print((int)magData[1]);   Serial.print(" ");
    Serial.print("Z: ");           Serial.print((int)magData[2]);   Serial.print("\n");
  }
}

void dataUpdate_New(){
  lsm.read();
  accelData[0] = lsm.accelData.x;
  accelData[1] = lsm.accelData.y;
  accelData[2] = lsm.accelData.z;
  magData[0] = lsm.magData.x;
  magData[1] = lsm.magData.y;
  magData[2] = lsm.magData.z;
}

void dataUpdate_Old(){
  accelData[3] = accelData[0];
  accelData[4] = accelData[1];
  accelData[5] = accelData[2];
  magData[3] =  magData[0];
  magData[4] =  magData[1];
  magData[5] =  magData[2];
}