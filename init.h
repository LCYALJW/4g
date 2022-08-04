#ifndef _ECR8660_INIT_H_
#define _ECR8660_INIT_H_
#include "cs32f0xx.h"

void gpio_config(void);
void spi_gpio_init(void);
void usart_config(void);
void time_config(void);
void adc_i2c_init(void);
void exti0_1_config(void);
void exti0_1_interrupt(enable_state_t enable);
void spi_gpio_init(void);

#endif 



