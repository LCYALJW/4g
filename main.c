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

unsigned char rx_buf [80];							//USART��������
unsigned char rx_index = 0x00;					//USART���ո���
unsigned char rx_flag = 0x00;						//USART�����Ƿ���ɱ�־λ
unsigned char exist_state = 0;					//�Ĵ����Ƿ����flash��־λ
unsigned char i = 0;										//forѭ������
unsigned char j = 0;										//forѭ������
unsigned int fpga_state = 0;						//FPGA׼�����
unsigned char fpga_error = 0;						//FPGA���г����־
unsigned char fpga_error_count = 0;			//FPGA�������
unsigned char fpga_sync_count = 0;			//ʧ������
unsigned char ret = 0;									//��������״ֵ̬
unsigned char fpga_select = 0; 					//fpgaɨƵѡ��
unsigned char scan_state = 0;
unsigned int ts_number = 0;							//Ҫ�����ļĴ������� 
unsigned int reg_count = 0;							//usartһ����д��ļĴ�������
unsigned int data_value = 0;						//��ȡflashʱ���ݴ����
unsigned int time_out;									//��ʱʱ��
unsigned int fpga_data = 0;							//��fpga�ļĴ���ֵ
double freq = 0.0;											//Ƶ��
unsigned int rx_state = 0;							//RX������־λ
unsigned int tx_state = 0;							//TX������־λ
unsigned char freq_count = 0;						//Ƶ�μ���
unsigned int scan_start_freq = 0;				//��ʼɨ��Ƶ��
unsigned int scan_end_freq = 0;					//����ɨ��Ƶ��
unsigned char AD1_lock_state = 0;				//AD1оƬ������־λ
unsigned char AD2_lock_state = 0;				//AD2оƬ������־λ
rgroup_t ts_buff[5];										//�Ĵ����ݴ�����

unsigned int reg_number = 0;						//AD1�Ĵ�������
rgroup_t reg_arr[30];										//AD1�洢��flash�еļĴ�������

unsigned int reg_number_2 = 0;					//AD2�Ĵ�������
rgroup_t reg_arr_2[30];									//AD2�洢��flash�еļĴ�������

unsigned int freq_default[5];

