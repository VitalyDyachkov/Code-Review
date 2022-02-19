

#include "main.h"

#define ADDR_FABRIC_NUMBER 0x08003F80
uint32_t config_word[4] __attribute__((at(ADDR_FABRIC_NUMBER)));
//uint32_t* PTR_AFN = (uint32_t*)ADDR_FABRIC_NUMBER;

/*!
* \brief Установка и запись конфигурации во флеш
* \return none
 */
void SetConfig(void);

/*!
* \brief Запрос конфигурации
* \return none
 */
void GetConfig(uint8_t settings[]);

/*!
* \brief Запись заводского номера
* \return none
 */
void SetNumber(uint32_t my_number);

/*!
* \brief Чтение заводского номера
* \return Заводской номер
 */
uint32_t GetNumber(void);

/*!
* \brief Чтение данных из флеш памяти
* \return Значение по адресу
 */
uint32_t ReadFlash(uint32_t Address);

/*!
* \brief Запись данных в память флеш
* \return none
 */
void WriteFlashData(uint32_t Address,uint32_t Data);
