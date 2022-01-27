
#include "func_variables.h"

unsigned char CNMIN[2];

void Stop_avt2(int schet);
void Avto(unsigned int schet, float F_LH,float F_HL,float M_LH,float M_HL,  float F_Lmid,  float F_Hmid,  float M_Lmid,  float M_Hmid );
void Avto_N(void);
void Ruch_N(void);
void OnRelay(void);
void OffRelay(void);
char spr_buffer[7];
float temp_mod;
long global_freq;

bool flag_stop_driv_cn = false;
bool flag_stop_driv_cc = false;
bool flag_gate_state_driver = false;

char state_driver_cn = 0;
char state_driver_cc = 0;

void Avto(unsigned int schet, float F_LH,float F_HL,float M_LH,float M_HL,  float F_Lmid,  float F_Hmid,  float M_Lmid,  float M_Hmid )
{

counter = 0;
	flag_stop_driv_cn = false;
    flag_stop_driv_cc = false;
do
{

	readFM();
	delay_ms(5);
	
	#asm ("cli")
	temp_mod = integer_module;//12.2567;
	sprintf(spr_buffer,"%.f",temp_mod);

	buff_debug[0] = 11;   
	buff_debug[1] = spr_buffer[0];
	buff_debug[2] = spr_buffer[1];
	buff_debug[3] = spr_buffer[2];
	buff_debug[4] = spr_buffer[3];
	buff_debug[5] = spr_buffer[4];
	buff_debug[6] = spr_buffer[5];
	buff_debug[7] = spr_buffer[6];   
          send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG); 
          
          temp_mod = integer_fase;//12.2567;
	sprintf(spr_buffer,"%.f",integer_fase);

	buff_debug[0] = 12;   
	buff_debug[1] = spr_buffer[0];
	buff_debug[2] = spr_buffer[1];
	buff_debug[3] = spr_buffer[2];
	buff_debug[4] = spr_buffer[3];
	buff_debug[5] = spr_buffer[4];
	buff_debug[6] = spr_buffer[5];
	buff_debug[7] = spr_buffer[6];   
          send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
	#asm ("sei")
//if((integer_module > M_HL) || integer_module > 10)  // Модуль > 6 Уменьшаем Ссв
	if(integer_module > M_HL)//  || integer_fase < -5)
	{
		
		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x04;
		
		if (TEMP2 == 0x04) // Если Ссв работает на уменьшение, останавливаем
		{
			// Стоп Ссв
			send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
			//  delay_us(500);
			delay_ms(1);
			state_auto_tuning &= 0xfb;
		}
		
		//send_can(CAN_MSG_GET_STATE_MOTOR,SEND_TO_MOTOR_CC);// Состояние Ссв
		//flag_gate_state_driver = true;
		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x08;
			
		if (TEMP2 != 0x08) // Если Ссв не работает на увеличение
		//if (state_driver_cc == STATE_DRIVER_STOP) // Если Ссв не работает на увеличение
		{
			buff_debug[0] = DEBUG_CC_MOVE_DOWN; 
			send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
			ZICL_CV++;

			can_mes[0]=0x00;          //целевая позиция
			can_mes[1]=0x00;			
			can_mes[2]=0x00;
			can_mes[3]=0x00;
			can_mes[4]=0x00;          //режим старта
			 
			send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)

			delay_ms(1);
			state_auto_tuning|=0x08;
		}
		else{
			buff_debug[0] = DEBUG_CC_DOWN; 
			send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
			
		}
	}
	
	readFM();
	delay_ms(5);
	
	//if((integer_module < 10 || integer_module < M_LH)) // Модуль < -6 Увеличиваем Ссв
	if(integer_module < M_LH)
	{		

		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x08;
		if (TEMP2 == 0x08) // Если Ссв работает на увеличение, останавливаем
		{
		// Стоп Ссв
		send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
		//delay_us(500);
		delay_ms(1);
		state_auto_tuning &= 0xf7;
		}
		
		//send_can(CAN_MSG_GET_STATE_MOTOR,SEND_TO_MOTOR_CC);// Состояние Ссв
		//flag_gate_state_driver = true;
		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x04;
		
		if (TEMP2 != 0x04) // Если Ссв не работает на уменьшение
		//if (state_driver_cc == STATE_DRIVER_STOP) // Если Ссв не работает на уменьшение
		{
			buff_debug[0] = DEBUG_CC_MOVE_UP;
			send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
			ZICL_CV++;
			
			can_mes[0]=0x00;          //целевая позиция
			can_mes[1] = 0x1a ;
			can_mes[2] = 0x0a;
			can_mes[3]=0x00;
			can_mes[4]=0x00;          //режим старта

			send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
			delay_ms(1);
			state_auto_tuning|=0x04;
		}
		else{
			buff_debug[0] = DEBUG_CC_UP; 
			send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);			
		}
	}
	
	readFM();
	delay_ms(5);
	
	//if((integer_fase > F_HL) && integer_fase > 0) )// Фаза > 25 Уменьшаем Сн
	if(integer_fase > F_HL)
	{		

		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x20;
		
		if (TEMP2 == 0x20) // Если Сн работает на увеличение, останавливаем
		{
			// Стоп Ссв
			send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
			// delay_us(500);
			delay_ms(1);
			state_auto_tuning &= 0xdf;
		}
		
		//send_can(CAN_MSG_GET_STATE_MOTOR,SEND_TO_MOTOR_CN);// Состояние Ссв
		//flag_gate_state_driver = true;
		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x10;
		
		if (TEMP2 != 0x10) // Если Сн не работает на уменьшение
		//if (state_driver_cn == STATE_DRIVER_STOP) // Если Сн не работает на уменьшение
		{
			buff_debug[0] = DEBUG_CN_MOVE_DOWN;
			send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
			ZICL_CN++;
			can_mes[0]=0x00;          //целевая позиция
			can_mes[1]=0x00;
			can_mes[2]=0x00;
			can_mes[3]=0x00;
			can_mes[4]=0x00;          //режим старта
			send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
			delay_ms(1);
			state_auto_tuning|=0x10;
		}
		else{
			buff_debug[0] = DEBUG_CN_DOWN; 
			send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);			
		}
	}

	readFM();
	delay_ms(5);
	//if((integer_fase < 0 && integer_fase < F_LH)) // Фаза < -25 Увеличиваем Сн
	if(integer_fase < F_LH)// || integer_module < 8)
	{	

		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x10;
		
		if (TEMP2 == 0x10) // Если Сн работает на уменьшение, останавливаем
		{
			// Стоп Ссв
			send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
			 //delay_us(500);
			delay_ms(1);
			state_auto_tuning &= 0xef;
		}
		
		//send_can(CAN_MSG_GET_STATE_MOTOR,SEND_TO_MOTOR_CN);// Состояние Ссв
		//flag_gate_state_driver = true;
		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x20;
		
		if (TEMP2 != 0x20) // Если Сн не работает на увеличение
		//if (state_driver_cn == STATE_DRIVER_STOP) // Если Сн не работает на увеличение
		{
			buff_debug[0] = DEBUG_CN_MOVE_UP;
			send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
			ZICL_CN++;
			can_mes[0]=0x00;          //целевая позиция
			can_mes[1] = 0xff;
			can_mes[2] = 0x03;
			can_mes[3]=0x00;
			can_mes[4]=0x00;          //режим старта
											  //0 - немедленно переместить вал в указанную абсолютную позицию
											  //1 - немедленно переместить вал в указанную относительную позицию
			send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
			delay_ms(1);
			state_auto_tuning|=0x20;
		}
		else{
			buff_debug[0] = DEBUG_CN_UP; 
			send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);			
		}
	}

	readFM();
	delay_ms(5);
	// Останавливаем двигатели
	//if(integer_fase < F_Lmid || integer_fase > F_Hmid) // Фаза -14...14 останавливаем Сн
	if(integer_fase < F_Lmid && integer_fase > F_Hmid) // Фаза -14...14 останавливаем Сн
	{
		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x30;
		buff_debug[0] = DEBUG_STOP_CN;
		send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
		
		if (TEMP2 != 0x00) // Если Сн работает, останавливаем
		{
			send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
			delay_ms(1);
			state_auto_tuning &= 0xcf;
		}
		
		STOPCN = 1; //выставили флаг остановки
	} 
	else 
	{ 
		STOPCN = 0;
	}

	readFM();
	delay_ms(1);
	
	if(integer_module < M_Lmid && integer_module > M_Hmid) // Модуль -5...5 останавливаем Ссв
	{
		TEMP2 = state_auto_tuning;
		TEMP2 &= 0x0c;
		buff_debug[0] = DEBUG_STOP_CCV;
		send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
	
		if (TEMP2 != 0x00) // Если Ссв работает , останавливаем
		{
			// Стоп Ссв
			send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
			delay_ms(2);
			state_auto_tuning &= 0xf3;
		}
		
		STOPCV = 1; //выставили флаг остановки
	}
	else 
	{
		STOPCV = 0;
	}

	if(STOPCV == 1 && STOPCN == 1)
	{
		STOPCV = 0;
		STOPCN = 0;
		stop_stop();
	}
	
	Stop_avt(schet);

	if(ZICL_CN >= cnt_loop_tuning_cn || ZICL_CV >= cnt_loop_tuning_ccv)
	{
			  FLAG_Z =1;
	}
}
while (((integer_fase > F_HL || integer_fase < F_LH) || (integer_module > M_HL || integer_module < M_LH)) && 
       (!flag_stop_driv_cn && !flag_stop_driv_cc) &&  
	   (FLAG_Z != 1) && (StateSystem & STATE_AUTO_TUN));

	FLAG_Z = 0;
	ZICL_CN =0;
	ZICL_CV=0;
	TEMP1=SOST;
	TEMP1&=0x20;
	flag_stop_driv_cn = false;
    flag_stop_driv_cc = false;
	
	if (TEMP1 == 0x20)
	{
		buff_debug[0] = DEBUG_STOP_ALL_DRIV;
		send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
		// Останавливаем двигатели
		send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
		delay_ms(2);
		send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
		delay_ms(2);
		state_auto_tuning &= 0xcf;state_auto_tuning &= 0xf3;
	}

}

