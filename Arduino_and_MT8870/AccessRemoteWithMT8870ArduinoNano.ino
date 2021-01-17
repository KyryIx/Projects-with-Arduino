//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
// System to receive command by phone using code DTMF with decoder MT8870           //
// Code for Arduino developed by Everton P. Cruz - everton.eng.br                   //
//                                                                                  //
// MT8870 - http://www.da.isy.liu.se/vanheden/pdf/mt8870.pdf                        //
// StD - Delayed Steering (Output). Presents a logic high when a received tone-pair //
//       has been registered and the output latch updated; returns to logic low     //
//       when the voltage on St/GT falls below VTSt.                                //
// Q1 ~ Q4 - Three State Data (Output). When enabled by TOE, provide the code       //
//           corresponding to the last valid tone-pair received (see Table 1).      //
//           When TOE is logic low, the data outputs are high impedance.            //
//////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////
// https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/ //
/////////////////////////////////////////////////////////////////////////////////////////////////

#define StD   2
#define Q1    6
#define Q2    5
#define Q3    4
#define Q4    3

void setup() {
  Serial.begin( 9600 ); // Initialize serial communications with the PC
  while( !Serial );     // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  pinMode( StD, INPUT );
  pinMode( Q1,  INPUT );
  pinMode( Q2,  INPUT );
  pinMode( Q3,  INPUT );
  pinMode( Q4,  INPUT );

  Serial.println( "System to read phone numbers in calls" );

  attachInterrupt( digitalPinToInterrupt(StD), getNumber, RISING ); // 0 or 1 for Uno and Nano
}

void loop() {
  // nothing //
}

void getNumber(){
  int value = 8 * digitalRead(Q4) + 4 * digitalRead(Q3) + 2 * digitalRead(Q2) + 1 * digitalRead(Q1);
  Serial.print( "The value receive is: " );
  Serial.println( value );
}
