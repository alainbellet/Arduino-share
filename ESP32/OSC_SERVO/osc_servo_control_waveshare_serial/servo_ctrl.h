#include <SCServo.h>
#include <math.h>


// === SC Servo ===
#define CTRL_SC_SERVO
SCSCL sc;

int MAX_ID = 20;
// the uart used to control servos.
// GPIO 18 - S_RXD, GPIO 19 - S_TXD, as default.
#define S_RXD 18
#define S_TXD 19

float ServoDigitalRange = 1023.0;
float ServoAngleRange   = 210.0;
float ServoDigitalMiddle= 511.0;
#define ServoInitACC      0
#define ServoMaxSpeed     1500
#define MaxSpeed_X        1500
#define ServoInitSpeed    1500
int SERVO_TYPE_SELECT = 2;
int MAX_MIN_OFFSET = 30;
#define SMS_STS_ID SCSCL_ID


// set the servo ID list.
byte ID_List[253];
bool Torque_List[253];

// []: the ID of the servo.
// the buffer of the information read from the active servo.
s16  loadRead[253];
s16  speedRead[253];
byte voltageRead[253];
int  currentRead[253];
s16  posRead[253];
s16  modeRead[253];
s16  temperRead[253];

// []: the num of the active servo.
// use listID[activeNumInList] to get the ID of the active servo.
byte listID[253];
byte searchNum = 0;
bool searchedStatus = false;
bool searchFinished = false;
bool searchCmd      = false;
byte activeNumInList = 0;
s16 activeServoSpeed = 100;
byte servotoSet = 0;

// linkageBuffer to save the angle.
float linkageBuffer[50];

// the buffer of the bytes read from USB-C and servos. 
int usbRead;
int stsRead;


void getFeedBack(byte servoID){
  if(sc.FeedBack(servoID)!=-1){
    posRead[servoID] = sc.ReadPos(-1);
    speedRead[servoID] = sc.ReadSpeed(-1);
    loadRead[servoID] = sc.ReadLoad(-1);
    voltageRead[servoID] = sc.ReadVoltage(-1);
    currentRead[servoID] = sc.ReadCurrent(-1);
    temperRead[servoID] = sc.ReadTemper(-1);
    modeRead[servoID] = sc.ReadMode(servoID);
  }else{
    // Serial.println("FeedBack err");
  }
}


void servoInit(){
  Serial1.begin(1000000, SERIAL_8N1, S_RXD, S_TXD);
  sc.pSerial = &Serial1;
  while(!Serial1) {}

  for (int i = 0; i < MAX_ID; i++){
    Torque_List[i] = true;
  }
}


void setMiddle(byte InputID){
  sc.CalibrationOfs(InputID);
}


void setMode(byte InputID, byte InputMode){
  sc.unLockEprom(InputID);
  if(InputMode == 0){
    if(SERVO_TYPE_SELECT == 1){
      sc.writeWord(InputID, 11, 4095);
      sc.writeByte(InputID, SMS_STS_MODE, InputMode);
    }
    else if(SERVO_TYPE_SELECT == 2){
      sc.writeWord(InputID, SCSCL_MIN_ANGLE_LIMIT_L, 20);
      // st.writeWord(InputID, SCSCL_MIN_ANGLE_LIMIT_H, 5123);
      sc.writeWord(InputID, SCSCL_MAX_ANGLE_LIMIT_L, 1003);
      // st.writeWord(InputID, SCSCL_MAX_ANGLE_LIMIT_H, 60240);
    }
  }

  else if(InputMode == 3){
    if(SERVO_TYPE_SELECT == 1){
      sc.writeByte(InputID, SMS_STS_MODE, InputMode);
      sc.writeWord(InputID, 11, 0);
    }
    else if(SERVO_TYPE_SELECT == 2){
      sc.writeWord(InputID, SCSCL_MIN_ANGLE_LIMIT_L, 0);
      // st.writeWord(InputID, SCSCL_MIN_ANGLE_LIMIT_H, 0);
      sc.writeWord(InputID, SCSCL_MAX_ANGLE_LIMIT_L, 0);
      // st.writeWord(InputID, SCSCL_MAX_ANGLE_LIMIT_H, 0);
    }
  }
  sc.LockEprom(InputID);
}


void setID(byte ID_select, byte ID_set){
  if(ID_set > MAX_ID){MAX_ID = ID_set;}
  sc.unLockEprom(ID_select);
  sc.writeByte(ID_select, SMS_STS_ID, ID_set);
  sc.LockEprom(ID_set);
}


void servoStop(byte servoID){
  sc.EnableTorque(servoID, 0);
  delay(10);
  sc.EnableTorque(servoID, 1);
}


void servoTorque(byte servoID, u8 enableCMD){
  sc.EnableTorque(servoID, enableCMD);
}

// select the ID of active servo.
void activeID(int cmdInput){
  activeNumInList += cmdInput;
  if(activeNumInList >= searchNum){
    activeNumInList = 0;
  }
  else if(activeNumInList < 0){
    activeNumInList = searchNum;
  }
}


void activeSpeed(int cmdInput){
  activeServoSpeed += cmdInput;
  if (activeServoSpeed > ServoMaxSpeed){
    activeServoSpeed = ServoMaxSpeed;
  }
  else if(activeServoSpeed < 0){
    activeServoSpeed = 0;
  }
}


int rangeCtrl(int rawInput, int minInput, int maxInput){
  if(rawInput > maxInput){
    return maxInput;
  }
  else if(rawInput < minInput){
    return minInput;
  }
  else{
    return rawInput;
  }
}