#ifndef _SPI_COMMON_H_
#define _SPI_COMMON_H_
#include <stdio.h>
#include <string.h>
#include "cs32f0xx_gpio.h"


#define FPGA_READ() 						 	gpio_input_data_bit_read(GPIOB,GPIO_PIN_1)		//读SPI管脚
#define FPGA_READ_2() 						gpio_input_data_bit_read(GPIOB,GPIO_PIN_0)		//读SPI管脚

#define FIRST_PROGRAMME						((uint32_t)0x10203040)												//是否第一次写入的特殊标志

#define REG_START_ADDR  					((uint32_t)0x08008000)												//AD1存储写过的寄存器在flash中的起始地址
#define REG_NUMBER_ADDR						((uint32_t)0x08008004)												//AD1寄存器个数写入的地址

#define REG_START_ADDR_2  				((uint32_t)0x08009000)												//AD2存储写过的寄存器在flash中的起始地址
#define REG_NUMBER_ADDR_2					((uint32_t)0x08009004)												//AD2寄存器个数写入的地址

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