void Avto_N(void)
{	
		  char start_position_dr_1 = 0x39;
		  char start_position_dr_2 = 0x30;
	send_can(CAN_MSG_START_AT,SEND_TO_PMU);
	OnRelay();
	delay_ms(20);
	/*переменные для таймера*/
	t1 = 0;
	t2 = 0;
	
	time = 0;
	time_flag = 0;
SOST |= 0x20;  // Идет настройка
delay_ms(50);
/*****режим таррировки*****/
delay_ms(50);
state_auto_tuning = 0x00;
counter = 0;
TEMP2 = 0x00;
fault = 0x00;
/******выбор дискретов из таблицы градуировки******/
GetSetDiscrets();
/*****************Автонастройка ************************/
/*читаем частоту и устанавливаем ограничения*/


/* if (global_freq >= 10000000 && global_freq < 19000000)
 {
   CNMAX[0]=0xe8;       // Частота > 10 MHz
   CNMAX[1]=0x01;
   CSMAX[0]=0x0d;
   CSMAX[1]=0x05;
 }
else if (global_freq >= 19000000)  //&& FQ < 230)
 {
  CNMAX[0]=0xE0;       // Частота < 10 MHz
  CNMAX[1]=0x01;
  CSMAX[0]=0xa8;
  CSMAX[1]=0x03;                 
 }
else
{
  CNMAX[0]=0xd0;       // Частота < 10 MHz
  CNMAX[1]=0x03;
  CSMAX[0]=0x1a;
  CSMAX[1]=0x0a;
} */

                           /*читаем частоту и устанавливаем ограничения*/						
                           if (global_freq >= 10000000&& global_freq <19000000)
                               {
                                 CNMAX[0]=0xe8;       // Частота > 10 MHz
                                 CNMAX[1]=0x01;//5.5
                                 CSMAX[0]=0x0d;
                                 CSMAX[1]=0x05;//6			
								 CNMIN[0]=0x0f;       // Частота > 10 MHz
                                 CNMIN[1]=0x00;//0
                               }
                          else if (global_freq >= 19000000)// && FQ < 230)
                               {
                                CNMAX[0]=0xE0;       // Частота < 10 MHz
                                CNMAX[1]=0x01;//5
								start_position_dr_1 = 0x35;
                                CSMAX[0]=0xa9;
                                CSMAX[1]=0x03;//4        
								start_position_dr_2 = 0x30;
								
							     CNMIN[0]=0x0f;       // Частота > 10 MHz
                                 CNMIN[1]=0x00;//0
                               }
						   else if(global_freq >= 3000000 && global_freq <= 9900000)
                              {
                                CNMAX[0]=0xd0;       // Частота < 10 MHz
                                CNMAX[1]=0x03;//9.9
                                CSMAX[0]=0x1a;
                                CSMAX[1]=0x0a;//9.9
								
								 CNMIN[0]=0x0f;       // Частота > 10 MHz
                                 CNMIN[1]=0x00;//0
                              }
							  else
							  {
	                             CNMAX[0]=0xd0;       // Частота < 10 MHz
                                CNMAX[1]=0x03;//9.9
                                CSMAX[0]=0x1a;
                                CSMAX[1]=0x0a;//9.9
								
								 CNMIN[0]=0xd1;       // Частота > 10 MHz
                                 CNMIN[1]=0x02;//0							  
							  }
							  
state_auto_tuning &= 0x7f;
TEMP2 = 0x00;
SOST2 &= 0xfe; //СУ исправно
while (TEMP2 != 0x80 && counter != 3)
   {
   send_can(CAN_MSG_GET_MODULE,SEND_TO_BAU);// Состояние БФСК

    delay_ms(100); // Ждем ответа
    TEMP2 = state_auto_tuning;
    TEMP2 &= 0x80;
    counter++;
   }

if ((TEMP2 != 0x80 && BUFFCAN[1] != 0x30) && fault == 0x00)
{
 SOST&=0xdf;  // нет настройки
 //fault = 0x02;   // Неисправно БФСК
 SOST2 |= 0x01; //СУ неисправно
}
state_auto_tuning &= 0x7f;
//проверка КШД1,2

TEMP2 = 0x00;

while (TEMP2 != 0x80 && counter != 3)
   {
   send_can(CAN_MSG_GET_STATE_MOTOR,SEND_TO_MOTOR_CC);// Состояние Ссв

    delay_ms(10); // Ждем ответа
    TEMP2 = state_auto_tuning;
    TEMP2 &= 0x80;
    counter++;
   }

if ((TEMP2 != 0x80 && BUFFCAN[0] != 0x00) && fault == 0x00)
{
 SOST&=0xdf;  // нет настройки
 //fault = 0x03;   // Неисправно Ссв
 SOST2 |= 0x01; //СУ неисправно

}
state_auto_tuning &= 0x7f;

TEMP2 = 0x00;

while (TEMP2 != 0x80 && counter != 3)
   {
   send_can(CAN_MSG_GET_STATE_MOTOR,SEND_TO_MOTOR_CN);// Состояние Сн

    delay_ms(10); // Ждем ответа
    TEMP2 = state_auto_tuning;
    TEMP2 &= 0x80;
    counter++;
   }

if ((TEMP2 != 0x80 && BUFFCAN[0] != 0x00) && fault == 0x00)
{
 SOST&=0xdf;  // нет настройки
 //fault = 0x04;   // Неисправно Сн
 SOST2 |= 0x01; //СУ неисправно
}
state_auto_tuning &= 0x7f;
counter = 0;

//----------------------------------- грубая настройка -----------------------------------------------------------
delay_ms(200);
readFM();
delay_ms(2);

if (!(((integer_fase <= 1.0 && integer_fase >= -1.0) && (integer_module <= 11.5 && integer_module >= 10.5)) && fault == 0x00)) 
{
	if(global_freq <= 3000000)
{
		/*Cn ->max*/
		/*Csv->min*/
 	                            speed(400,1);
                                speed(400,2); 
 	buff_debug[0] = DEBUG_FIRST_SET_POSITION_17;
	send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG); 
 
	//Down Cn
							can_mes[0] = 0x00;//целевая позиция
							can_mes[1] = 0xd0;
							can_mes[2] = 0x03;
							can_mes[3] = 0x00;
							can_mes[4] = 0x00;	//режим старта
//0 - немедленно переместить вал в указанную абсолютную позицию
//1 - немедленно переместить вал в указанную относительную позицию                               
							send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CN);
                            delay_ms(2);

    // Up Ссв

							can_mes[0] = 0x00;	//целевая позиция
//---------------------- конец грубой настройки ------------------------------------------------------
if(global_freq <= 1700000)
{
							can_mes[1] = 0x1c;//1C
							can_mes[2] = 0x02;//02							
}							
else
{	
							can_mes[1] = 0xd5;
							can_mes[2] = 0x01;
}
							can_mes[3] = 0x00;
							can_mes[4] = 0x00;	//режим старта
                                                                //0 - немедленно переместить вал в указанную абсолютную позицию
                                                                //1 - немедленно переместить вал в указанную относительную позицию
							send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CC);
                            delay_ms(2);	

                            CT_POZDV=0x0001;
                            CT_POZDV1=0x0002;
                            CT_POZDV2=0x0003;
                            CT_POZDV3=0x0004;

                            counter = 0;

                            do
                            {
                                Stop_avt2(100);
                            }                               
							while ((CT_POZDV != CT_POZDV2 || CT_POZDV1 != CT_POZDV3)&& 
							      (!((integer_fase < 1.0 && integer_fase > -1.0)&&
								  (integer_module < 11.5 && integer_module > 10.5))));
							readFM();

                           delay_ms(2);
						   
                                CT_POZDV=0x0001;
                                CT_POZDV1=0x0002;
                                CT_POZDV2=0x0003;
                                CT_POZDV3=0x0004;


                                counter = 0;

                                TEMP1=SOST;
                                TEMP1&=0x20;
                                if (TEMP1 == 0x20)
                                   {
									  // Останавливаем двигатели
									  send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
									  delay_ms(2);
									  send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
									  delay_ms(2);
                                   }							   
						   }
