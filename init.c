#include "init.h"
#include "cs32f0xx_gpio.h"
#include "cs32f0xx_rcu.h"
#include "cs32f0xx_i2c.h"
#include "cs32f0xx_usart.h"
#include "cs32f0xx.h"
#include "systick.h"
#include <stdio.h>
#include "cs32f0xx_conf.h"


#define SPI_FLASH_CS_LOW()       gpio_bits_reset(GPIOA, GPIO_PIN_4)
#define SPI_FLASH_CS_HIGH()      gpio_bits_set(GPIOA, GPIO_PIN_4)
#define CLK_LOW()       				 gpio_bits_reset(GPIOA, GPIO_PIN_5)          
#define CLK_HIGH()       				 gpio_bits_set(GPIOA, GPIO_PIN_5)
#define MOSI_LOW()       				 gpio_bits_reset(GPIOA, GPIO_PIN_7)
#define MOSI_HIGH()       			 gpio_bits_set(GPIOA, GPIO_PIN_7)
#define MISO_READ() 						 gpio_input_data_bit_read(GPIOA,GPIO_PIN_6)


/***********************************************************************************
函数功能：GPIO口初始化函数
函数参数：无
函数返回：无
***********************************************************************************/
void gpio_config(void)
{ 
    gpio_config_t  gpio_config_struct; 
    
    // Clock Config	
    rcu_ahb_periph_clock_enable_ctrl(RCU_AHB_PERI_PORTA, ENABLE);
		rcu_ahb_periph_clock_enable_ctrl(RCU_AHB_PERI_PORTB, ENABLE);
		rcu_ahb_periph_clock_enable_ctrl(RCU_AHB_PERI_PORTF, ENABLE);
			
		gpio_config_struct.gpio_pin = GPIO_PIN_9;	 				//RUNNING LED
    gpio_config_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_config_struct.gpio_speed = GPIO_SPEED_HIGH;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_PP;
    gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIOB, &gpio_config_struct);
		
		gpio_config_struct.gpio_pin = GPIO_PIN_10;	 			//FPGA_GPIO2
    gpio_config_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_config_struct.gpio_speed = GPIO_SPEED_HIGH;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_PP;
    gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIOB, &gpio_config_struct);
		
		gpio_config_struct.gpio_pin = GPIO_PIN_11;	 			//FPGA_GPIO1
    gpio_config_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_config_struct.gpio_speed = GPIO_SPEED_HIGH;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_PP;
    gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIOB, &gpio_config_struct);
		
		gpio_config_struct.gpio_pin = GPIO_PIN_3;	 			//FPGA select
    gpio_config_struct.gpio_mode = GPIO_MODE_INPUT;
    gpio_config_struct.gpio_speed = GPIO_SPEED_HIGH;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_PP;
    gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIOB, &gpio_config_struct);
		
		gpio_config_struct.gpio_pin = GPIO_PIN_4;	 			//WDOG_GPIO
    gpio_config_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_config_struct.gpio_speed = GPIO_SPEED_HIGH;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_PP;
    gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIOB, &gpio_config_struct);
		
		gpio_bits_set(GPIOB,GPIO_PIN_4);
		delay(10);
		gpio_bits_reset(GPIOB,GPIO_PIN_4);
		
}


/***********************************************************************************
函数功能：SPI GPIO口初始化函数
函数参数：无
函数返回：无
***********************************************************************************/
void spi_gpio_init(void)
{
    gpio_config_t  gpio_config_struct; 

    rcu_ahb_periph_clock_enable_ctrl(RCU_AHB_PERI_PORTA, ENABLE);  
		rcu_ahb_periph_clock_enable_ctrl(RCU_AHB_PERI_PORTB, ENABLE);
    
    // SPI GPIO Config
    gpio_config_struct.gpio_pin = GPIO_PIN_5|GPIO_PIN_7; //PA5 PA7
    gpio_config_struct.gpio_mode = GPIO_MODE_OUTPUT;
    gpio_config_struct.gpio_speed = GPIO_SPEED_HIGH;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_PP;
    gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL; 
    gpio_init(GPIOA, &gpio_config_struct);    
	
		gpio_config_struct.gpio_pin = GPIO_PIN_6;  //  PA6
    gpio_config_struct.gpio_mode = GPIO_MODE_INPUT;  
    gpio_config_struct.gpio_speed = GPIO_SPEED_HIGH;
    gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIOA, &gpio_config_struct);
		
						
    gpio_config_struct.gpio_pin = GPIO_PIN_4;  //  SD_SPI_CS_PIN 
    gpio_config_struct.gpio_mode = GPIO_MODE_OUTPUT;  
    gpio_config_struct.gpio_speed = GPIO_SPEED_HIGH;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_PP;
    gpio_config_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOA, &gpio_config_struct);	
			
    SPI_FLASH_CS_HIGH();
		CLK_LOW();
		MOSI_LOW();
}


