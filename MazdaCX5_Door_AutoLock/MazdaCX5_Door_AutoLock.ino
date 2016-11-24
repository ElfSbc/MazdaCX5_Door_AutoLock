#include <SPI.h>
#include <mcp_can.h>

#define SPI_CS_PIN			            10				// pin for CS CAN
#define CAN_SPEED			              CAN_250KBPS		// can speed settings
#define CAN_FREQ			              MCP_16MHZ		// can frequency

#define CAN_DOOR_ID			            0x43E			// can packet id for doors
#define CAN_DOOR_BYTE			          5
#define CAN_DOOR_FRONT_LEFT_BIT		  6
#define CAN_DOOR_FRONT_RIGHT_BIT	  5
#define CAN_DOOR_REAR_LEFT_BIT		  4
#define CAN_DOOR_REAR_RIGHT_BIT		  3

#define CAN_SPEED_ID			          0x202			// can packet id for Speed
#define CAN_SPEED_BYTE			        5
#define CAN_SPEED_BIT_FROM		      1
#define CAN_SPEED_BIT_TO		        2

#define PIN_TO_LOCK                 13

class Door{
	private:
		bool state = false; // closed
	public:
		bool getState(){
			return state;
		      };
		void setState(bool newState){
			state = newState;
		      };
};

class Car{
	private:		
                long unsigned int rxId;
                unsigned char len = 0;
                unsigned char rxBuf[8];
                MCP_CAN* CAN;
	public:
		int speed;
		int rpm;
		Door doorFrontLeft 	;
		Door doorFrontRight ;
		Door doorRearLeft 	;
		Door doorRearRight 	;
		
		Car ();		
		void canProcess(void);

};

Car :: Car ()
{
	Serial.begin(115200);

        MCP_CAN *CAN = new MCP_CAN(SPI_CS_PIN);                        // Set CS pin

	if(CAN->begin(MCP_ANY, CAN_SPEED, CAN_FREQ) == CAN_OK) 
		Serial.print("CAN: [OK]");  
	else
		Serial.print("CAN: [ERROR]");  
	CAN->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
	
	CAN->init_Filt(0, 0, CAN_DOOR_ID);
	CAN->init_Filt(1, 0, CAN_SPEED_ID);
	
};

void Car :: canProcess(void)
		{
			CAN->readMsgBuf(&rxId, &len, rxBuf);
			if (rxId==CAN_DOOR_ID){	
				doorFrontLeft. setState	( bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_FRONT_LEFT_BIT   ));
				doorFrontRight.setState	( bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_FRONT_RIGHT_BIT  ));
				doorRearLeft.  setState	( bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_REAR_LEFT_BIT    ));
				doorRearRight. setState	( bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_REAR_RIGHT_BIT   ));
			}
			if (rxId==CAN_SPEED_ID){
				speed = rxBuf[CAN_SPEED_BYTE]	>>	CAN_SPEED_BIT_TO	;
			}	
		};

/////////////////////////////////////////////////////
Car mazda;
  
bool wasOpened = true;  // true if one of the doors was opened
int lockPin = PIN_TO_LOCK;        // pin 5V to close doors

char msgString[150];
/////////////////////////////////////////////////////


void setup()
{

  pinMode(lockPin, OUTPUT);   
  digitalWrite(lockPin, LOW);  
}

void loop()
{
  mazda.canProcess();

  sprintf(msgString, "%d %d %d %d %d %d  0V\n", wasOpened, mazda.doorFrontLeft.getState(), mazda.doorFrontRight.getState(), mazda.doorRearLeft.getState(), mazda.doorRearRight.getState(), mazda.speed);
  Serial.print(msgString);


  wasOpened = wasOpened || mazda.doorFrontLeft.getState() || mazda.doorFrontRight.getState() || mazda.doorRearLeft.getState() || mazda.doorRearRight.getState();
  
  if (wasOpened & mazda.speed > 10){
      wasOpened = false;
      
      digitalWrite(lockPin, HIGH);
      delay(1000);
      digitalWrite(lockPin, LOW);      
      delay(1000);
      
      sprintf(msgString, "%d %d %d %d %d %d +5V\n", wasOpened, mazda.doorFrontLeft.getState(), mazda.doorFrontRight.getState(), mazda.doorRearLeft.getState(), mazda.doorRearRight.getState(), mazda.speed);
      Serial.print(msgString);

  }

}
  