else
{							   
/*set zero position*/
if (((POZD1 != start_position_dr_1 )|| (POZD2 != start_position_dr_2))&& 
   (!((integer_fase < 1.0 && integer_fase > -1.0)&&
   ( integer_module < 11.5 && integer_module > 10.5)))&& (fault == 0))	
{
 	buff_debug[0] = DEBUG_FIRST_SET_POSITION_30;
    send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);  
	/*Cn ->max*/
	/*Csv->min*/
 	                        speed(400,1);
                            speed(400,2);  
                            delay_ms(20); 

							can_mes[0] = 0x00;//целевая позиция	       
							can_mes[1] = CNMAX[0];//0xd0      // Частота < 10 MHz                                
							can_mes[2] = CNMAX[1];//0x03
							can_mes[3] = 0x00;
							can_mes[4] = 0x00;	//режим старта
                                                                //0 - немедленно переместить вал в указанную абсолютную позицию
                                                                //1 - немедленно переместить вал в указанную относительную позицию

							send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CN);
							//уменьшаем Ссв 

							can_mes[0] = 0x00;	//целевая позиция
							can_mes[1] = 0x0f;
							can_mes[2] = 0x00;
							can_mes[3] = 0x00;
							can_mes[4] = 0x00;	//режим старта
                                                                //0 - немедленно переместить вал в указанную абсолютную позицию
                                                                //1 - немедленно переместить вал в указанную относительную позицию

							send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CC);
                            delay_ms(2);

                            CT_POZDV=0x0001;
                            CT_POZDV1=0x0002;
                            CT_POZDV2=0x0003;
                            CT_POZDV3=0x0004;

                            counter = 0;
							
                            do
                            {                                  
								do
                                {    
								Stop_avt2(100);                           
                                }
								while(CT_POZDV != CT_POZDV2 || CT_POZDV1 != CT_POZDV3);	
                            }                               
							while ((CT_POZDV != CT_POZDV2 || CT_POZDV1 != CT_POZDV3)&&
							(!((integer_fase < 1.0 && integer_fase > -1.0)&&
							(integer_module < 11.5 && integer_module > 10.5))));
							
							counter = 0;
                            TEMP1=SOST;
                            TEMP1&=0x20;
                            
							if (TEMP1 == 0x20)
                            {
							// Останавливаем двигатели
								send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
								delay_ms(2);
								send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
								delay_ms(2);
                            }	
}
}
                           #asm("wdr")  /*сбрасываем watchdog*/
						   delay_ms(200);
						   readFM();					   
