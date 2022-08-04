#include "command.h"
#include "common.h"

extern unsigned int time_out;
extern unsigned char AD1_lock_state;
extern unsigned char AD2_lock_state;
extern unsigned char i;							
extern unsigned char j;		
extern unsigned int ts_number;				
extern unsigned int reg_count;			
extern unsigned char exist_state;						
extern rgroup_t ts_buff[5];			
extern unsigned char rx_buf [80];
					
extern unsigned int reg_number;	
extern rgroup_t reg_arr[30];
				
extern unsigned int reg_number_2;	
extern rgroup_t reg_arr_2[30];	

unsigned char Time_Out(unsigned char ad_number)
{
	unsigned char ret = 0;
	time_out = 0;
	switch(ad_number){
		case 1:
			while(FPGA_READ() && (time_out < AD_TIME_OUT)){
					time_out++;
					__nop();
				}
				if(time_out < AD_TIME_OUT){
					time_out = 0;
					ret = 1;
				}
				else{
					time_out = 0;
					AD1_lock_state = 0;
					ret = 0;
				}
			break;
				
		case 2:
			while(FPGA_READ_2() && (time_out < AD_TIME_OUT)){
					time_out++;
					__nop();
				}
				if(time_out < AD_TIME_OUT){
					time_out = 0;
					ret = 1;
				}
				else{
					time_out = 0;
					AD2_lock_state = 0;
					ret = 0;
				}
			break;
	}
	return ret;
	
}

void command_one(unsigned char ad_number)
{
	switch(ad_number){
		case 1:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				for(i = 0; i < reg_count; i++){
					for(j = 0; j < ts_number; j++){
						if(ts_buff[i].reg == reg_arr[j].reg){
							//旧寄存器改动新值
							printf("AD1 register is exist\r\n");
							reg_arr[j].reg_value = ts_buff[i].reg_value;
							exist_state = 1;
							break;
						}else{
							exist_state = 0;
						}
					}
					if(!exist_state){
						reg_arr[reg_number].reg = ts_buff[i].reg;
						reg_arr[reg_number].reg_value = ts_buff[i].reg_value;
						printf("AD1 write addr : %8x , write value : %8x\r\n",reg_arr[reg_number].reg,reg_arr[reg_number].reg_value);
						if(reg_number < 30)
						reg_number++;
					}
				}
			break;
		
		case 2:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				for(i = 0; i < reg_count; i++){
					for(j = 0; j < ts_number; j++){
						if(ts_buff[i].reg == reg_arr_2[j].reg){
							//旧寄存器改动新值
							printf("AD2 register is exist\r\n");
							reg_arr_2[j].reg_value = ts_buff[i].reg_value;
							exist_state = 1;
							break;
						}else{
							exist_state = 0;
						}
					}
					if(!exist_state){
						reg_arr_2[reg_number_2].reg = ts_buff[i].reg;
						reg_arr_2[reg_number_2].reg_value = ts_buff[i].reg_value;
						printf("AD2 write addr : %8x , write value : %8x\r\n",reg_arr_2[reg_number_2].reg,reg_arr_2[reg_number_2].reg_value);
						if(reg_number_2 < 30)
						reg_number_2++;
					}
				}
			break;
	}
		
}


void command_two(unsigned char ad_number)
{
	switch(ad_number){
		case 1:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				memcpy(&ts_buff[1].reg,&rx_buf[17],4);
				memcpy(&ts_buff[1].reg_value,&rx_buf[21],4);
				for(i = 0; i < reg_count; i++){
						for(j = 0; j < ts_number; j++){
							if(ts_buff[i].reg == reg_arr[j].reg){
								//旧寄存器改动新值
								printf("AD1 register is exist\r\n");
								reg_arr[j].reg_value = ts_buff[i].reg_value;
								exist_state = 1;
								break;
							}else{
								exist_state = 0;
							}
						}
						if(!exist_state){
							reg_arr[reg_number].reg = ts_buff[i].reg;
							reg_arr[reg_number].reg_value = ts_buff[i].reg_value;
							printf("AD1 write addr : %8x , write value : %8x\r\n",reg_arr[reg_number].reg,reg_arr[reg_number].reg_value);
							if(reg_number < 30)
							reg_number++;
						}
				}
			break;
				
		case 2:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				memcpy(&ts_buff[1].reg,&rx_buf[17],4);
				memcpy(&ts_buff[1].reg_value,&rx_buf[21],4);
				for(i = 0; i < reg_count; i++){
						for(j = 0; j < ts_number; j++){
							if(ts_buff[i].reg == reg_arr_2[j].reg){
								//旧寄存器改动新值
								printf("AD2 register is exist\r\n");
								reg_arr_2[j].reg_value = ts_buff[i].reg_value;
								exist_state = 1;
								break;
							}else{
								exist_state = 0;
							}
						}
						if(!exist_state){
							reg_arr_2[reg_number_2].reg = ts_buff[i].reg;
							reg_arr_2[reg_number_2].reg_value = ts_buff[i].reg_value;
							printf("AD2 write addr : %8x , write value : %8x\r\n",reg_arr_2[reg_number_2].reg,reg_arr_2[reg_number_2].reg_value);
							if(reg_number_2 < 30)
							reg_number_2++;
						}
				}
			
			break;
	}
		
}


