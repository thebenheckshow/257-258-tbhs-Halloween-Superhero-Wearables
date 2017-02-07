//Arduino Pro Mini 3.3V

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303.h>
#define TCAADDR 0x70

#define leftArm  0
#define rightArm 1
#define leftLeg  2
#define rightLeg 3

#define kickLight  10
#define elLight    11

#define SDI  6
#define CLK  7
#define SDO  8

Adafruit_LSM303 lsm;

int leftPunch = 0;
int rightPunch = 0;
int leftKick = 0;
int rightKick = 0;

int leftPunchTimer = 0;
int rightPunchTimer = 0;
int leftKickTimer = 0;
int rightKickTimer = 0;

int lightTimer = 0;

unsigned long ticks = 0;
unsigned char dataOut[16];      //For sending commands

void setup() {

  Serial.begin(9600);
  Wire.begin();

  pinMode(kickLight, OUTPUT);
  pinMode(elLight, OUTPUT);
  pinMode(SDI, INPUT);
  pinMode(CLK, OUTPUT);
  pinMode(SDO, OUTPUT);

  for (int x = 0 ; x < 4 ; x++) {
    
    select(x);
    
    if (lsm.begin()) {
      Serial.print("Sensor ");
      Serial.print(x);
      Serial.println(" READY");
    }   
    
  }

  delay(1000);

  digitalWrite(kickLight, 1); 
  delay(100); 
  digitalWrite(kickLight, 0); 
  delay(100); 
  digitalWrite(kickLight, 1); 
  delay(100); 
  digitalWrite(kickLight, 0); 
  delay(100); 
  
  playSFX(0, 'A', 'T', 'A' + random(4), 255);
  
}

void loop() {

  //Serial.print(ticks++, DEC);
  //Serial.write(9);



  select(0);
  lsm.read();
  //Serial.print("ZERO\t");  
  //reading();
  leftPunchCheck();  

  select(1);
  lsm.read();
  //Serial.print("  ONE\t");  
  //reading();
  rightPunchCheck();

  select(2);
  lsm.read();
  //Serial.print("    TWO\t"); 
  //reading();
  //leftKickCheck();
  
  select(3);
  lsm.read();
  //Serial.print("      THREE\t");   
  //reading(); 
  rightKickCheck();  
  
  
  if (leftPunchTimer > 0 and leftPunchTimer < 30) {
    leftPunchTimer -= 1;
  }

  if (rightPunchTimer > 0 and rightPunchTimer < 30) {
    rightPunchTimer -= 1;
  }

  if (leftKickTimer > 0) {
    leftKickTimer -= 1;
  }
  
  if (rightKickTimer > 0) {
    rightKickTimer -= 1;
  }  

  if (lightTimer) {
    lightTimer -= 1;
    digitalWrite(kickLight, 1);
    digitalWrite(elLight, 1); 
  }
  else {
    digitalWrite(kickLight, 0);
    digitalWrite(elLight, 0);   
  }
  
  //Serial.println(" ");
  delay(10);
  
}

void leftPunchCheck() {
 
  if (lsm.accelData.y > 20000 and leftPunchTimer == 0) {
  
    leftPunchTimer = 30;
    
  }
  
  if (lsm.accelData.y < -20000 and leftPunchTimer == 30) {
  
    leftPunchTimer = 29;
    lightTimer = 20;
    playSFX(0, 'A', 'P', 'A' + random(3), 255);
    //Serial.println("......LEFT PUNCH!");    
    
  }
 
}

void rightPunchCheck() {
 
  if (lsm.accelData.y > 20000 and rightPunchTimer == 0) {
  
    rightPunchTimer = 30;

  }
  
  if (lsm.accelData.y < -20000 and rightPunchTimer == 30) {
  
    rightPunchTimer = 29;
    lightTimer = 20;
    playSFX(1, 'A', 'P', 'A' + random(3), 255);
    //Serial.println("..................RIGHT PUNCH!");    
    
  }
 
}

void leftKickCheck() {                //NEEDS DIALING IN!!!!!!!!!!!!!!!

  if (lsm.magData.x > 200 and leftKickTimer == 0) { 
    leftKickTimer = 50;
    lightTimer = 20;
    playSFX(2, 'A', 'K', 'A' + random(3), 255);
    //Serial.print(lsm.accelData.x, DEC);Serial.print(",");
    //Serial.print(lsm.accelData.y, DEC);Serial.print(",");
    //Serial.print(lsm.accelData.z, DEC);
    //Serial.println("..............................................LEFT KICK"); 
  } 
  
}

void rightKickCheck() {
 
  if (lsm.magData.x < -150 and lsm.magData.y > -350 and lsm.accelData.z < -200 and rightKickTimer == 0) {  //Check all 3 mags in series  
    rightKickTimer = 50;
    lightTimer = 20;
    playSFX(2, 'A', 'K', 'A' + random(3), 255);
    //Serial.println("................................RIGHT KICK");        
  }

}


void reading() {
  
  Serial.print((int)lsm.accelData.x);
  Serial.print(",");
  Serial.print((int)lsm.accelData.y);
  Serial.print(",");
  Serial.print((int)lsm.accelData.z);
  Serial.print(",");
  Serial.print((int)lsm.magData.x);
  Serial.print(",");
  Serial.print((int)lsm.magData.y);
  Serial.print(",");
  Serial.println((int)lsm.magData.z);
  
}


void select(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void playSFX(unsigned char whichChannel, unsigned char folder, unsigned char clip0, unsigned char clip1, unsigned char priority) { //0x01

	dataOut[0] = whichChannel;
	dataOut[1] = folder;	
	dataOut[2] = clip0;
	dataOut[3] = clip1;
	dataOut[4] = priority;
	
	sendData(0x01);

}

void sendData(unsigned char commandCode) {						//Sends the 16 bytes of command data to the Propeller. Bytes are sent back but we don't read them (faster)

	dataOut[15] = commandCode;									//The final byte is always the command byte

	for (int x = 0 ; x < 16 ; x++) {							//How many bytes we want to send
		for(int g = 0; g < 8; g++) {    
			digitalWrite(SDO, dataOut[x] & B00000001); 			//Assert the bit 	
			digitalWrite(CLK, 1);								//Pulse the clock
			digitalWrite(CLK, 0);								
			dataOut[x] >>= 1;									//Shift the bits to get the next bit ready. This also auto-deletes the variables!	
		}
	}

}