/*1-я грубая настройка*/
if (( integer_module < 11.0 )||( integer_fase < 1.0 ) &&!((integer_fase < 1.0 && integer_fase > -1.0)&&( integer_module < 11.5 && integer_module > 10.5))&& (fault == 0))
{
 buff_debug[0] = DEBUG_ROUGH_1;
send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);  
	/*Cn ->max*/
	/*Csv->min*/
 	                speed(400,1);
                    speed(400,2); 
                    //увеличиваем Сн
                    can_mes[0] = 0x00;//целевая позиция
	                can_mes[1] = CNMAX[0];
	                can_mes[2] = CNMAX[1];
	                can_mes[3] = 0x00;
	                can_mes[4] = 0x00;	//режим старта
                                                                //0 - немедленно переместить вал в указанную абсолютную позицию
                                                                //1 - немедленно переместить вал в указанную относительную позицию
					send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CN);
                    delay_ms(2);
                    //уменьшаем Ссв
                    can_mes[0]=0x00;	//целевая позиция
	                can_mes[1]=0x0f;
	                can_mes[2]=0x00;
	                can_mes[3]=0x00;
	                can_mes[4]=0x00;	//режим старта
                                                                //0 - немедленно переместить вал в указанную абсолютную позицию
                                                                //1 - немедленно переместить вал в указанную относительную позицию
                    send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CC);
                    delay_ms(2);

                    CT_POZDV=0x0001;
                    CT_POZDV1=0x0002;
                    CT_POZDV2=0x0003;
                    CT_POZDV3=0x0004;

                    counter = 0;

                    do
                    {
                        Stop_avt2(100);
                    }
					while ((CT_POZDV != CT_POZDV2 || CT_POZDV1 != CT_POZDV3) && 
							((integer_fase < 0.6) || 
							(integer_module < 11)) && 
							(!((integer_fase <= 1.0 && integer_fase >= -1.0)||
							(integer_module <= 11.5 && integer_module >= 10.5))));

                                CT_POZDV=0x0001;
                                CT_POZDV1=0x0002;
                                CT_POZDV2=0x0003;
                                CT_POZDV3=0x0004;


                                counter = 0;

                                TEMP1=SOST;
                                TEMP1&=0x20;
                                if (TEMP1 == 0x20)
                                   {
									  // Останавливаем двигатели
									  send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
									  delay_ms(2);
									  send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
									  delay_ms(2);
                                   }
}
							delay_ms(200);
							readFM();
							delay_ms(2);
/*second tunining*/
	if ((( integer_module < 10.7 )||( integer_module < 10.8 )) || 
	     (integer_fase > 0.0) && (!((integer_fase < 1.0 && integer_fase > -1.0) && 
		 (integer_module < 11.5 && integer_module > 10.5))) && 
		  (fault == 0 ))	
{
		buff_debug[0] = DEBUG_ROUGH_2;
send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);  
	/*Csv->max*/	
	if(global_freq <= 3000000)
	{
		                        speed(150,1);
                                speed(60,2);		
	}
	else
	{
		                        speed(150,1);
                                speed(150,2);		
	}
 
	                                counter = 0;

                                CT_POZDV=0x0001;
                                CT_POZDV1=0x0002;
                                CT_POZDV2=0x0003;
                                CT_POZDV3=0x0004;


 /*                                if (FQ > 100)  // Частота > 10 MHz
                                   { */
                                    // Уменьшаем Сн
                        can_mes[0]=0x00;	//целевая позиция
	                    can_mes[1]=CNMIN[0];
	                    can_mes[2]=CNMIN[1];
	                    can_mes[3]=0x00;
	                    can_mes[4]=0x00;	//режим старта
                                                                //0 - немедленно переместить вал в указанную абсолютную позицию
                                                                //1 - немедленно переместить вал в указанную относительную позицию

                                    //send_can(0x05,0x02); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
									send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CN);