void command_three(unsigned char ad_number)
{
	switch(ad_number){
		case 1:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				memcpy(&ts_buff[1].reg,&rx_buf[17],4);
				memcpy(&ts_buff[1].reg_value,&rx_buf[21],4);
				memcpy(&ts_buff[2].reg,&rx_buf[29],4);
				memcpy(&ts_buff[2].reg_value,&rx_buf[33],4);
				for(i = 0; i < reg_count; i++){
						for(j = 0; j < ts_number; j++){
							if(ts_buff[i].reg == reg_arr[j].reg){
								//旧寄存器改动新值
								printf("AD1 register is exist\r\n");
								reg_arr[j].reg_value = ts_buff[i].reg_value;
								exist_state = 1;
								break;
							}else{
								exist_state = 0;
							}
						}
						if(!exist_state){
							reg_arr[reg_number].reg = ts_buff[i].reg;
							reg_arr[reg_number].reg_value = ts_buff[i].reg_value;
							printf("AD1 write addr : %8x , write value : %8x\r\n",reg_arr[reg_number].reg,reg_arr[reg_number].reg_value);
							if(reg_number < 30)
							reg_number++;
						}
				}
			break;
		
		case 2:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				memcpy(&ts_buff[1].reg,&rx_buf[17],4);
				memcpy(&ts_buff[1].reg_value,&rx_buf[21],4);
				memcpy(&ts_buff[2].reg,&rx_buf[29],4);
				memcpy(&ts_buff[2].reg_value,&rx_buf[33],4);
				for(i = 0; i < reg_count; i++){
						for(j = 0; j < ts_number; j++){
							if(ts_buff[i].reg == reg_arr_2[j].reg){
								//旧寄存器改动新值
								printf("AD2 register is exist\r\n");
								reg_arr_2[j].reg_value = ts_buff[i].reg_value;
								exist_state = 1;
								break;
							}else{
								exist_state = 0;
							}
						}
						if(!exist_state){
							reg_arr_2[reg_number_2].reg = ts_buff[i].reg;
							reg_arr_2[reg_number_2].reg_value = ts_buff[i].reg_value;
							printf("AD2 write addr : %8x , write value : %8x\r\n",reg_arr_2[reg_number_2].reg,reg_arr_2[reg_number_2].reg_value);
							if(reg_number_2 < 30)
							reg_number_2++;
						}
				}
			break;
		
	}
		
}

