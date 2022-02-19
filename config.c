/*9.Модуль конфигурирования (настройки) устройства. 

	Данный модуль должен принимать и сохранять в энергонезависимой памяти МК параметры настройки МВВ, включая его адрес, и предоставлять эти данные другим программным модулям.  
	Здесь же должен храниться заводской номер МВВ, который доступен только для чтения. 
	Примечание. 
	Должна быть отдельная возможность стирания/записи заводского номера. Функция доступна производителю, но не доступна пользователю. 
	
*/

#include "config.h"

void SetConfig(void)
{
	//ToDo - Запись конфигурации во внутреннюю память МК
}

void GetConfig(uint8_t settings[])
{
	//ToDo - Чтение конфигурации из внутренней памяти МК
}

void SetNumber(uint32_t my_number)
{
	//ToDo - Стирание странички перед записью
	WriteFlashData(ADDR_FABRIC_NUMBER,my_number);
	
}

uint32_t GetNumber(void)
{
	//ToDo - Чтение заводского номера в память
		uint32_t retVal = 0;
		retVal = ReadFlash(ADDR_FABRIC_NUMBER);
		return retVal;
}

uint32_t ReadFlash(uint32_t Address)
{
		uint32_t retVal = 0;
		retVal = *(__IO uint32_t*)Address;
		return retVal;
}

void WriteFlashData(uint32_t Address,uint32_t Data)
{
		HAL_FLASH_Unlock();
		HAL_FLASH_Program(FLASH_TYPEPROGRAMDATA_WORD,Address,Data);
		HAL_FLASH_Lock();
}