if(global_freq <= 1700000)
{
                                    // Up Ссв
                            can_mes[0] = 0x00;	//целевая позиция
							can_mes[1] = CSMAX[0];
							can_mes[2] = CSMAX[1];
							can_mes[3] = 0x00;
							can_mes[4] = 0x00;	//режим старта
                                                                //0 - немедленно переместить вал в указанную абсолютную позицию
                                                                //1 - немедленно переместить вал в указанную относительную позицию

                                    //send_can(0x05,0x01); 	
									send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CC);
}
else
{									   
                                    // Down Ссв
                            can_mes[0] = 0x00;	//целевая позиция
							can_mes[1] = 0x0f;
							can_mes[2] = 0x00;
							can_mes[3] = 0x00;
							can_mes[4] = 0x00;	//режим старта
                                                                //0 - немедленно переместить вал в указанную абсолютную позицию
                                                                //1 - немедленно переместить вал в указанную относительную позицию
							//send_can(0x05,0x01); 
							send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CC);							
}
if(global_freq <= 1700000)
{
									   do {
										Stop_avt2(100);
									}						  
									while((CT_POZDV != CT_POZDV2 || CT_POZDV1 != CT_POZDV3) && (!(integer_module <= 11.2 && integer_module >= 10.7)));// &&  F < 0);
}
else{                                   
								   do {
										Stop_avt2(100);
									}						  
									while((CT_POZDV != CT_POZDV2 || CT_POZDV1 != CT_POZDV3) && 
									     ((( integer_module < 10.7 )||( integer_module < 11.3 )) || (integer_fase > 0.0)) &&
									     (!((integer_fase <= 1.0 && integer_fase >= -1.0) && (integer_module <= 11.5 && integer_module >= 10.5))));
}									 
									 
                                TEMP1=SOST;
                                TEMP1&=0x20;
                                if (TEMP1 == 0x20)
                                   {
									  // Останавливаем двигатели
									  send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
									  delay_ms(2);
									  send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
									  delay_ms(2);
                                   }
							
}
//---------------------- точная настройка ------------------------------------------------------------

delay_ms(200);
cnt_loop_tuning_cn = 4;
cnt_loop_tuning_ccv = 4;
readFM();
if (((integer_fase > 1.0 || integer_fase < -1.0) || (integer_module < 10.5 || integer_module  > 11.5)) && fault == 0x00) // если фаза не равна -5...+5 и модуль не равен -7...+7 положительна
{
 //установка конфигурации 1
 speed(100, 1);
 speed(70, 2);

 CT_POZDV=0x0001;
 CT_POZDV1=0x0002;
 CT_POZDV2=0x0003;
 CT_POZDV3=0x0004;      
readFM();
//(schet , F_LH, F_HL, M_LH, M_HL, F_Lmid, F_Hmid, M_Lmid, M_Hmid )
//Avto(500, 0xe6, 0x19, 0xf7, 0x07, 0x0d, 0xf2, 0x07, 0xf7); //-25<F<25, -8<M<8

 	buff_debug[0] = DEBUG_ACCURATE_1;
send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG); 

Avto(25, -5.0,5.0,7.0,13.0,3.0,-3.0,13.0,7.0);


  readFM();

  delay_ms(2);
counter = 0;
//---------------------- конец точной настройки ------------------------------------------------------
//---------------------- 2-я точная настройка ------------------------------------------------------
 delay_ms(100);

//                           if (FQ < 40 && FQ >= 15 ){
/*  speed(70, 1);
 speed(50, 2); */
  speed(50, 1);
 speed(30, 2);
//                                } else {
//                                speed(100, 1);  //120
//                                speed(60, 2);   //70
//                                }
 cnt_loop_tuning_cn = 5;
 cnt_loop_tuning_ccv = 5;

 CT_POZDV=0x0001;
 CT_POZDV1=0x0002;
 CT_POZDV2=0x0003;
 CT_POZDV3=0x0004;

 counter=0;

 state_auto_tuning=0x00;
 //if (time_flag == 0){
                                                  readFM();

//Avto(500, 0xf5, 0x0a, 0xfa, 0x05, 0x0a, 0xf5, 0x05, 0xfa); //-10<F<10, -5<M<5

	buff_debug[0] = DEBUG_ACCURATE_2;
send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);

  Avto(25, -5.0,5.0,9.0,12.0,3.0,-3.0,12.0,9.0); 
//Avto(25, -5.0,5.0,9.0,11.0,3.0,-3.0,11.0,9.0); 
  
  //Avto(500, -7.0,7.0,9.0,11.0,7.0,-7.0,11.0,9.0);

  readFM();

  delay_ms(2);
                                                  counter = 0;
  //}
//---------------------- конец 2-й точной настройки ------------------------------------------------------
//---------------------- 3-яя точная настройка ------------------------------------------------------
 delay_ms(100);

//                               if (FQ < 40 && FQ >= 15 ){
 speed(50, 1);
 speed(20, 2);
//                                } else {
//                                 speed(70, 1);
//                                 speed(30, 2);
//                             }
 cnt_loop_tuning_cn = 7;
 cnt_loop_tuning_ccv = 7;
//  if (FQ <= 40){cnt_loop_tuning_cn = 5;  cnt_loop_tuning_ccv = 5;}
 CT_POZDV=0x0001;
 CT_POZDV1=0x0002;
 CT_POZDV2=0x0003;
 CT_POZDV3=0x0004;

 counter=0;      // частота

 state_auto_tuning=0x00;
 //if (time_flag == 0){
readFM();

//Avto(500, 0xfa, 0x05, 0xfb, 0x04, 0x05, 0xfa, 0x04, 0xfb); //-5<F<5, -4<M<4

	buff_debug[0] = DEBUG_ACCURATE_3;
send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);

Avto(25,-5.0,5.0,9.0,12.0,5.0,-5.0,12.0,9.0); 
//Avto(25,-5.0,5.0,9.0,11.0,5.0,-5.0,11.0,9.0); 


                                                  readFM();

  delay_ms(2);
                                                  counter = 0;
 //  }