/***********************************************************************************
函数功能：USART初始化函数
函数参数：无
函数返回：无
***********************************************************************************/
void usart_config(void)
{ 
    usart_config_t usart_config_struct;
		nvic_config_t  nvic_config_struct;
    gpio_config_t  gpio_config_struct; 
    
    // Clock Config	
    rcu_ahb_periph_clock_enable_ctrl(RCU_AHB_PERI_PORTA, ENABLE);
    rcu_apb1_periph_clock_enable_ctrl(RCU_APB1_PERI_USART2, ENABLE);  
		rcu_apb2_periph_clock_enable_ctrl(RCU_APB2_PERI_USART1, ENABLE);
    
    // GPIO MF Config
    gpio_mf_config(GPIOA, GPIO_PIN_NUM2, GPIO_MF_SEL1);   
    gpio_mf_config(GPIOA, GPIO_PIN_NUM3, GPIO_MF_SEL1);
		gpio_mf_config(GPIOA, GPIO_PIN_NUM9, GPIO_MF_SEL1);   
    gpio_mf_config(GPIOA, GPIO_PIN_NUM10, GPIO_MF_SEL1);
        
    gpio_config_struct.gpio_pin = GPIO_PIN_2 | GPIO_PIN_3;	 //PA2,PA3 USART2
    gpio_config_struct.gpio_mode = GPIO_MODE_MF;
    gpio_config_struct.gpio_speed = GPIO_SPEED_MEDIUM;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_PP;
    gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIOA, &gpio_config_struct);	
	
		gpio_config_struct.gpio_pin = GPIO_PIN_9 | GPIO_PIN_10;	 //PA9,PA10 USART1
    gpio_config_struct.gpio_mode = GPIO_MODE_MF;
    gpio_config_struct.gpio_speed = GPIO_SPEED_MEDIUM;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_PP;
    gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL;
    gpio_init(GPIOA, &gpio_config_struct);
	
    // USART1 Config	
    usart_def_init(USART1);
    usart_config_struct.usart_rate = 115200;
    usart_config_struct.data_width = USART_DATA_WIDTH_8;
    usart_config_struct.stop_bits = USART_STOP_BIT_1;
    usart_config_struct.usart_parity = USART_PARITY_NO;
    usart_config_struct.flow_control = USART_FLOW_CONTROL_NONE;
    usart_config_struct.usart_mode = USART_MODE_TX | USART_MODE_RX;
    usart_init(USART1, &usart_config_struct);
		
		// USART2 Config	
    usart_def_init(USART2);
    usart_config_struct.usart_rate = 115200;
    usart_config_struct.data_width = USART_DATA_WIDTH_8;
    usart_config_struct.stop_bits = USART_STOP_BIT_1;
    usart_config_struct.usart_parity = USART_PARITY_NO;
    usart_config_struct.flow_control = USART_FLOW_CONTROL_NONE;
    usart_config_struct.usart_mode = USART_MODE_TX | USART_MODE_RX;
    usart_init(USART2, &usart_config_struct);
		
		/* Enable the USART Interrupt */
		nvic_config_struct.nvic_IRQ_channel = IRQn_USART1;
		nvic_config_struct.nvic_channel_priority = 0;
		nvic_config_struct.nvic_enable_flag = ENABLE;
		nvic_init(&nvic_config_struct);

    usart_enable_ctrl(USART1,ENABLE);	
		usart_enable_ctrl(USART2,ENABLE);	
		
		usart_interrupt_config(USART1, USART_INT_RXNE, ENABLE); //Enable the USART Receive interrupt
}


/***********************************************************************************
函数功能：TIMER初始化函数
函数参数：无
函数返回：无
***********************************************************************************/
void time_config(void)
{
	//1S中断一次
	timer_config_t  timer_config_struct;
	nvic_config_t   nvic_config_struct;
	uint16_t divider = 0;

	rcu_apb1_periph_clock_enable_ctrl(RCU_APB1_PERI_TIM3, ENABLE); //TIM3 clock enable 
	
	/*TIM3 Configuration: Encoder mode1:
	TIM3 input clock (TIM3CLK) is set to APB1 clock (PCLK1).
	TIM3CLK= PCLK1 = HCLK = SystemCoreClock

	When TIM3 counter clock = 8 MHz,
	divider = (TIM3CLK / TIM3 counter clock) - 1 =  ((SystemCoreClock) /8MHz) - 1

	The encoder mode is encoder mode1: Counter counts up/down on TI2 rising edge 
	depending on TI1 level 
	The Autoreload value is set to 800, so the encoder round is 800 TIM counter clock.*/   

	divider = (uint16_t) ((SystemCoreClock ) / 64000) - 1; 

	// Time base configuration
	timer_config_struct.time_period = 65535;
	timer_config_struct.time_divide = 0;
	timer_config_struct.clock_divide = 0;
	timer_config_struct.count_mode = TIM_COUNT_PATTERN_UP;
	tim_timer_config(TIM3, &timer_config_struct);	
	tim_pdiv_register_config(TIM3, divider, TIM_PDIV_MODE_IMMEDIATE);

	//Enable the TIM3  Interrupt
	nvic_config_struct.nvic_IRQ_channel = IRQn_TIM3;
	nvic_config_struct.nvic_channel_priority = 0;
	nvic_config_struct.nvic_enable_flag = ENABLE;
	nvic_init(&nvic_config_struct);
	
	tim_interrupt_config(TIM3, TIM_INTR_UPDATE, ENABLE); // TIM Interrupts enable 
	
	tim_enable_ctrl(TIM3, ENABLE);
	
}