AD_REG_T ad_reg_arr[] = {
	{0x20004000, 0x00020005},
	{0x20004004, 0x00050005},
	{0x20004028, 0x802f},
	{0x20004030, 0x8000},
	{0x20004008, 1500000},			//���ն�	��λ��KHZ
	//{0x20004018, 2700000},			//���Ͷ�	��λ��KHZ
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

		//��ʱ����ʼ��
		time_config();
	
		//��ʼ���δ�ʱ��
    cs_start_systick_config();
		
		//GPIO�ڳ�ʼ��
		gpio_config();
	
		//��ʼ��I2C
		adc_i2c_init(); 
	
		//IO���жϳ�ʼ��
		exti0_1_config();
		
		//uart��ʼ��
    usart_config();
	
		delay(1000);
		
		printf("fpga select : %d\r\n",fpga_select);
		printf("--------SPI DEMO--------\r\n");
		
		
		//��ʼ��SPI
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
		//��λAD1
		if(!AD1_lock_state){
			spi_write(1,1);
			spi_write(1,0);
			spi_write(1,1);
		}
		
			delay(1000);
		
		//��ʼ��AD1
		if(!AD1_lock_state){
			ECR8660_Data = 0;
			ECR8660_Read(1,ECR8660_Inter_register, 0xf40, &ECR8660_Data);
			printf("0xf40: %x\r\n",ECR8660_Data);
			
			//��ʼ��AD1
			ECR8660_INIT(1);
		}
		
			delay(1000);
		
		//���ش��뵽AD1
		ECR8660_CodeDownload();
		
		if(!AD1_lock_state){
			ECR8660_write(1,ECR8660_Inter_register, 0x7fc, 0);
			ECR8660_write(1,ECR8660_Inter_register, 0x7fc, 1);
		}

			delay(2000);
		
		if(!AD1_lock_state){
			//��AD1����״̬�Ĵ���
			ECR8660_Data = 0;
			ECR8660_Read(1,ECR8660_ecternal_register, 0x20000000, &ECR8660_Data);
			printf(" data 0x20000000: %8x ==data\r\n",ECR8660_Data);
			if(ECR8660_Data != 0x123)
			{
				goto init;
			}
			
			//����AD1 API
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
	
		
		//�ж�AD1 RXƵ���Ƿ���ס
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
		
		//�ж�AD1 TXƵ���Ƿ���ס
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
		
		//������ʾAD����ס
		gpio_bits_set(GPIOB,GPIO_PIN_9);
		
		//����FPGA AD1������
		spi_write(2,1);
		
		
		usart_interrupt_config(USART1, USART_INT_RXNE, DISABLE); // Disable the USART Receive interrupt
		//AD1 flash��д�Ĵ���ֵ
		flash_unlock();
		flash_flag_clear(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
		
		data_value = *(__IO uint32_t *)REG_START_ADDR;
		printf("AD1 start  mark: %8x\r\n",data_value);
		//ȡ���洢��flash��֮ǰ�Ĵ�����ֵ
		
		if(FIRST_PROGRAMME == data_value){
			reg_number = *(__IO uint32_t *)REG_NUMBER_ADDR;
			if((reg_number > 0) && (reg_number <= 30))
			printf("AD1 register number is %d\r\n",reg_number);
			
			//��ADоƬд��֮ǰ�洢��flash�ļĴ�����ֵ
			if((reg_number > 0) && (reg_number <= 30)){
				memcpy(reg_arr,(unsigned int*)(REG_NUMBER_ADDR+4),reg_number*8);
				
				//��������л����мĴ���д�룬Ҫ��дȻ��дһ������Ĵ���
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
				
				//д�����Ĵ���
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
		
		//ȡ��Ĭ��Ƶ��
		usart_interrupt_config(USART1, USART_INT_RXNE, DISABLE); // Disable the USART Receive interrupt
		//AD1 flash��д�Ĵ���ֵ
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
					//���flash���޸Ĺ���AD1�Ĵ���
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
					
					//��ӡflash�д洢���޸Ĺ���AD1�Ĵ���
					case 0x2:
						usart_interrupt_config(USART1, USART_INT_RXNE, DISABLE); // Disable the USART Receive interrupt
						flash_unlock();                        
						flash_flag_clear(FLASH_FLAG_ENDF | FLASH_FLAG_PGERR | FLASH_FLAG_WPERR);
						if((reg_number > 0) && (reg_number <= 30)){//flash���мĴ���
							data_value = *(__IO uint32_t *)REG_NUMBER_ADDR;
							printf("AD1 register number is %d\r\n",data_value);
							for(i= 0; i < reg_number; i++){
								printf("AD1 write register : %8x value: %8x\r\n",reg_arr[i].reg,reg_arr[i].reg_value);
							}
						}else{//flash��û�мĴ���
							printf("AD1 flash have no register\r\n");
						}
						flash_lock();
						usart_interrupt_config(USART1, USART_INT_RXNE, ENABLE); // Enable the USART Receive interrupt
						break;
					
					//��AD1�Ĵ���	
					case 0x3:
						memcpy(&ts_buff[0].reg,&rx_buf[5],4);
						
						fpga_state = Time_Out(1);
						if(fpga_state);
						else goto init;
						
						ECR8660_Read(1,ECR8660_SPI_Read, ts_buff[0].reg, &ECR8660_Data);
						printf("AD1 addr:%8x,value:%8x\r\n",ts_buff[0].reg,ECR8660_Data);
					
						break;
					
					//дAD1�Ĵ���
					case 0x4:
						//�ҳ��ж�����ָ��
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
						
						//����ָ�����������AD
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
						//��������л����мĴ���д�룬Ҫ��дȻ��дһ������Ĵ���
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
						
						//д�����Ĵ���
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
					
					//FPGA���Ĵ���
					case 0x51:
						fpga_data = 0;
						fpga_data = spi_read(rx_buf[5]);
						printf("fpga read reg : %d,fpga read value : %d",rx_buf[5],fpga_data);
						break;
					
					//дFPGA�Ĵ���
					case 0x52:
						spi_write(rx_buf[5],rx_buf[6]);
						printf("fpga write reg : %d,fpga write value : %d",rx_buf[5],rx_buf[6]);
						break;
					
				}	
				
				//��ս���buff
				memset(rx_buf,0,sizeof(rx_buf));
				memset(ts_buff,0,sizeof(ts_buff));
				reg_count = 0;
				ts_number = 0;
			}
			
			if(!scan_state){//ɨƵ
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
			}else{//ͬ��
				if(fpga_error){
					fpga_error = 0;
					if(fpga_sync_count){//��ͬ��
						printf("fpga sync count : %d",fpga_sync_count);
						fpga_sync_count = 0;
						fpga_error_count = 0;
					}else{//û��ͬ��
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
�������ܣ�USART2�жϺ���
������������
�������أ���
***********************************************************************************/
void USART1_IRQHandler(void)
{
	/* USART Receiver*/
    if (usart_interrupt_status_get(USART1, USART_INT_RXNE) == SET){
			
        rx_buf[rx_index++] = (uint8_t)usart_data_recv(USART1);
				if(rx_index == 4){
					//�ҵ�Э��ͷ
					if((rx_buf[0] != 0x43) || (rx_buf[1] != 0x53) || (rx_buf[2] != 0x41) || (rx_buf[3] != 0x44)){
						
						rx_buf[0] = rx_buf[1];
						rx_buf[1] = rx_buf[2];
						rx_buf[2] = rx_buf[3];
						rx_index = 3;
					}
				}
				//ȷ��Э��β
        if((rx_index == 80)  ||((rx_buf[rx_index -4]== 0xfe) && (rx_buf[rx_index -3]== 0xfc) && (rx_buf[rx_index -2]== 0xfe) && (rx_buf[rx_index -1]== 0xfc))){
					
            rx_flag = 1;
            rx_index = 0;
        }

    }
}


/***********************************************************************************
�������ܣ���ʱ���жϺ���
������������
�������أ���
***********************************************************************************/
void TIM3_IRQHandler(void)
{
		//1S�����ж�һ��
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
�������ܣ�EXTI�жϺ���
������������
�������أ���
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