//---------------------- конец 3-й точной настройки ------------------------------------------------------
//---------------------- 4-ая точная настройка ------------------------------------------------------
delay_ms(100);
 cnt_loop_tuning_cn = 8;
 cnt_loop_tuning_ccv = 8;
 CT_POZDV=0x0001;
 CT_POZDV1=0x0002;
 CT_POZDV2=0x0003;
 CT_POZDV3=0x0004;
 counter=0;      // частота
 state_auto_tuning=0x00;

          readFM();

//Avto(500, 0xfa, 0x05, 0xfb, 0x04, 0x05, 0xfa, 0x04, 0xfb); //-5<F<5, -4<M<4

 	buff_debug[0] = DEBUG_ACCURATE_4;
send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG); 

Avto(25,-5.0,5.0,9.0,12.1,5.0,-5.0,12.1,9.0); 
//Avto(25,-5.0,5.0,9.0,11.3,5.0,-5.0,11.3,9.0); 


//Avto(25,-5.0,5.0,9.2,10.8,5.0,-5.0,10.8,9.2); 

  readFM();

  delay_ms(2);
counter = 0;
//}
//---------------------- 5-ая точная настройка ------------------------------------------------------
 delay_ms(100);

	speed(10, 1);
	speed(5, 2); 
 cnt_loop_tuning_cn = 7;
 cnt_loop_tuning_ccv = 7;
 POZDV[0]=0x0001;
 POZDV[1]=0x0002;
 POZDV[2]=0x0003;
 POZDV[3]=0x0004;
 counter = 0;      // частота
 state_auto_tuning=0x00;
 readFM();
 //Avto(1000, 0xfc, 0x03, 0xfc, 0x03, 0x03, 0xfc, 0x03, 0xfc); //-3<F<3, -3<M<3

	buff_debug[0] = DEBUG_ACCURATE_5;
send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);

  Avto(25, -0.2,0.2,10.5,11.5,0.2,-0.2,11.5,10.5); //-3<F<3, -3<M<3
  //Avto(25, -0.2,0.2,10.9,11.1,0.2,-0.2,11.1,10.9); //-3<F<3, -3<M<3
 
 
 // Avto(25, -0.5,0.5,9.8,10.2,0.5,-0.5,10.2,9.8); //-3<F<3, -3<M<3 
//}
  // Останавливаем двигатели
  send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
  delay_ms(2);
  send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
  delay_ms(2);
}
                   delay_ms(200);
 //}
state_auto_tuning=0x00;
readFM();

delay_ms(2);
counter = 0;
SOST &= 0xdf;

//--------------------------Конец настройки--------------------------------------------------------
	delay_ms(200);
	readFM();
	delay_ms(100);
	readFM();
	delay_ms(100);
	flag2 = 1; //настройка произведена
//признак удачной настройки

		//if (((integer_fase <= 1.0 && integer_fase >= -1.0) && (integer_module <= 11.5 && integer_module >= 10.5)) && fault == 0x00)  
             if (((integer_fase <= MAX_FASE && integer_fase >= MIN_FASE) && 
             (integer_module <= MAX_MODUL && integer_module >= MIN_MODUL)) && fault == 0x00)
		 {				 

			  state_auto_tuning = 0x00;     // Автоматическая настройка готов
			  SOST &= 0xef;
			  SOST2 |= 0x02;
			   can_mes[0] = 0x30;
			  send_can(CAN_MSG_RESULT_TUNING, SEND_TO_PMU);
			   if ((SOST & 0x02) == 0){ //ДУ
			  //отправка сообщения ПДУ
			  putchar1(RS485_ANSWER_TUN_READY);
			  putchar1(END_MSG);
			  }  

			  Write_Discret();
		  }
else {
          flag_set_discrets = false;
          PORTC &= 0xf7; //вкл обход СУ
          SOST |= 0x10;

          state_auto_tuning = 0x00;    // Автоматическая настройка неисправно
          TEMP1 = SOST;
          TEMP1 &= 0x20;



          if (fault == 0x02)
          {

                  delay_ms(2);
                  can_mes[0] = 0x32;  //Неисправно БФСК
                  send_can(CAN_MSG_ERROR, SEND_TO_PMU);
                   SOST2 |= 0x01; // СУ неисправно

          }

          if (fault == 0x03)
          {
                  delay_ms(2);
                  can_mes[0] = 0x33;  //Неисправно КШД1
                  send_can(CAN_MSG_ERROR, SEND_TO_PMU);

          }

          if (fault == 0x04)
          {
                  delay_ms(2);
                  can_mes[0] = 0x34;  //Неисправно КШД2
                  send_can(CAN_MSG_ERROR, SEND_TO_PMU);
          }

          if (fault == 0x00){
                  can_mes[0] = 0x31;
                  send_can(CAN_MSG_RESULT_TUNING, SEND_TO_PMU);
                  delay_ms(2);

                  if ((SOST & 0x02) == 0){ //ДУ
				  
                  putchar1(RS485_ANSWER_TUN_NOTREADY);
                  putchar1(END_MSG);
				  
                  }
          } else if (fault == 0x01){
                  can_mes[0] = 0x31;
				  
                  send_can(CAN_MSG_RESULT_TUNING, SEND_TO_PMU);
				  
                  delay_ms(2);

           if ((SOST & 0x02) == 0){ //ДУ
                  putchar1(RS485_ANSWER_TUN_NOTREADY);
                  putchar1(END_MSG);
                  }

                  }
          }
}
else
{
	
			  state_auto_tuning = 0x00;     // Автоматическая настройка готов
			  can_mes[0] = 0x30;			
			  
			  send_can(CAN_MSG_RESULT_TUNING, SEND_TO_PMU);
			  
			   if ((SOST & 0x02) == 0){ //ДУ
			  //отправка сообщения ПДУ
			  putchar1(RS485_ANSWER_TUN_READY);
			  putchar1(END_MSG);
			  }  

			  Write_Discret();
}
//send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
	// PORTC&=0xfe;        // Выключили ВАР
	delay_ms(5);
	OFF_RN3;
    OFF_RN4;
	//PORTC&=0xFC; // Выключили РН3, РН4
	//PORTC&=0xfd; // Выключили РН
	SOST&=0xdf;  // Конец настройки
	delay_ms(2);
	speed(100, 1);
	speed(100, 2);
	
}

