 
#include <90can128.h>
#include <delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "func_variables.h"

extern      long temp_freq; 
extern      int sourse_msg;
extern int   integer_fase;
extern int   integer_module;  
extern int StateBAN;
extern int idx_loop; 
extern unsigned char FF[9]; //хранение частоты  
extern unsigned char BAU_FF[7]; //хранение частоты      
      float coefficient[15] = {0.5,//2^-1
                        0.25,//2^-2
                        0.125,//2^-3
                        0.0625,//2^-4
                        0.03125,//2^-5
                        0.015625,//2^-6
                        0.0078125,//2^-7
                        0.00390625,//2^-8
                        0.001953125,//2^-9
                        0.0009765625,//2^-10
                        0.00048828125,//2^-11
                        0.000244140625,//2^-12
                        0.0001220703125,//2^-13
                        0.00006103515625,//2^-14
                        0.000030517578125//2^-15                                    
      }; 
float coefficient_freq_bau = 2.68438024;
/*произведем перевод частоты в приемлемый вид для БАУ */       
void ConvertFreqInBAU(void)
{
     int loop;
     long offset = 24;       
     long delimiter = 10000000;   
     temp_freq = 0;
     
     for(loop = 0;loop < 7;loop++)
     {
       BAU_FF[loop] = BAU_FF[loop] - 0x30;      
     }
 
     for(loop = 0;loop < 6;loop++)
     {
       temp_freq += (delimiter * BAU_FF[loop]);    
       delimiter /= 10;   
       BAU_FF[loop] = 0;
     }                 
     
     temp_freq = temp_freq * coefficient_freq_bau; 
     
     for(loop = 1;loop < 5;loop++)
     {
          BAU_FF[loop] = (temp_freq >> offset);
          offset -= 8; 
     }
}

void ConvertFreqOutBAU(long freq)
{
	long temp_freq = freq;
	long delimiter = 10000000;
	unsigned char one_char = 0;
	int i;
	int idx_offset = 0;
	
	for(i = 0; i < 8; i++)
	{
		one_char = temp_freq/delimiter;
		temp_freq -= one_char*delimiter;
        delimiter /= 10;
		if(i == 5)
		{
			FreqStore[idx_offset] = ',';
			FreqStore[++idx_offset] = 0x30 + one_char;
		}
		else
		{
			FreqStore[idx_offset++] = 0x30 + one_char;
		}		
	}

}

int  ConvertFaseOutBAU(unsigned char msg_buff[])
{
	
	  long  mantissa = 0;
	  int   cmp_bit = 0;
	  long  temp_msg = 0;
	  int   offset = 8;
	  float convert_fase = 0.0; 
	  int   integer_f = 0;
	  int   resvalue = 0;
	  bool  flag_negative = false;
	  idx_loop = 0;  
	  
	  
	  integer_f = (msg_buff[6] & 0x60) >> 5;
	  
	  for(idx_loop = 6;idx_loop < 8;idx_loop++)
	  { 
		temp_msg = msg_buff[idx_loop];
		temp_msg <<= offset;
		mantissa |= temp_msg; 
		temp_msg = 0;
		offset -= 8; 
	  }

	  /*если фаза отрицательная произведем инверсию*/
	  if((msg_buff[6] & 0x80 ) != 0)
	  {
		 mantissa = mantissa ^ 0x1fff;
		 integer_f = integer_f ^ 0x03;
		 flag_negative = true;
	  }

	  offset = 12;
	  
	  for(idx_loop = 0; idx_loop < 13;idx_loop++)
	  {    
		  cmp_bit = mantissa & (int)(0x0001) << offset; 
		  
		  if(cmp_bit != 0)
		  {
			//convert_fase += 1;
			convert_fase += coefficient[idx_loop];
		  }

		  offset--;
	  }

	  convert_fase += integer_f;
	  
	  if(flag_negative)
	  {
		 convert_fase *= -1; 
	  }
	  
	  convert_fase = convert_fase * 10;
	  
	  resvalue = convert_fase;
	   
	  return resvalue;
	  
}

int  ConvertModulOutBAU(unsigned char msg_buff[])
{
	
	  long  mantissa = 0;
	  int   cmp_bit = 0;
	  long  temp_msg = 0;
	  int   offset = 8;
	  float convert_module = 0.0; 
	  int   integer_m = 0;
	  int   resvalue = 0;
	  bool  flag_negative = false;
	  idx_loop = 0;  
	  
	  
	  integer_m = msg_buff[5] & 0x01;
	  
	  for(idx_loop = 6;idx_loop < 8;idx_loop++)
	  { 
		temp_msg = msg_buff[idx_loop];
		temp_msg <<= offset;
		mantissa |= temp_msg; 
		temp_msg = 0;
		offset -= 8; 
	  }

	  offset = 15;
	  
	  for(idx_loop = 0; idx_loop < 15;idx_loop++)
	  {    
		  cmp_bit = mantissa & (int)(0x0001) << offset; 
		  
		  if(cmp_bit != 0)
		  {
			convert_module += coefficient[idx_loop];
		  }

		  offset--;
	  }

	  convert_module += integer_m;
	  
	  convert_module = convert_module * 10;
	  
	  resvalue = convert_module;
	   
	  return resvalue;
	  
}

bool CompareFreqBAU(unsigned char msg_buff[])
{
	
	  long compare_freq = 0; 
	  long compare_freq_whith_coeff = 0; 
	  long temp_msg = 0;
	  long offset = 24;
	  bool  retvalue = false;   
	  
	  int i;
	  
	  for(i = 4;i < 8;i++)
	  { 
		temp_msg = msg_buff[i];
		temp_msg <<= offset;
		compare_freq |= temp_msg; 
		temp_msg = 0;
		offset -= 8; 
	  } 
	  
	   compare_freq_whith_coeff = compare_freq / coefficient_freq_bau;
	   
	   if(temp_freq == compare_freq)
	   {
		  retvalue = true;
	   }
	   /*если частота установлена с РПДУ*/
	   else if(StateBAN & SET_FREQ_RS485)
	   {
			temp_freq = compare_freq;
		   /*произведем перевод частоты для отображения на дисплей*/
			ConvertFreqOutBAU(compare_freq_whith_coeff);
		   /*произведем вывод значение частоты на дисплей*/
			SetFreqToLCD();
			/*сбросим флаг состояния BAN*/
			StateBAN &= ~SET_FREQ_RS485;
			
			retvalue = true;
	   }
	   return retvalue;
	   
}
