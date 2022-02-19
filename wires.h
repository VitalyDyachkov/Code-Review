
	#include "main.h"
	
	#define ON_WIRE_1	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
	#define OFF_WIRE_1   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

	#define VREF_ADC 3.0
	#define RESOLUTION_ADC 4096
	#define L_NORM_VOLTAGE    	 		1.0 
	#define H_NORM_VOLTAGE    	 		1.229
	#define L_ALARM_1_VOLTAGE    		1.4
	#define H_ALARM_1_VOLTAGE    		1.718 
	#define L_ALARM_2_VOLTAGE    		1.3
	#define H_ALARM_2_VOLTAGE    		1.491 
	#define L_ALARM_1_2_VOLTAGE    	1.6
	#define H_ALARM_1_2_VOLTAGE    	1.876 
	#define L_LINE_BREAK_VOLTAGE    2.7
	#define H_LINE_BREAK_VOLTAGE    3.310
	#define L_SHORT_CIRCUIT_VOLTAGE 0.100
	#define H_SHORT_CIRCUIT_VOLTAGE 0.390
	
	#define ONE_BIT_W (VREF_ADC / RESOLUTION_ADC)
	
	#define LOW_BORDER_NORM_V    (L_NORM_VOLTAGE / ONE_BIT_W)
	#define HIGH_BORDER_NORM_V   (H_NORM_VOLTAGE / ONE_BIT_W)

	#define LOW_BORDER_ALARM_1_V  (L_ALARM_1_VOLTAGE / ONE_BIT_W)
	#define HIGH_BORDER_ALARM_1_V (H_ALARM_1_VOLTAGE / ONE_BIT_W)

	#define LOW_BORDER_ALARM_2_V  (L_ALARM_2_VOLTAGE / ONE_BIT_W)
	#define HIGH_BORDER_ALARM_2_V (H_ALARM_2_VOLTAGE / ONE_BIT_W)

	#define LOW_BORDER_ALARM_1_2_V (L_ALARM_1_2_VOLTAGE / ONE_BIT_W)
	#define HIGH_BORDER_ALARM_1_2_V (H_ALARM_1_2_VOLTAGE / ONE_BIT_W)

	#define LOW_BORDER_LINE_BREAK_V (L_LINE_BREAK_VOLTAGE / ONE_BIT_W)
	#define HIGH_BORDER_LINE_BREAK_V (H_LINE_BREAK_VOLTAGE / ONE_BIT_W)

	#define LOW_BORDER_SHORT_CIRCUIT_V (L_SHORT_CIRCUIT_VOLTAGE / ONE_BIT_W)
	#define HIGH_BORDER_SHORT_CIRCUIT_V (H_SHORT_CIRCUIT_VOLTAGE / ONE_BIT_W)


/*!
* \brief Последовательное включение контактов с обработкой результатов на входе в АЦП1 канал 5
* \return struct[4] = NORM,ALARM_1,ALARM_2,ALARM_1_2,LINE_BREAK,SHORT_CIRCUIT
 */
	void StartMeasurement(state_wires_str* state_w);