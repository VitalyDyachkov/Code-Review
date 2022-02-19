/*6.Модуль обработки состояния 2х внешних шлейфов; 
 
	Считать, что устойчивое (истинное) состояние шлейфа должно быть не менее 0,3с. 
	Если за время одного цикла опроса (1,5с) произошло изменение состояние шлейфа отличное от Нормы, с возвратом  (или множественное событие),
	то модуль должен передать факт того, что это изменение было. 
	- Модуль должен различать следующие состояния: 
	а) Норма – напряжение измерения   (0,5 ≤ Uацп ≤  0,9)В  
	б) Шлейф 1 Считать 1    (0,9 ≤ Uацп ≤  1,2)В 
	в) Шлейф 2 Считать 2   (1,2 ≤ Uацп ≤  1,6)В 
	г) Шлейф 1 и Шлейф 2  Считать 1/2   (1,6 ≤ Uацп ≤  2)   
	д) Обрыв    (2 ≤ Uацп ≤  3,3)В 
	е) Короткое замыкание    (0 ≤ Uацп ≤  0,5)В 
	Примечание.   
	Уточненные границы переходов будут предоставлены со схемой.  
	Возможно, что бы не оказаться в неустойчивом положении на границах состояний следует организовать искусственный (программный) гистерезис.
	
	
	Норма (Alarm1&2=Closed) 1.2 V 1.166 1.229
	Считать 1 (Alarm1=Open) 1.682 V 1.644 1.718
	Считать 2 (Alarm2=Open) 1.456 V 1.42 1.491
	Считать 1&2 (Alarm1&2=Open) 1.839 V 1.801 1.876
	Обрыв (Break=Open) 3.3 V
	Короткое замыкание (Shot=Closed) 0.3 V

*/
	#include "wires.h"
	 uint32_t adc_value;
	extern ADC_HandleTypeDef hadc;
	
	void StartMeasurement(state_wires_str* state_w) 
	{
		
		HAL_StatusTypeDef state_adc;
		ON_WIRE_1
		HAL_Delay(5);
		//ToDo - произвести  измерение с пмощью АЦП_5 принять решение
		state_adc = HAL_ADC_Start(&hadc);
		HAL_Delay(2);
		if(state_adc == HAL_OK)
		{
			adc_value = HAL_ADC_GetValue(&hadc);
			HAL_ADC_Stop(&hadc);
		}
	  //HAL_ADC_Stop(&hadc);	
		//ToDo - определить значение АЦП
		//перевести в милливольты!!!
		if(adc_value >= LOW_BORDER_NORM_V && adc_value <= HIGH_BORDER_NORM_V )
		{
			state_w->state_wires = NORM;
		}
		if(adc_value >= LOW_BORDER_ALARM_1_V && adc_value <= HIGH_BORDER_ALARM_1_V)
		{
			state_w->state_wires = ALARM_1;
		}
		if(adc_value >= LOW_BORDER_ALARM_2_V && adc_value <= HIGH_BORDER_ALARM_2_V)
		{
			state_w->state_wires =   ALARM_2;
		}
		if(adc_value >= LOW_BORDER_ALARM_1_2_V && adc_value <= HIGH_BORDER_ALARM_1_2_V)
		{
			state_w->state_wires =   ALARM_1_2;
		}
		if(adc_value >= LOW_BORDER_LINE_BREAK_V && adc_value <= HIGH_BORDER_LINE_BREAK_V)
		{
			state_w->state_wires =  LINE_BREAK;
		}
		if(adc_value >= LOW_BORDER_SHORT_CIRCUIT_V && adc_value <= HIGH_BORDER_SHORT_CIRCUIT_V)
		{
			state_w->state_wires =   SHORT_CIRCUIT;
		}
		OFF_WIRE_1

	}
	