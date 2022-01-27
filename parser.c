size_t leng_part_command;

bool Parser_TCP_Packet2 (uint8_t source[],bool flag_part_command,int num_command)
{
	Command_t result = CMD_NOT_FOUND;
	size_t num_data;
	int cnt_num_coincide;

	num_data = strlen((const char*)source);//длина принятой строки данных
	if (!flag_part_command)//если ранее небыло найденно части комманды 
	{
	for(int i = 0 ;i < 10;i++)//перебираем по порядку все возможные комманды
		{
			size_t leng_command;
			leng_command = strlen(commands[i]);
			if (num_data < leng_command)
			{
				leng_command = num_data;
				flag_part_command = true;
			}
			for(int j = 0;j<leng_command;j++)
			{
				if (source[j] == commands[i][j])
				{
					cnt_num_coincide++; 
				}
				else
				{
					break;
				}
			}
			if (cnt_num_coincide == leng_command)
			{
				if(flag_part_command)
				{
					/* комманда найденна частично,
					получаем число найденных символов --> leng_command
					и номер комманды                  --> i 
					*/
					leng_part_command = leng_command;
					num_partial_command = i;
					result = CMD_PARTIAL;
				}
				else
				{
					/* комманда найденна полностью вернуть значение и номер комманды*/
					result = CMD_FOUND;
				}
				return result;
			}
			
			kom[i] = strstr((char *)&source[shift_position_found],commands[i]);
			
			if (kom[i]!= 0)
			{
				FlagsCommands[i] = true;//Если комманда обнаружена выставляем флаг для дальнейшего опознания и обработки комманды
				flag_comm_in_pack = true;//Устанавливаем признак наличия найденной комманды
			} 
		}
		return result;
	}
	else//продолжаем поиск ранее найденной комманды
	{
			
			int leng_end_command;
			leng_end_command = strlen(commands[i]) - leng_command;//определяем число оставшихся символов для поиска 
			if (num_data < leng_command)
			{
				leng_command = num_data;
				flag_part_command = true;
			}
			for(int j = 0;j<leng_command;j++)
			{
				if (source[j] == commands[i][j])
				{
					cnt_num_coincide++; 
				}
				else
				{
					return result;
				}
			}
			if (cnt_num_coincide == leng_command)
			{
				if(flag_part_command)
				{
					/* комманда найденна частично,
					получаем число найденных символов --> leng_command
					и номер комманды                  --> i 
					*/
					leng_part_command = leng_command;
					num_partial_command = i;
					result = CMD_PARTIAL;
				}
				else
				{
					/* комманда найденна полностью вернуть значение и номер комманды*/
					result = CMD_FOUND;
				}
				return result;
			}	
	}
	return result;
}