/***********************************************************************************
函数功能：I2C初始化函数
函数参数：无
函数返回：无
***********************************************************************************/
void adc_i2c_init(void)
{ 
    i2c_config_t  i2c_config_struct;
    gpio_config_t  gpio_config_struct;
	
    rcu_ahb_periph_clock_enable_ctrl(RCU_AHB_PERI_PORTB, ENABLE);
    rcu_apb1_periph_clock_enable_ctrl(RCU_APB1_PERI_I2C1, ENABLE);

    //I2C Pin Config
    gpio_mf_config(GPIOB, GPIO_PIN_NUM6, GPIO_MF_SEL1);  // PB6--I2C_SCL
    gpio_mf_config(GPIOB, GPIO_PIN_NUM7, GPIO_MF_SEL1);  // PB7--I2C_SDA

    gpio_config_struct.gpio_pin = GPIO_PIN_6|GPIO_PIN_7;   
    gpio_config_struct.gpio_mode = GPIO_MODE_MF; gpio_config_struct.gpio_speed = GPIO_SPEED_LOW;
    gpio_config_struct.gpio_out_type = GPIO_OUTPUT_OD;
    gpio_config_struct.gpio_pull = GPIO_PULL_UP;
    gpio_init(GPIOB, &gpio_config_struct);
	
    //I2C configuration 
    i2c_config_struct.mode = I2C_BUS_MODE_I2C;
    i2c_config_struct.analog_flag = I2C_ANALOG_FILTER_ENABLE;
		i2c_config_struct.slave_addr1 = 0x00;
    i2c_config_struct.digital_value = 0x00; 
    i2c_config_struct.ack_flag = I2C_ACK_ENABLE;
    i2c_config_struct.ack_addr = I2C_ACK_ADDR_7BITS;
    i2c_config_struct.tmr_value = 0x00303E5D;   
  
    i2c_init(I2C1, &i2c_config_struct);
    i2c_enable_ctrl(I2C1, ENABLE);
  
}


/***********************************************************************************
函数功能：EXTI初始化函数
函数参数：无
函数返回：无
***********************************************************************************/
void exti0_1_config(void)
{ 
  gpio_config_t gpio_config_struct;
  exti_config_t exti_config_struct;
  nvic_config_t nvic_config_struct;
    
	rcu_ahb_periph_clock_enable_ctrl(RCU_AHB_PERI_PORTB, ENABLE); //Enable GPIOB clock 
	rcu_apb2_periph_clock_enable_ctrl(RCU_APB2_PERI_SYSCFG, ENABLE);//Enable SYSCFG clock

	//Configure PB0 pins as input
	gpio_config_struct.gpio_pin = GPIO_PIN_0;
	gpio_config_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_config_struct.gpio_pull = GPIO_PULL_NO_PULL;
	gpio_init(GPIOB, &gpio_config_struct);

	syscfg_exti_line_config(SYSCFG_EXTI_PORT_PB, SYSCFG_EXTI_PIN_0); //Connect EXTI0 Line to PB0 pin

	//Configure EXTI0 line 
	exti_config_struct.exti_line = EXTI_LINE_0;  
	exti_config_struct.exti_mode = EXTI_MODE_INTR;
	exti_config_struct.exti_trigger = EXTI_TRIGGER_RISING;
	exti_config_struct.exti_line_cmd = ENABLE;
	exti_init(&exti_config_struct);

	//Configure EXTI0line 
	exti_config_struct.exti_line = EXTI_LINE_0;
	exti_init(&exti_config_struct);

	//Enable and set EXTI0_1 Interrupt
	nvic_config_struct.nvic_IRQ_channel = IRQn_EXTI0_1;
	nvic_config_struct.nvic_channel_priority = 0x00;
	nvic_config_struct.nvic_enable_flag = ENABLE;
	nvic_init(&nvic_config_struct);
}


/***********************************************************************************
函数功能：EXTI中断使能函数
函数参数：无
函数返回：无
***********************************************************************************/
void exti0_1_interrupt(enable_state_t enable)
{ 
  nvic_config_t nvic_config_struct;

	//Enable and set EXTI0_1 Interrupt
	nvic_config_struct.nvic_IRQ_channel = IRQn_EXTI0_1;
	nvic_config_struct.nvic_channel_priority = 0x00;
	nvic_config_struct.nvic_enable_flag = enable;
	nvic_init(&nvic_config_struct);
}






