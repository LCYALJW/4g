/**
  * @file    w25x64/main.c 
  * @brief   Main program body
  * @author  ChipSea MCU Group
  * @version V1.0.0
  * @date   2018.11.01         
  * @copyright CHIPSEA TECHNOLOGIES (SHENZHEN) CORP.
  * @note 
  * <h2><center>&copy; COPYRIGHT 2018 ChipSea</center></h2>
  *
  */

#include "main.h"
#include "common.h"
#include "command.h"
#include "ECR8660.h"
#include <string.h>
#include <stdlib.h>
#include "spi_adc.h"
#include "i2c_ee.h"
#include "init.h"

unsigned char rx_buf [80];							//USART接收数组
unsigned char rx_index = 0x00;					//USART接收个数
unsigned char rx_flag = 0x00;						//USART接收是否完成标志位
unsigned char exist_state = 0;					//寄存器是否存在flash标志位
unsigned char i = 0;										//for循环变量
unsigned char j = 0;										//for循环变量
unsigned int fpga_state = 0;						//FPGA准备完成
unsigned char fpga_error = 0;						//FPGA运行出错标志
unsigned char fpga_error_count = 0;			//FPGA出错次数
unsigned char fpga_sync_count = 0;			//失步次数
unsigned char ret = 0;									//函数返回状态值
unsigned char fpga_select = 0; 					//fpga扫频选择
unsigned char scan_state = 0;
unsigned int ts_number = 0;							//要操作的寄存器个数 
unsigned int reg_count = 0;							//usart一次性写入的寄存器个数
unsigned int data_value = 0;						//读取flash时的暂存变量
unsigned int time_out;									//超时时间
unsigned int fpga_data = 0;							//读fpga的寄存器值
double freq = 0.0;											//频率
unsigned int rx_state = 0;							//RX锁定标志位
unsigned int tx_state = 0;							//TX锁定标志位
unsigned char freq_count = 0;						//频段计数
unsigned int scan_start_freq = 0;				//起始扫描频点
unsigned int scan_end_freq = 0;					//结束扫描频点
unsigned char AD1_lock_state = 0;				//AD1芯片锁定标志位
unsigned char AD2_lock_state = 0;				//AD2芯片锁定标志位
rgroup_t ts_buff[5];										//寄存器暂存数组

unsigned int reg_number = 0;						//AD1寄存器个数
rgroup_t reg_arr[30];										//AD1存储在flash中的寄存器数组

unsigned int reg_number_2 = 0;					//AD2寄存器个数
rgroup_t reg_arr_2[30];									//AD2存储在flash中的寄存器数组

unsigned int freq_default[5];

AD_REG_T ad_reg_arr[] = {
	{0x20004000, 0x00020005},
	{0x20004004, 0x00050005},
	{0x20004028, 0x802f},
	{0x20004030, 0x8000},
	{0x20004008, 1500000},			//接收端	单位：KHZ
	//{0x20004018, 2700000},			//发送端	单位：KHZ
	{0x00201180, 0x22220001},
	{0x00201084, 0x2},
};

extern PEAK_T peak_4g;

/** @addtogroup CS32F0xx_DEMO_Examples
  * @{
  */
  
/** @addtogroup w25x64
  * @{
  */



/**
  * @fn int main(void)
  * @brief  Main program.   MCU clock setting is  configured  through SystemInit() 
  *          in startup file (startup_cs32f0xx.s) before to enter to application main.       
  * @param  None
  * @return None
  */
