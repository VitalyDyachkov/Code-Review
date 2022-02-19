/*2.Модуль контроля питающего напряжения;

	Модуль включается при достижении питания на входе внутреннего стабилизатора 18В.  Отключается при напряжении 15В. 
	Максимальное напряжение питания может достигать 36В. 
	Передача данных о величине питающего напряжения передаётся в контроллер Блок-Л согласно протоколу обмена.  
	Измерение напряжения проводится на накопительной ёмкости модуля (DC) через делитель с коэффициентом понижения k= 36/3.3 (уточняется по схеме). 
	При снижении напряжения ниже 18 В модуль формирует соответствующее сообщение (протокол) о низком напряжении питания.  
	При снижении напряжения близкого к напряжению отключения модуль должен перевести выходное реле в состояние, соответствующее начальному, указанному при настройке. 
	Пороговую величину напряжения следует выбрать такой, что бы в случае пропадания питания модуль успел перевести реле в исходное состояние.  
	Примечание.  
	Возможно,  потребуется алгоритм отслеживающий скорость изменения напряжения, на основе которой будет принято решение,
	что произойдёт отключение модуля и нужно реле вернуть в исходное состояние. 
	При проведении измерений следует усреднять данные АЦП в пределах времени цикла опроса – не более 1,5с 
	
*/

	#include "voltage_control.h"
	
extern ADC_HandleTypeDef hadc;	

bool CheckVoltage(void)
{
		bool retValue = true;
		uint32_t adc_value;
		HAL_StatusTypeDef state_adc;
		//ToDo - произвести  измерение с пмощью АЦП_5 принять решение
		state_adc = HAL_ADC_Start(&hadc);
		if(state_adc == HAL_OK)
		{
			adc_value = HAL_ADC_GetValue(&hadc);	
			HAL_ADC_Stop(&hadc);
		}
		else
		{
			retValue = false;
		}

	  if(adc_value != 1)
		{
			retValue = false;
		}

		return retValue;
		
}
	
	