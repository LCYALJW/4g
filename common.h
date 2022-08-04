#ifndef _SPI_COMMON_H_
#define _SPI_COMMON_H_
#include <stdio.h>
#include <string.h>
#include "cs32f0xx_gpio.h"


#define FPGA_READ() 						 	gpio_input_data_bit_read(GPIOB,GPIO_PIN_1)		//��SPI�ܽ�
#define FPGA_READ_2() 						gpio_input_data_bit_read(GPIOB,GPIO_PIN_0)		//��SPI�ܽ�

#define FIRST_PROGRAMME						((uint32_t)0x10203040)												//�Ƿ��һ��д��������־

#define REG_START_ADDR  					((uint32_t)0x08008000)												//AD1�洢д���ļĴ�����flash�е���ʼ��ַ
#define REG_NUMBER_ADDR						((uint32_t)0x08008004)												//AD1�Ĵ�������д��ĵ�ַ

#define REG_START_ADDR_2  				((uint32_t)0x08009000)												//AD2�洢д���ļĴ�����flash�е���ʼ��ַ
#define REG_NUMBER_ADDR_2					((uint32_t)0x08009004)												//AD2�Ĵ�������д��ĵ�ַ

#define FREQ_DEFALUT_ADDR  				((uint32_t)0x0800a000)

#define AD_TIME_OUT								500000

#define TEST_MODE 								0

typedef struct{
	unsigned int reg;
	unsigned int reg_value;
}rgroup_t;

typedef struct {
	unsigned int ad_reg;
	unsigned int ad_reg_value;
}AD_REG_T;



#endif


