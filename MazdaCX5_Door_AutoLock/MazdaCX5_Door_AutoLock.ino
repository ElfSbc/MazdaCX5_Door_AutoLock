// d
#include <SPI.h>
#include <mcp_can.h>

#define SPI_CS_PIN			            10				// pin for CS CAN
#define CAN_SPEED			              CAN_1000KBPS		// can speed settings
#define CAN_FREQ			              MCP_16MHZ		// can frequency

#define CAN_DOOR_ID			            0x43E			// can packet id for doors
#define CAN_DOOR_BYTE			          4
#define CAN_DOOR_FRONT_LEFT_BIT		  5
#define CAN_DOOR_FRONT_RIGHT_BIT	  4
#define CAN_DOOR_REAR_LEFT_BIT		  3
#define CAN_DOOR_REAR_RIGHT_BIT		  2

#define CAN_SPEED_ID			          0x217			// can packet id for Speed
#define CAN_SPEED_BYTE_1			       4
#define CAN_SPEED_BYTE_2             5


#define PIN_TO_LOCK                 8



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
              char msgString2[150];
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
		void canProcess(MCP_CAN *CAN);

};

Car :: Car ()
{
//  MCP_CAN *CAN = new MCP_CAN(SPI_CS_PIN);                        // Set CS pin

//  if(CAN->begin(MCP_ANY, CAN_SPEED, CAN_FREQ) == CAN_OK) 
//    Serial.print("CAN: [OK]");  
//  else
//    Serial.print("CAN: [ERROR]");  

//    CAN->begin(MCP_ANY, CAN_SPEED, CAN_FREQ);
//  CAN->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
  
  //CAN->init_Filt(0, 0, CAN_DOOR_ID);
  //CAN->init_Filt(1, 0, CAN_SPEED_ID);
};

void Car :: canProcess(MCP_CAN *CAN)
		{

			CAN->readMsgBuf(&rxId, &len, rxBuf);
			if (rxId==CAN_DOOR_ID){	
				doorFrontLeft. setState	( bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_FRONT_LEFT_BIT   ));
				doorFrontRight.setState	( bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_FRONT_RIGHT_BIT  ));
				doorRearLeft.  setState	( bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_REAR_LEFT_BIT    ));
				doorRearRight. setState	( bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_REAR_RIGHT_BIT   ));
        sprintf(msgString2, "- %d %d %d %d | %d", 
              bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_FRONT_LEFT_BIT   ), 
              bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_FRONT_RIGHT_BIT  ), 
              bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_REAR_LEFT_BIT    ), 
              bitRead ( rxBuf[CAN_DOOR_BYTE], CAN_DOOR_REAR_RIGHT_BIT   ),
              rxBuf[3]);
      //  Serial.println(msgString2);
                
			}
			if (rxId==CAN_SPEED_ID){
				speed = rxBuf[CAN_SPEED_BYTE_1]*256+rxBuf[CAN_SPEED_BYTE_2];
//               sprintf(msgString2, "- speed: %d", speed/100);
//               Serial.println(msgString2);
			}	
		};

/////////////////////////////////////////////////////
Car mazda;

MCP_CAN CAN0(SPI_CS_PIN);                          // Set CS to pin 10
  
bool wasOpened = true;  // true if one of the doors was opened

char msgString[150];
/////////////////////////////////////////////////////


void setup()
{
  Serial.begin(115200);
  Serial.println("=======================OK========================\n");
  if(CAN0.begin(MCP_ANY, CAN_SPEED, CAN_FREQ) == CAN_OK) 
    Serial.print("CAN: [OK]\n");  
  else
    Serial.print("CAN: [ERROR]\n");  
  CAN0.setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
  
//  CAN0.init_Filt(0, 0, CAN_DOOR_ID);

  pinMode(PIN_TO_LOCK, OUTPUT);   
  digitalWrite(PIN_TO_LOCK, LOW);
}

void loop()
{
  mazda.canProcess(&CAN0);

  wasOpened = wasOpened || mazda.doorFrontLeft.getState() || mazda.doorFrontRight.getState() || mazda.doorRearLeft.getState() || mazda.doorRearRight.getState();
  
  if (wasOpened & mazda.speed/100>20 &!mazda.doorFrontLeft.getState() & !mazda.doorFrontRight.getState() & !mazda.doorRearLeft.getState() & !mazda.doorRearRight.getState()){

      sprintf(msgString, "+ %d %d %d %d | %d", mazda.doorFrontLeft.getState(), mazda.doorFrontRight.getState(), mazda.doorRearLeft.getState(), mazda.doorRearRight.getState(), mazda.speed/100);
   Serial.println(msgString);
   
      tone(PIN_TO_LOCK, 1000,100);
      delay(100);              // wait for a second
      noTone(PIN_TO_LOCK);
      wasOpened = false;      
  //delay(100);              // wait for a second
//      digitalWrite(PIN_TO_LOCK, HIGH);
//      delay(1000);
//      digitalWrite(PIN_TO_LOCK, LOW);      
//      delay(1000);
      


  }

  

}
  
