/* closedloop.c : motor functions 
 * submitted by : Nivedita Rajendran
 * Date         : Nov 29, 2018
 */


#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include<ctype.h>
#include<string.h>

uint16_t previousEncoderValue;
volatile int16_t currentSpeed;
volatile int32_t Kp;
volatile int32_t Setpoint;
volatile int32_t errorValue;
volatile uint32_t ClosedLoopMode;

void closedLoopHandler(void)
{
  uint16_t encoder;
  uint32_t difference;
  static uint32_t countDown = 100;

  if(countDown != 0){
    countDown--;
    return;
  }
  else {
    countDown = 100;
  }

  encoder = encoderread(1);

  /*safe substract ..take care of roll over*/
  difference = ((uint32_t )encoder - (uint32_t )previousEncoderValue);

  if(difference > 65536){
    difference = difference - 65536;
  }
  previousEncoderValue = encoder;

  currentSpeed = difference;
  /*if the control loop is on close it*/
  if(ClosedLoopMode){
    errorValue = currentSpeed - Setpoint;
    motor(0,errorValue * Kp);
  }
}

ParserReturnVal_t Cmdspeed(int action) {
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("speed\n\n"
	   "This command initialise the speed of DCmotor\n"
	   );
    
    return CmdReturnOk;
  }
  printf("currentspeed: %d\n",currentSpeed);
  printf("Current setpoint: %ld\n", Setpoint);
  printf("Current error: %ld\n", errorValue);
  printf("Current ClosedLoopMode: %lu\n",ClosedLoopMode );
  printf("Kp: %ld\n",Kp);

  return CmdReturnOk;
}

ADD_CMD("speed",Cmdspeed,"               show motor speed")

ParserReturnVal_t Cmdloop(int action) {
  uint32_t closedLoopMode;


  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("speed\n\n"
	   "This command initialise the speed of DCmotor\n"
	   );
    
    return CmdReturnOk;
  }
  closedLoopMode = 1;
  fetch_uint32_arg(&closedLoopMode);
  ClosedLoopMode = closedLoopMode;
  return CmdReturnOk;
}

ADD_CMD("loop",Cmdloop,"[0/1]             Enable closed loop mode")

ParserReturnVal_t CmdKp(int action) {

  int32_t kp;
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("Kp\n\n"
	   "This command set Kp value\n"
	   );
    
    return CmdReturnOk;
  }
  kp = 0;
  fetch_int32_arg(&kp);
  Kp = kp;

  return CmdReturnOk;
}

ADD_CMD("Kp",CmdKp,"Kp            set Kp value for loop")


ParserReturnVal_t CmdsetSpeed(int action) {

  int32_t setpoint;
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("setspeed\n\n"
	   "This command setpoint value\n"
	   );
    
    return CmdReturnOk;
  }
  setpoint = 0;
  fetch_int32_arg(&setpoint);
  Setpoint = setpoint;

  return CmdReturnOk;
}

ADD_CMD("setspeed",CmdsetSpeed,"<setpoint>            setpoint value for loop")



