// This Program makes use of an accelerometer to control the brightness of an RGB LED.
// Each axis of the accelerometer controls each colour. I2C communication was used to Communicate with the sensor.

#include <stdio.h>
#include <stdint.h>

#include "common.h"


I2C_HandleTypeDef hi2c1;
uint8_t i2cbuffer[8];
int16_t gx,gy,gz;
int Xgyro,Ygyro,Zgyro;
int32_t flag = 0;

void i2cinit(void) {
pwminit();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_I2C1_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;

  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 4;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  HAL_I2C_Init(&hi2c1); 
  
i2cbuffer[0] = 0x00; 
HAL_I2C_Mem_Write(&hi2c1, 0xd2,107,I2C_MEMADD_SIZE_8BIT,i2cbuffer,1,10); // brings the device out of sleep mode.
  
}

void findaddress(void) {
 // int32_t i = 0;

//  while( i < 255) {

    if(HAL_I2C_IsDeviceReady(&hi2c1,0xd2,1,10) == HAL_OK) {
printf("Device ready\n");
//      break;
//}
//i++;
}
else {
printf("Device not ready \n");
}
}

void i2cgyro(void) {
 // printf("MPUADDRESS %lx \n", MPU6050);
 

 if(flag == 1) {

 i2cbuffer[0] = 0x00; // configuration value for gyroscope to set range as +/- 250
  HAL_I2C_Mem_Write(&hi2c1,0xd2,28,I2C_MEMADD_SIZE_8BIT,i2cbuffer,1,10);
  
  i2cbuffer[0] = 0x00;
  HAL_I2C_Mem_Read(&hi2c1, 0xd2,59,I2C_MEMADD_SIZE_8BIT,i2cbuffer,6,10); // This is the WHO_AM_I  register that contains the address of the device.
 // printf("%d \n", i2cbuffer[0]);
  gx = (i2cbuffer[0]<<8 | i2cbuffer[1]);
  gy = (i2cbuffer[2]<<8 | i2cbuffer[3]);
  gz = (i2cbuffer[4]<<8 | i2cbuffer[5]);
  HAL_Delay(500);S


  Xgyro = mapFn(gx,-10000,10000,0,1000);
 Ygyro = mapFn(gy,-10000,10000,0,1000);
 Zgyro = mapFn(gz,-10000,10000,0,1000);
if(Xgyro < 0) {
Xgyro = 0;
}
if(Ygyro < 0) {
Ygyro = 0;
}if(Zgyro < 0) {
Zgyro = 0;
}if(Xgyro > 1000) {
Xgyro = 1000;
}if(Ygyro > 1000) {
Ygyro = 1000;
}if(Zgyro > 1000) {
Zgyro = 1000;
}
pwm(1,Xgyro);
pwm(2,Ygyro);
pwm(3,Zgyro);
  printf("Xaxis %d\n",Xgyro);
  printf("Yaxis %d\n",Ygyro);
  printf("Zaxis %d\n",Zgyro);
  printf("Xaxis %d\n",gx);
  printf("Yaxis %d\n",gy);
  printf("Zaxis %d\n",gz);

}
}
ParserReturnVal_t CMDI2Cinit(int action) {
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("i2cinit\n\n"
	   "This command initialise the I2C\n"
	   );

    return CmdReturnOk;
  }

  i2cinit();
  
  return CmdReturnOk;
}

ADD_CMD("i2cinit",CMDI2Cinit,"                Initialise the I2C")


ParserReturnVal_t CMDI2CAddress(int action) {
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("i2caddress\n\n"
	   "This command finds the address of I2C Bus\n"
	   );

    return CmdReturnOk;
  }

  findaddress();
  
  return CmdReturnOk;
}

ADD_CMD("i2caddress",CMDI2CAddress,"                returns the address of I2C")

 

ParserReturnVal_t CMDI2C(int action) {
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("i2c\n\n"
	   "This command finds the address of I2C Bus\n"
	   );

    return CmdReturnOk;
  }

  //i2cgyro();
  fetch_int32_arg(&flag);
 

  return CmdReturnOk;
}

ADD_CMD("i2c",CMDI2C,"                returns the address of I2C")

 
