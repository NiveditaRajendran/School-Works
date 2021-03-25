/* dac.c       : This code makes use of DAC Of STM32F3 to create
 *               1. Constant Voltage at PA4
 *               2. Sawtooth Waveform at PA4
 *               3. Sine Waveform at PA4
 * Submitted By: Nivedita Rajendran
 * Date        : 2018/09/27
 */

#include<stdio.h>
#include<stdint.h>

#include"common.h"
#include"math.h"

static DAC_HandleTypeDef hdac;
uint32_t startSawtooth = 0;
uint32_t startSine = 0;

#define Radian_Per_Degree              0.0174532925 

/* Ouputs a Sawtooth Waveform at PA4 using DAC
 */
int outputSaw()
{
  uint32_t j;
  HAL_StatusTypeDef val;
  
  if(startSawtooth == 1) {
    for(j = 0; j <= 4095; j++) {
      val = HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,j);
      
      if(val != HAL_OK) {
	printf("Unable to initial value on DAC "
	       "channel 1, val=%d\n",val);
	return 0;
      }
    }
  }
  return 1;
}

/* Outputs a Sine Waveform at PA4 using DAC
 */
void outputSine()
{
  uint32_t i;
  signed int temp = 0;
  uint32_t degree = 0;
  
  if(startSine == 1) {
    while(degree <= 360)
      {
	temp = 2047*sin(Radian_Per_Degree*degree);
	temp = 2048-temp;
	
	i = ((uint32_t)temp);
	HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,i);
	
	degree++;
      }
  }
}

/* converts one range of values to another range
 */
float mapFn(float x, float range1Low,
	    float range1High,float range2Low,float range2High)
{
  return (x - range1Low) * (range2High - range2Low) /
    (range1High - range1Low) + range2Low;
}

ParserReturnVal_t CmdDACInit(int action)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_DAC1_CLK_ENABLE();
  
  DAC_ChannelConfTypeDef DacConfig;
  HAL_StatusTypeDef rc;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("DACInit\n\n"
	   "This command Initialises the DAC at PA4\n"
	   );
    return CmdReturnOk;
  }
  
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
  
  hdac.Instance = DAC1;
  
  rc = HAL_DAC_Init(&hdac);
  
  if(rc != HAL_OK){
    printf("unable to initialize""DAC, rc=%d\n",rc); 
  }
  
  DacConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  DacConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  
  rc = HAL_DAC_ConfigChannel(&hdac,&DacConfig,DAC_CHANNEL_1);
  if(rc != HAL_OK) {
    printf("Unable to configure DAC "
	   "channel 1, rc=%d\n",rc);
  }
  
  __HAL_DAC_ENABLE(&hdac,DAC_CHANNEL_1);
  
  
  if(rc != HAL_OK) {
    printf("Unable to initial value on DAC "
	   "channel 1, rc=%d\n",rc);
  }
  
  return CmdReturnOk;
  
}

ADD_CMD("DACInit",CmdDACInit,"                initialise DAC at PA4")

ParserReturnVal_t CmdDACVolt(int action)
{
  
  float val;
  HAL_StatusTypeDef rc;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("DACVolts\n\n"
	   "This command takes in the Voltage that DAC needs to output.\n"
	   "input value should be between 0 - 3.3Volts\n"
	   );
    return CmdReturnOk;
  }
  
  rc = fetch_float_arg(&val);
  if (rc) {
    printf("Please Enter a Voltage between 0 - 3.3V\n");
    return CmdReturnBadParameter1;
  }
  
  if(val > 3.31) {
    printf("Enter a voltage between 0 - 3.3V \n");
    return CmdReturnBadParameter1;
  }
  
  val = ((uint32_t)mapFn(val,0,3.31,0,4095));
  
  rc = HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,val);
  
  if(rc != HAL_OK) {
    printf("Unable to initial value on DAC "
	   "channel 1, rc=%d\n",rc);
    return CmdReturnBadParameter2;
  }
  
  return CmdReturnOk;
}

ADD_CMD("DACVolt",CmdDACVolt,"<val>           DAC output for Inputs in V range")

ParserReturnVal_t CmdDACRaw(int action)
{
  
  uint32_t val;
  HAL_StatusTypeDef rc;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("DACRaw\n\n"
	   "This command takes in the Input in Raw format. "
	   "That is, the input should be between 0 - 4095\n"
	   );
    return CmdReturnOk;
  }
  
  rc = fetch_uint32_arg(&val);
  if (rc) {
    printf("Please Enter a RAW DAC value \n");
    return CmdReturnBadParameter1;
  }
  
  if(val > 4095) {
    printf("Enter a DAC value between 0 - 4095 \n");
    return CmdReturnBadParameter1;
  }
  rc = HAL_DAC_SetValue(&hdac,DAC_CHANNEL_1,DAC_ALIGN_12B_R,val);
  
  if(rc != HAL_OK) {
    printf("Unable to initial value on DAC "
	   "channel 1, rc=%d\n",rc);
    return CmdReturnBadParameter2;
  }
  
  return CmdReturnOk;
}

ADD_CMD("DACRaw",CmdDACRaw,"<val>           DAC output" 
	"for RAW DAC Input value ")

ParserReturnVal_t CmdDACSawtoothON(int action)
{
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("DACSawON\n\n"
	   "This command generates a Sawtooth waveform in the DAC output\n"
	   );
    return CmdReturnOk;
  }
  
  startSawtooth = 1;
  
  return CmdReturnOk;
}

ADD_CMD("DACSawON",CmdDACSawtoothON,"                Turns ON"
	" Sawtooth Using DAC ")

ParserReturnVal_t CmdDACSawtoothOFF(int action)
{
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("DACSawOFF\n\n"
	   "This command stops Sawtooth waveform in the DAC output\n"
	   );
    return CmdReturnOk;
  }
  
  startSawtooth = 0;  
  
  return CmdReturnOk;
}

ADD_CMD("DACSawOFF",CmdDACSawtoothOFF,"                Turns OFF"
	"Sawtooth Using DAC ")

ParserReturnVal_t CmdDACSineWaveON(int action)
{
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("CmdDACSineON\n\n"
	   " This command generates a Sine Wave at the DAC output\n"
	   );
    return CmdReturnOk;
  }
  
  startSine = 1;
  
  return CmdReturnOk;
}

ADD_CMD("DACSineON",CmdDACSineWaveON,"                Turns ON"
	" Sine wave using DAC ")

ParserReturnVal_t CmdDACSineWaveOFF(int action)
{
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("CmdDACSineOFF\n\n"
	   " This command Stops the Sine Wave at the DAC output\n"
	   );
    return CmdReturnOk;
  }
  
  startSine = 0;
  
  return CmdReturnOk;
}

ADD_CMD("DACSineOFF",CmdDACSineWaveOFF,"                Turns OFF"
	"Sine wave using DAC ")