int main(void)
{ 
		unsigned int ECR8660_Data = 0;
		
		memset(rx_buf,0,sizeof(rx_buf));

		//定时器初始化
		time_config();
	
		//初始化滴答定时器
    cs_start_systick_config();
		
		//GPIO口初始化
		gpio_config();
	
		//初始化I2C
		adc_i2c_init(); 
	
		//IO口中断初始化
		exti0_1_config();
		
		//uart初始化
    usart_config();
	
		delay(1000);
		
		printf("fpga select : %d\r\n",fpga_select);
		printf("--------SPI DEMO--------\r\n");
		
		
		//初始化SPI
		spi_gpio_init();
		
		printf("reg set number : %d\r\n",(sizeof(ad_reg_arr)/sizeof(AD_REG_T)));
			
		fpga_state = 0;
		while(fpga_state != 0xbc){
			spi_write(FPGA_STATE,0xbc);
			fpga_state = spi_read(FPGA_STATE);
			printf("fpga state : %d\r\n",fpga_state);
		}  
		fpga_state = 0;
		
		fpga_state = spi_read(0x19);
		printf("software version %s %s v1.1\r\n",__DATE__,__TIME__);
		printf("test mode: %d\r\n",TEST_MODE);
		printf("fpga select : %d\r\n",fpga_select);
		printf("fpga version : 0x%x",fpga_state);
		
init:
		//复位AD1
		if(!AD1_lock_state){
			spi_write(1,1);
			spi_write(1,0);
			spi_write(1,1);
		}
		
			delay(1000);
		
		//初始化AD1
		if(!AD1_lock_state){
			ECR8660_Data = 0;
			ECR8660_Read(1,ECR8660_Inter_register, 0xf40, &ECR8660_Data);
			printf("0xf40: %x\r\n",ECR8660_Data);
			
			//初始化AD1
			ECR8660_INIT(1);
		}
		
			delay(1000);
		
		//下载代码到AD1
		ECR8660_CodeDownload();
		
		if(!AD1_lock_state){
			ECR8660_write(1,ECR8660_Inter_register, 0x7fc, 0);
			ECR8660_write(1,ECR8660_Inter_register, 0x7fc, 1);
		}

			delay(2000);
		
		if(!AD1_lock_state){
			//读AD1锁定状态寄存器
			ECR8660_Data = 0;
			ECR8660_Read(1,ECR8660_ecternal_register, 0x20000000, &ECR8660_Data);
			printf(" data 0x20000000: %8x ==data\r\n",ECR8660_Data);
			if(ECR8660_Data != 0x123)
			{
				goto init;
			}
			
			//配置AD1 API
			for(i = 0; i < (sizeof(ad_reg_arr)/sizeof(AD_REG_T));i++){
				
				fpga_state = Time_Out(1);
				if(fpga_state);
				else goto init;
				
				ECR8660_write(1,ECR8660_ecternal_register, ad_reg_arr[i].ad_reg, ad_reg_arr[i].ad_reg_value);
				delay(10);
				if(ad_reg_arr[i].ad_reg == 0x201180){
					delay(1000);
				};
			}
			delay(1000);
			
			printf("ADC1 init complate\r\n");
		}
		
		AD1_lock_state = 1;
	
		
		//判断AD1 RX频点是否锁住
		rx_state = 0;
		while((rx_state != 1) && (time_out < AD_TIME_OUT)){
			ECR8660_Read(1,ECR8660_ecternal_register, 0x00200160, &rx_state);
			time_out++;
			__nop();
		}
		if(time_out < AD_TIME_OUT){
			time_out = 0;
		}else{
			time_out = 0;
			AD1_lock_state = 0;
			printf("AD1 RX is not lock\r\n");
			goto init;
		}
		printf("AD1 RX is lock\r\n");
		
		//判断AD1 TX频点是否锁住
		tx_state = 0;
		while((rx_state != 1) && (time_out < AD_TIME_OUT)){
			ECR8660_Read(1,ECR8660_ecternal_register, 0x00200060, &tx_state);
			time_out++;
			__nop();
		}
		if(time_out < AD_TIME_OUT){
			time_out = 0;
		}else{
			time_out = 0;
			AD1_lock_state = 0;
			printf("AD1 TX is not lock\r\n");
			goto init;
		}
		printf("AD1 TX is lock\r\n");
		
		//亮灯提示AD已锁住
		gpio_bits_set(GPIOB,GPIO_PIN_9);
		
		//告诉FPGA AD1已锁定
		spi_write(2,1);
		
		
		usart_interrupt_config(USART1, USART_INT_RXNE, DISABLE); // Disable the USART Receive interrupt
		//AD1 flash读写寄存器值
		flash_unlock();
		flash_flag_clear(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
		
		data_value = *(__IO uint32_t *)REG_START_ADDR;
		printf("AD1 start  mark: %8x\r\n",data_value);
		//取出存储在flash中之前寄存器的值
		
		if(FIRST_PROGRAMME == data_value){
			reg_number = *(__IO uint32_t *)REG_NUMBER_ADDR;
			if((reg_number > 0) && (reg_number <= 30))
			printf("AD1 register number is %d\r\n",reg_number);
			
			//往AD芯片写入之前存储在flash的寄存器和值
			if((reg_number > 0) && (reg_number <= 30)){
				memcpy(reg_arr,(unsigned int*)(REG_NUMBER_ADDR+4),reg_number*8);
				
				//如果有上行或下行寄存器写入，要先写然后写一个特殊寄存器
				for(i = 0; i < reg_number; i++){
					if(reg_arr[i].reg == 0x20004008){
						
						fpga_state = Time_Out(1);
						if(fpga_state);
						else goto init;
						
						ECR8660_write(1,ECR8660_ecternal_register, reg_arr[i].reg, reg_arr[i].reg_value);
						
						fpga_state = Time_Out(1);
						if(fpga_state);
						else goto init;
						
						ECR8660_write(1,ECR8660_ecternal_register, 0x00201180, 0x00000008);
						printf("AD1 write register : %8x value: %8x\r\n",reg_arr[i].reg,reg_arr[i].reg_value);
						
						delay(1000);
					}
				}
				
				for(i = 0; i < reg_number; i++){
					if(reg_arr[i].reg == 0x20004018){
						
						fpga_state = Time_Out(1);
						if(fpga_state);
						else goto init;
						
						ECR8660_write(1,ECR8660_ecternal_register, reg_arr[i].reg, reg_arr[i].reg_value);
						
						fpga_state = Time_Out(1);
						if(fpga_state);
						else goto init;
						
						ECR8660_write(1,ECR8660_ecternal_register, 0x00201180, 0x20000008);
						printf("AD1 write register : %8x value: %8x\r\n",reg_arr[i].reg,reg_arr[i].reg_value);
						
						delay(1000);
					}
				}
				
				//写其他寄存器
				for(i = 0; i < reg_number; i++){
					if((reg_arr[i].reg != 0x20004008) && (reg_arr[i].reg != 0x20004018)){
						
						fpga_state = Time_Out(1);
						if(fpga_state);
						else goto init;
						
						ECR8660_write(1,ECR8660_ecternal_register, reg_arr[i].reg, reg_arr[i].reg_value);
						delay(10);
						printf("AD1 write register : %8x value: %8x\r\n",reg_arr[i].reg,reg_arr[i].reg_value);
					}
				}
				
			}
		}else{
			printf("AD1 this is first time write flash\r\n");
			flash_word_program(REG_START_ADDR,FIRST_PROGRAMME);
			reg_number = 0;
			printf("AD1 register number is %d\r\n",reg_number);
		}
		if(reg_number > 30){
			reg_number = 0;
			printf("AD1 register number is %d\r\n",reg_number);
		}
		flash_lock();
		
		printf("AD1 flash init complate\r\n");
		
		//取出默认频点
		usart_interrupt_config(USART1, USART_INT_RXNE, DISABLE); // Disable the USART Receive interrupt
		//AD1 flash读写寄存器值
		flash_unlock();
		flash_flag_clear(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
		memcpy(freq_default,(unsigned int*)(FREQ_DEFALUT_ADDR),20);
		flash_lock();
		usart_interrupt_config(USART1, USART_INT_RXNE, ENABLE); // Enable the USART Receive interrupt
		printf("default freq init complate\r\n");
		
		for(i= 0; i < 5; i++){
			printf("default freq : %d\r\n",freq_default[i]);
		}
		
    while(1)
    {
			if(rx_flag){
				rx_flag = 0;
				
				switch(rx_buf[4])
				{
					//清空flash中修改过的AD1寄存器
					case 0x1:
						memset(reg_arr,0,sizeof(reg_arr));
						usart_interrupt_config(USART1, USART_INT_RXNE, DISABLE); // Disable the USART Receive interrupt
						flash_unlock(); 
						flash_flag_clear(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
						flash_page_erase(REG_START_ADDR);
						flash_lock(); 
						usart_interrupt_config(USART1, USART_INT_RXNE, ENABLE); // Enable the USART Receive interrupt
						reg_number = 0;
						printf("AD1 clear flash complate\r\n");
						break;
					
					//打印flash中存储的修改过的AD1寄存器
					case 0x2:
						usart_interrupt_config(USART1, USART_INT_RXNE, DISABLE); // Disable the USART Receive interrupt
						flash_unlock();                        
						flash_flag_clear(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
						if((reg_number > 0) && (reg_number <= 30)){//flash中有寄存器
							data_value = *(__IO uint32_t *)REG_NUMBER_ADDR;
							printf("AD1 register number is %d\r\n",data_value);
							for(i= 0; i < reg_number; i++){
								printf("AD1 write register : %8x value: %8x\r\n",reg_arr[i].reg,reg_arr[i].reg_value);
							}
						}else{//flash中没有寄存器
							printf("AD1 flash have no register\r\n");
						}
						flash_lock();
						usart_interrupt_config(USART1, USART_INT_RXNE, ENABLE); // Enable the USART Receive interrupt
						break;
					
					//读AD1寄存器	
					case 0x3:
						memcpy(&ts_buff[0].reg,&rx_buf[5],4);
						
						fpga_state = Time_Out(1);
						if(fpga_state);
						else goto init;
						
						ECR8660_Read(1,ECR8660_SPI_Read, ts_buff[0].reg, &ECR8660_Data);
						printf("AD1 addr:%8x,value:%8x\r\n",ts_buff[0].reg,ECR8660_Data);
					
						break;
					
					//写AD1寄存器
					case 0x4:
						//找出有多少条指令
						for(i = 13; i < 72; i++){
							
							if((rx_buf[i] == 0x52) && (rx_buf[i+1] == 0x45) && (rx_buf[i+2] == 0x47) && (rx_buf[i+3] == 0x4a)){
								
								reg_count++;
							}
						}
						printf("AD1 reg count : %d\r\n",reg_count);
						if(reg_number + reg_count <= 30){
							ts_number = reg_number + reg_count;
						}else{
							printf("AD1 the flash space is full\r\n");
						}
						
						//根据指令个数来操作AD
						switch(reg_count){
							case 1:
									command_one(1);
							
								break;
							case 2:
									command_two(1);
									
								break;
							case 3:
									command_three(1);
								
								break;
							case 4:
									command_four(1);
							
								break;
							case 5:
									command_five(1);
							
								break;
						}
						
						printf("AD1 reg number : %d\r\n",reg_number);
						//如果有上行或下行寄存器写入，要先写然后写一个特殊寄存器
						for(i = 0; i < reg_number; i++){
							if(reg_arr[i].reg == 0x20004008){
								
								fpga_state = Time_Out(1);
								if(fpga_state);
								else goto init;
						
								ECR8660_write(1,ECR8660_ecternal_register, reg_arr[i].reg, reg_arr[i].reg_value);
								
								fpga_state = Time_Out(1);
								if(fpga_state);
								else goto init;
						
								ECR8660_write(1,ECR8660_ecternal_register, 0x00201180, 0x00000008);
								printf("AD1 write register : %8x value: %8x\r\n",reg_arr[i].reg,reg_arr[i].reg_value);
								
								delay(1000);
							}
						}
						
						for(i = 0; i < reg_number; i++){
							if(reg_arr[i].reg == 0x20004018){
								
								fpga_state = Time_Out(1);
								if(fpga_state);
								else goto init;
						
								ECR8660_write(1,ECR8660_ecternal_register, reg_arr[i].reg, reg_arr[i].reg_value);
								
								fpga_state = Time_Out(1);
								if(fpga_state);
								else goto init;
						
								ECR8660_write(1,ECR8660_ecternal_register, 0x00201180, 0x20000008);
								printf("AD1 write register : %8x value: %8x\r\n",reg_arr[i].reg,reg_arr[i].reg_value);
								
								delay(1000);
							}
						}
						
						//写其他寄存器
						for(i = 0; i < reg_number; i++){
							if((reg_arr[i].reg != 0x20004008) && (reg_arr[i].reg != 0x20004018)){
								
								fpga_state = Time_Out(1);
								if(fpga_state);
								else goto init;
						
								ECR8660_write(1,ECR8660_ecternal_register, reg_arr[i].reg, reg_arr[i].reg_value);
								printf("AD1 write register : %8x value: %8x\r\n",reg_arr[i].reg,reg_arr[i].reg_value);
							}
						}
						
						printf("AD1 data programmer spi complate\r\n");
						
						usart_interrupt_config(USART1, USART_INT_RXNE, DISABLE); // Disable the USART Receive interrupt
						
						flash_unlock();                        
						flash_flag_clear(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
						flash_page_erase(REG_START_ADDR);
						
						flash_word_program(REG_START_ADDR,FIRST_PROGRAMME);
						flash_word_program(REG_NUMBER_ADDR,reg_number);
						
						for(i = 0; i < reg_number; i++){                
							flash_word_program(REG_START_ADDR + (8*(i+1)),reg_arr[i].reg);
							flash_word_program(REG_START_ADDR + (8*(i+1))+4,reg_arr[i].reg_value);
						}
						flash_lock();
						
						usart_interrupt_config(USART1, USART_INT_RXNE, ENABLE); // Enable the USART Receive interrupt
						
						printf("AD1 data programmer flash complate\r\n");
						
						break;
					
					//FPGA读寄存器
					case 0x51:
						fpga_data = 0;
						fpga_data = spi_read(rx_buf[5]);
						printf("fpga read reg : %d,fpga read value : %d",rx_buf[5],fpga_data);
						break;
					
					//写FPGA寄存器
					case 0x52:
						spi_write(rx_buf[5],rx_buf[6]);
						printf("fpga write reg : %d,fpga write value : %d",rx_buf[5],rx_buf[6]);
						break;
					
				}	
				
				//清空接收buff
				memset(rx_buf,0,sizeof(rx_buf));
				memset(ts_buff,0,sizeof(ts_buff));
				reg_count = 0;
				ts_number = 0;
			}
			
			if(!scan_state){//扫频
				ret = Sweep_Frequency();
				if(ret == 1){
					printf("sync timeout\r\n");
					freq_count = 0;
					AD1_lock_state = 0;
					goto init;
				}else if(ret == 0){
					freq_default[freq_count] = peak_4g.freq;
					usart_interrupt_config(USART1, USART_INT_RXNE, DISABLE); // Disable the USART Receive interrupt
						
						flash_unlock();                        
						flash_flag_clear(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
						flash_page_erase(FREQ_DEFALUT_ADDR);
						
						for(i = 0; i < 5; i++){                
							flash_word_program(FREQ_DEFALUT_ADDR + (4*i),freq_default[i]);
						}
						flash_lock();
						
					usart_interrupt_config(USART1, USART_INT_RXNE, ENABLE); // Enable the USART Receive interrupt
					scan_state = 1;
				}else if(ret == 2){
					freq_count++;
					if(freq_count == 5)
						freq_count = 0;
				}
				printf("fpga sweep complate\r\n");
			}else{//同步
				if(fpga_error){
					fpga_error = 0;
					if(fpga_sync_count){//有同步
						printf("fpga sync count : %d",fpga_sync_count);
						fpga_sync_count = 0;
						fpga_error_count = 0;
					}else{//没有同步
						fpga_sync_count = 0;
						fpga_error_count++;
						printf("fpga not sync count : %d\r\n",fpga_error_count);
						if(fpga_error_count > 5){
							fpga_error_count = 0;
							printf("fpga sync again\r\n");
							freq_count = 0;
							scan_state = 0;
						}
						if(scan_state){
							spi_write(6,0);
							spi_write(6,1);
						}	
					}
				}
			}
			
		}	
}


/***********************************************************************************
函数功能：USART2中断函数
函数参数：无
函数返回：无
***********************************************************************************/
void USART1_IRQHandler(void)
{
	/* USART Receiver*/
    if (usart_interrupt_status_get(USART1, USART_INT_RXNE) == SET){
			
        rx_buf[rx_index++] = (uint8_t)usart_data_recv(USART1);
				if(rx_index == 4){
					//找到协议头
					if((rx_buf[0] != 0x43) || (rx_buf[1] != 0x53) || (rx_buf[2] != 0x41) || (rx_buf[3] != 0x44)){
						
						rx_buf[0] = rx_buf[1];
						rx_buf[1] = rx_buf[2];
						rx_buf[2] = rx_buf[3];
						rx_index = 3;
					}
				}
				//确定协议尾
        if((rx_index == 80)  ||((rx_buf[rx_index -4]== 0xfe) && (rx_buf[rx_index -3]== 0xfc) && (rx_buf[rx_index -2]== 0xfe) && (rx_buf[rx_index -1]== 0xfc))){
					
            rx_flag = 1;
            rx_index = 0;
        }

    }
}


/***********************************************************************************
函数功能：定时器中断函数
函数参数：无
函数返回：无
***********************************************************************************/
void TIM3_IRQHandler(void)
{
		//1S进入中断一次
		static unsigned char fpga_count = 0;
		static unsigned char wdog_state = 0;
    if (tim_interrupt_status_get(TIM3, TIM_INTR_UPDATE) != RESET)
    {
        tim_interrupt_status_clear(TIM3, TIM_INTR_UPDATE); 
				fpga_count++;
				if(wdog_state){
					wdog_state = 0;
					gpio_bits_set(GPIOB,GPIO_PIN_4);
				}else{
					wdog_state = 1;
					gpio_bits_reset(GPIOB,GPIO_PIN_4);
				}
				
				if(fpga_count > 10){
					fpga_count = 0;
					fpga_error = 1;
				}
    }
}


/***********************************************************************************
函数功能：EXTI中断函数
函数参数：无
函数返回：无
***********************************************************************************/
void EXTI0_1_IRQHandler(void)
{
    if(exti_interrupt_status_get(EXTI_LINE_0) != RESET)
    {
				fpga_sync_count++;
        /* Clear the EXTI line 0  bit */
        exti_interrupt_status_clear(EXTI_LINE_0);
    }
  
}


/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @return None
  */
int fputc(int ch, FILE *f)
{
    UNUSED(f);
    usart_data_send(USART1, (uint8_t) ch);
    while(RESET == usart_flag_status_get(USART1, USART_FLAG_TCF));
    return ch;
}


/**
  * @}
  */

/**
  * @}
  */