void command_four(unsigned char ad_number)
{
	switch(ad_number){
		case 1:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				memcpy(&ts_buff[1].reg,&rx_buf[17],4);
				memcpy(&ts_buff[1].reg_value,&rx_buf[21],4);
				memcpy(&ts_buff[2].reg,&rx_buf[29],4);
				memcpy(&ts_buff[2].reg_value,&rx_buf[33],4);
				memcpy(&ts_buff[3].reg,&rx_buf[41],4);
				memcpy(&ts_buff[3].reg_value,&rx_buf[45],4);
				for(i = 0; i < reg_count; i++){
						for(j = 0; j < ts_number; j++){
							if(ts_buff[i].reg == reg_arr[j].reg){
								//旧寄存器改动新值
								printf("AD1 register is exist\r\n");
								reg_arr[j].reg_value = ts_buff[i].reg_value;
								exist_state = 1;
								break;
							}else{
								exist_state = 0;
							}
						}
						if(!exist_state){
							reg_arr[reg_number].reg = ts_buff[i].reg;
							reg_arr[reg_number].reg_value = ts_buff[i].reg_value;
							printf("AD1 write addr : %8x , write value : %8x\r\n",reg_arr[reg_number].reg,reg_arr[reg_number].reg_value);
							if(reg_number < 30)
							reg_number++;
						}
				}
			break;
				
		case 2:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				memcpy(&ts_buff[1].reg,&rx_buf[17],4);
				memcpy(&ts_buff[1].reg_value,&rx_buf[21],4);
				memcpy(&ts_buff[2].reg,&rx_buf[29],4);
				memcpy(&ts_buff[2].reg_value,&rx_buf[33],4);
				memcpy(&ts_buff[3].reg,&rx_buf[41],4);
				memcpy(&ts_buff[3].reg_value,&rx_buf[45],4);
				for(i = 0; i < reg_count; i++){
						for(j = 0; j < ts_number; j++){
							if(ts_buff[i].reg == reg_arr_2[j].reg){
								//旧寄存器改动新值
								printf("AD2 register is exist\r\n");
								reg_arr_2[j].reg_value = ts_buff[i].reg_value;
								exist_state = 1;
								break;
							}else{
								exist_state = 0;
							}
						}
						if(!exist_state){
							reg_arr_2[reg_number_2].reg = ts_buff[i].reg;
							reg_arr_2[reg_number_2].reg_value = ts_buff[i].reg_value;
							printf("AD2 write addr : %8x , write value : %8x\r\n",reg_arr_2[reg_number_2].reg,reg_arr_2[reg_number_2].reg_value);
							if(reg_number_2 < 30)
							reg_number_2++;
						}
				}
			break;
	}
		
}

void command_five(unsigned char ad_number) 
{
	switch(ad_number){
		case 1:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				memcpy(&ts_buff[1].reg,&rx_buf[17],4);
				memcpy(&ts_buff[1].reg_value,&rx_buf[21],4);
				memcpy(&ts_buff[2].reg,&rx_buf[29],4);
				memcpy(&ts_buff[2].reg_value,&rx_buf[33],4);
				memcpy(&ts_buff[3].reg,&rx_buf[41],4);
				memcpy(&ts_buff[3].reg_value,&rx_buf[45],4);
				memcpy(&ts_buff[4].reg,&rx_buf[53],4);
				memcpy(&ts_buff[4].reg_value,&rx_buf[57],4);
				for(i = 0; i < reg_count; i++){
						for(j = 0; j < ts_number; j++){
							if(ts_buff[i].reg == reg_arr[j].reg){
								//旧寄存器改动新值
								printf("AD1 register is exist\r\n");
								reg_arr[j].reg_value = ts_buff[i].reg_value;
								exist_state = 1;
								break;
							}else{
								exist_state = 0;
							}
						}
						if(!exist_state){
							reg_arr[reg_number].reg = ts_buff[i].reg;
							reg_arr[reg_number].reg_value = ts_buff[i].reg_value;
							printf("AD1 write addr : %8x , write value : %8x\r\n",reg_arr[reg_number].reg,reg_arr[reg_number].reg_value);
							if(reg_number < 30)
							reg_number++;
						}
				}
			break;
				
		case 2:
				memcpy(&ts_buff[0].reg,&rx_buf[5],4);
				memcpy(&ts_buff[0].reg_value,&rx_buf[9],4);
				memcpy(&ts_buff[1].reg,&rx_buf[17],4);
				memcpy(&ts_buff[1].reg_value,&rx_buf[21],4);
				memcpy(&ts_buff[2].reg,&rx_buf[29],4);
				memcpy(&ts_buff[2].reg_value,&rx_buf[33],4);
				memcpy(&ts_buff[3].reg,&rx_buf[41],4);
				memcpy(&ts_buff[3].reg_value,&rx_buf[45],4);
				memcpy(&ts_buff[4].reg,&rx_buf[53],4);
				memcpy(&ts_buff[4].reg_value,&rx_buf[57],4);
				for(i = 0; i < reg_count; i++){
						for(j = 0; j < ts_number; j++){
							if(ts_buff[i].reg == reg_arr_2[j].reg){
								//旧寄存器改动新值
								printf("AD2 register is exist\r\n");
								reg_arr_2[j].reg_value = ts_buff[i].reg_value;
								exist_state = 1;
								break;
							}else{
								exist_state = 0;
							}
						}
						if(!exist_state){
							reg_arr_2[reg_number_2].reg = ts_buff[i].reg;
							reg_arr_2[reg_number_2].reg_value = ts_buff[i].reg_value;
							printf("AD2 write addr : %8x , write value : %8x\r\n",reg_arr_2[reg_number_2].reg,reg_arr_2[reg_number_2].reg_value);
							if(reg_number_2 < 30)
							reg_number_2++;
						}
				}
			break;
	}
		
}