void Ruch_N(void)
{
	int j = 0;
	unsigned int state_system = 0;
	OnRelay();
	/******выбор дискретов из таблицы градуировки******/
          GetSetDiscrets();
	state_system = CheckLink();	
	counter = 0;
          speed(100, 1);
          speed(100, 2);    
          send_can(CAN_MSG_START_MT,SEND_TO_PMU);
          while ((StateSystem & STATE_MANUAL_TUN) && !(state_error))
	{
		if (counter >= 5)
		{    
			counter = 0;
			readFM();
			delay_ms(2);
		}

		delay_ms(50);
		counter++;

		if(flag_get_msg_can) //получено сообщение
		{
			flag_get_msg_can = false;
			switch (BUFFCAN[0])
			{
				case INPUT_CAN_MSG_MOTOR_CC_TO_RIGHT: // Связь >
				{
					if(BUFFCAN[1] == 0x03)
					{
                        can_mes[0]=0x00;	//целевая позиция
                        can_mes[1]=0x1a;
                        can_mes[2]=0x0a;
                        can_mes[3]=0x00;
                        can_mes[4]=0x00;	//режим старта
                        send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
                        delay_ms(4);
                        send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_PMU); 
					}
					break;
				}
				case INPUT_CAN_MSG_MOTOR_CC_TO_LEFT: // Связь <
				{
				        if(BUFFCAN[1] == 0x03)
						{		   
				            can_mes[0]=0x00;	
					        can_mes[1]=0x0f;
					        can_mes[2]=0x00;
					        can_mes[3]=0x00;
					        can_mes[4]=0x00;	
					        send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					        delay_ms(4);    
                            send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_PMU); 
				        }
				        break;
				}
				case INPUT_CAN_MSG_MOTOR_CN_TO_RIGHT: // Настройка >
				{
				        if(BUFFCAN[1] == 0x03)
						{
							can_mes[0]=0x00;	//целевая позиция
					        can_mes[1]=0xff;
                            can_mes[2]=0x03;
					        can_mes[3]=0x00;
					        can_mes[4]=0x00;	
					        send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					        delay_ms(4);
                            send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_PMU); 
			            }
				        break;
				}
				case INPUT_CAN_MSG_MOTOR_CN_TO_LEFT: // Настройка <
				{
						if(BUFFCAN[1] == 0x03)
						{					
							 can_mes[0]=0x00;	//целевая позиция
							 can_mes[1]=0x0f;
							 can_mes[2]=0x00;
							 can_mes[3]=0x00;
							 can_mes[4]=0x00;	
							 send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
								   delay_ms(4);
							send_can(CAN_MSG_MOVE_MOTOR,SEND_TO_PMU); 	
													   //delay_ms(4);
							  }
				          break;
				}                                                      
				case INPUT_CAN_MSG_START_CALIBRATION: //Таррировка
				{
					if(BUFFCAN[1] == 0x03)
					{
                                                            StateSystem |= STATE_CALIBRATION;						 
                                                            OFF_RN4;
						delay_ms(100);
						send_can(CAN_MSG_START_CALIBRATION,SEND_TO_PMU);
						delay_ms(100); 
                                                            
                                                            while(StateSystem & STATE_CALIBRATION)
                                                            {
                                                                      readFM();             //отправляем значения частоты и фазы каждые 200 мс		
                                                                      delay_ms(200);					
                                                            }  
                                                            send_can(CAN_MSG_STOP_CALIBRATION,SEND_TO_PMU);		

					}								  
					break;															
				}
				default:
				break;
			
			}
			for ( j=0; j<8; j++) BUFFCAN[j]= 0; 
		}
		readFM();
		//send_can(CAN_MSG_DEBUG,SEND_TO_DEBUG);
	}


	if(fault == 0x02)
	{
		can_mes[0] =0x32;
		send_can(CAN_MSG_ERROR,SEND_TO_PMU); //Неисправно БФСК
	}
	if(fault == 0x03)
	{
		can_mes[0] =0x33;
		send_can(CAN_MSG_ERROR,SEND_TO_PMU); //Неисправно Ссв
	}
	if(fault == 0x04)
	{
		can_mes[0] =0x34;
		send_can(CAN_MSG_ERROR,SEND_TO_PMU); //Неисправно Сн
	}

	readFM();
        			  
//          if (((integer_fase <= MAX_FASE && integer_fase >= MIN_FASE) && 
//             (integer_module <= MAX_MODUL && integer_module >= MIN_MODUL)) && fault == 0x00)	
//	{   
		Write_Discret();
		can_mes[0] = 0x30;
		send_can(CAN_MSG_RESULT_TUNING, SEND_TO_PMU);
                    StateSystem |= STATE_READY;
                    StateSystem &= ~STATE_BYPASS;
                    
//	} 
//	else 
//	{
//		can_mes[0] = 0x31;
//		send_can(CAN_MSG_RESULT_TUNING, SEND_TO_PMU); 
//                    BYPASS;                   
//                    StateSystem |= STATE_BYPASS;
//                    StateSystem &= ~STATE_READY;
//	}
         
          OFF_RN3;
          OFF_RN4;
	delay_ms(10);
}

