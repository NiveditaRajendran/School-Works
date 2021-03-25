/* adc.c: A/D functions.
 * Author : Nivedita Rajendran
 * Date   : 2019/09/20
 */


#include <stdio.h>
#include <stdint.h>
#include "common.h"


ADC_HandleTypeDef hadc;
int flag;
uint32_t channel;

float mapFn(float x, float range1Low,
	    float range1High,float range2Low,float range2High)
{
  return (x - range1Low) * (range2High - range2Low) /
    (range1High - range1Low) + range2Low;
}
int ADC()
{
  
  uint32_t val;
  HAL_StatusTypeDef rc;
  ADC_ChannelConfTypeDef config;
  if(flag == 1){
    /* Configure the selected channel */
    config.Channel = channel;
    config.Rank = 1;  /* Rank needs to be 1, otherwise
			 no conversion is done */
    config.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
    config.SingleDiff = ADC_SINGLE_ENDED;
    config.OffsetNumber = ADC_OFFSET_NONE;
    config.Offset = 0;
    rc = HAL_ADC_ConfigChannel(&hadc,&config);
    if(rc != HAL_OK) {
      printf(
	     "ADC channel configure failed with rc=%u\n",
	     (unsigned)rc);
 }
    /* Start the ADC peripheral */
    rc = HAL_ADC_Start(&hadc);
    if(rc != HAL_OK) {
      printf(
	     "ADC start failed with rc=%u\n",
	     (unsigned)rc);
      return 0;
    }
    
 /* Wait for end of conversion */
    rc = HAL_ADC_PollForConversion(&hadc, 100);
    if(rc != HAL_OK) {
      printf(
	     "ADC poll for conversion failed with "
	     "rc=%u\n",(unsigned)rc);
      return 0;
 }
    
    /* Read the ADC converted values */
    val = HAL_ADC_GetValue(&hadc);
 /* Stop the ADC peripheral */
    rc = HAL_ADC_Stop(&hadc);
    if(rc != HAL_OK) {
      printf("ADC stop failed with "
	     "rc=%u\n",(unsigned)rc);
      return 0;
      
    }
    
    printf("%lu ", val);
    float adcValueInVoltage = mapFn(val,0,4041,0,3.31);
    printf("%.2fV ",adcValueInVoltage);
    float i = 0;
  while(i < (adcValueInVoltage*10))
    {
      printf("*");
      i++;
    }
  printf("\n");
  }
  return 1;
}

ParserReturnVal_t CmdADCInit(int action)
{ 
  HAL_StatusTypeDef rc;
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("adcinit\n\n"
	   "This command initializes the A/D\n"
	   );
    return CmdReturnOk;
  }

  /* ADC pins configuration
     Enable the clock for the ADC GPIOs */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
 /* Configure these ADC pins in analog 
    mode using HAL_GPIO_Init() */
  GPIO_InitStruct.Pin   = GPIO_PIN_0|GPIO_PIN_2;
  GPIO_InitStruct.Mode  = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); 
  
  /* Initialize ADC */
  __HAL_RCC_ADC1_CLK_ENABLE();

  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE; 
  hadc.Init.NbrOfConversion = 1;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.NbrOfDiscConversion = 0;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = 0;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  
  rc = HAL_ADC_Init(&hadc);
  if(rc != HAL_OK) {
    printf(
	   "ADC1 initialization failed with rc=%u\n",rc);
  }
  
  return CmdReturnOk;
}

ADD_CMD("ADCInit",CmdADCInit,"                Initialize A/D")

ParserReturnVal_t CmdADCStart(int action)
{ 
  
  HAL_StatusTypeDef rc;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("adc\n\n"
	   "This command reads the A/D\n"
	   );
    return CmdReturnOk;
  }
  
  rc = fetch_uint32_arg(&channel);
  if(rc) 
    {
      printf("Must specify A/D channel\n");
      return CmdReturnBadParameter1;
    }
  
  if (channel!=6 && channel!=8)
    {
      printf("Channel should be 6 or 8\n");
      return CmdReturnBadParameter1;
    }
  flag = 1;
  
  return CmdReturnOk;
}

ADD_CMD("ADCStart",CmdADCStart,
        "channel         Read A/D")

ParserReturnVal_t CmdADCStop(int action)
{ 
  
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("adc\n\n"
	   "This command reads the A/D\n"
	   );
    return CmdReturnOk;
  }
  
  flag = 0;

  return CmdReturnOk;
}

ADD_CMD("ADCStop",CmdADCStop,
        "Stops  A/D")


