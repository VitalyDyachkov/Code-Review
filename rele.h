
	#include "main.h"
	#include "stdbool.h"
	
	#define IMP_START_ON_RELE	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	#define IMP_STOP_ON_RELE    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	#define IMP_START_OFF_RELE	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	#define IMP_STOP_OFF_RELE   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); 
	#define STATE_RELE HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
/*!
* \brief Включение реле
* \return Контроль обратной связи
 */
bool ReleOn(void);
/*!
 * \brief Выключение реле
  * \return Контроль обратной связи 
 */
bool ReleOff(void);