void Stop_avt2(int schet)
{
counter++;
if (counter >= (schet*5))
{
#asm("wdr")  /*сбрасываем watchdog*/
//stop_stop();
readFM();
//										if((integer_fase < 1.0 && integer_fase > -1.0)&&( integer_module < 11.5 && integer_module > 10.5))
//										{
//											// Останавливаем двигатели
//											send_can(0x07,0x02); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
//											delay_ms(2);
//											send_can(0x07,0x01); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
//											delay_ms(2);											
//										}
counter = 0;

send_can(CAN_MSG_READ_POSITION,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)


//Принимаем сообщение от КШД2
//Прием ответа на команды инд 180

// wait until reception is complete
while (TEMP1 != 0x80){
TEMP1 = state_auto_tuning;
TEMP1 &= 0x80;
}
// while((state_auto_tuning&= 0x80) != 0x80);
//delay_ms(1000);
state_auto_tuning &=0x7F;

CT_POZDV = POZDV[1];
CT_POZDV = (CT_POZDV<<8);
CT_POZDV |= POZDV[2];


//чтение позиции двигателя 2
delay_ms(2);
send_can(CAN_MSG_READ_POSITION,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)

// wait until reception is complete
//while((state_auto_tuning&= 0x80) != 0x80);
while (TEMP1 != 0x80){
TEMP1 = state_auto_tuning;
TEMP1 &= 0x80;
}
// delay_ms(1000);
state_auto_tuning &=0x7F;

CT_POZDV1 = POZDV2[1];
CT_POZDV1 = (CT_POZDV1<<8);
CT_POZDV1 |= POZDV2[2];

delay_ms(2);
send_can(CAN_MSG_READ_POSITION,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
// wait until reception is complete
//while((state_auto_tuning&= 0x80) != 0x80);
while (TEMP1 != 0x80){
TEMP1 = state_auto_tuning;
TEMP1 &= 0x80;
}

state_auto_tuning &=0x7F;

CT_POZDV2 = POZDV[1];
CT_POZDV2 = (CT_POZDV2<<8);
CT_POZDV2 |= POZDV[2];

delay_ms(2);
send_can(CAN_MSG_READ_POSITION,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
// wait until reception is complete
//while((state_auto_tuning&= 0x80) != 0x80);
while (TEMP1 != 0x80){
TEMP1 = state_auto_tuning;
TEMP1 &= 0x80;
}

	state_auto_tuning &=0x7F;

	CT_POZDV3 = POZDV2[1];
	CT_POZDV3 = (CT_POZDV3<<8);
	CT_POZDV3 |= POZDV2[2];
}

	TEMP1=SOST;
	TEMP1&=0x20;

if(TEMP1 != 0x20) //нет настройки
{
	state_auto_tuning&=0x7f;

	// Останавливаем двигатели
	send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
	delay_ms(2);
	send_can(CAN_MSG_STOP_MOTOR,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
	delay_ms(2);

	CT_POZDV=0x0000;
	CT_POZDV1=0x0000;
	CT_POZDV2=0x0000;
	CT_POZDV3=0x0000;
	// SOST&=0xdf;  // нет настройки

}
#asm("wdr")  /*сбрасываем watchdog*/

//readADC();//исправленные значения фазы и модуля
}
void OnRelay(void)
{
          ON_SU;
	delay_ms(50);
          ON_RN3
	delay_ms(30);
          ON_RN4;
}
void OffRelay(void)
{
	 OFF_SU;
	 delay_ms(50);
	 OFF_RN3;
	 delay_ms(50);
	 OFF_RN4;
}

/* 		///////////////////////////////тест установка дискретов
		// Получено сообщение по RS485
		TEMP1 = state_auto_tuning;
		TEMP1 &= 0x40;
		if (TEMP1 == 0x40)
		{
			state_auto_tuning &= 0xbf;

			switch (rx_buffer1[0])
			{
				case 0x47: 
				{ //Установка дискретов
					DDRG = rx_buffer1[1];
					delay_ms(50);
					PORTG = rx_buffer1[1];
					delay_ms(200);
					DDRA = rx_buffer1[2];
					delay_ms(50);
					PORTA = rx_buffer1[2];
					break;
				}
				case 0x92:
				{// Настройка >
					can_mes[0]=0x00;	//целевая позиция
					can_mes[1]=0xd0;
					can_mes[2]=0x03;
					can_mes[3]=0x00;
					can_mes[4]=0x00;	//режим старта
					//0 - немедленно переместить вал в указанную абсолютную позицию
					//1 - немедленно переместить вал в указанную относительную позицию
					send_can(0x05,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					delay_ms(2);
					break;
				}
				case 0x93:
				{  //Настройка <
					can_mes[0]=0x00;	//целевая позиция
					can_mes[1]=0x0f;
					can_mes[2]=0x00;
					can_mes[3]=0x00;
					can_mes[4]=0x00;	//режим старта
					//0 - немедленно переместить вал в указанную абсолютную позицию
					//1 - немедленно переместить вал в указанную относительную позицию
					send_can(0x05,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					delay_ms(2);
					break;
				}	
				case 0x94:
				{ // Связь >
					can_mes[0]=0x00;	//целевая позиция
					can_mes[1]=0x1a;
					can_mes[2]=0x0a;
					can_mes[3]=0x00;
					can_mes[4]=0x00;	//режим старта
					//0 - немедленно переместить вал в указанную абсолютную позицию
					//1 - немедленно переместить вал в указанную относительную позицию
					send_can(0x05,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					delay_ms(2);
					break;
				}
				case 0x95:
				{// Связь <
					can_mes[0]=0x00;	//целевая позиция
					can_mes[1]=0x0f;
					can_mes[2]=0x00;
					can_mes[3]=0x00;
					can_mes[4]=0x00;	//режим старта
					//0 - немедленно переместить вал в указанную абсолютную позицию
					//1 - немедленно переместить вал в указанную относительную позицию
					send_can(0x05,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					delay_ms(2);
					break;
				}
				case 0x96:
				{//Стоп
					send_can(0x07,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					delay_ms(2);
					send_can(0x07,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					delay_ms(2);
					break;
				}
				case 0x99:
				{// Конец ручной настройки.
					SOST&=0x7f; // Конец ручной настройки
					delay_ms(10);
					// Останавливаем двигатели
					send_can(0x07,SEND_TO_MOTOR_CC); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					delay_ms(2);
					send_can(0x07,SEND_TO_MOTOR_CN); // отправляем сообщение по CAN (номер сообщения, номер блока - 1 - С связи; 2 - С настройки)
					delay_ms(2);
					send_can(0x12, SEND_TO_PMU);//Конец таррировки
					break;
				}
				default :
				{
					break;
				}
			}
			
			rx_buffer1[0] =0x00;
			rx_buffer1[1] =0x00;
			rx_buffer1[2] =0x00;
			rx_buffer1[3] =0x00;
		} */	
