#include "spi_adc.h"
#include "cs32f0xx_gpio.h"
#include "ECR8660.h"
#include <string.h>
#include "systick.h"
#include "command.h"
#include "common.h"

#define DELAY_TIME 							 10

#define PEAK_READ() 						 gpio_input_data_bit_read(GPIOB,GPIO_PIN_10)		//读中断管脚

#define SPI_FPGA()							 gpio_bits_reset(GPIOB,GPIO_PIN_11)						//配置FPGA
#define SPI_FLASH_CS_LOW()       gpio_bits_reset(GPIOA, GPIO_PIN_4)
#define SPI_FLASH_CS_HIGH()      gpio_bits_set(GPIOA, GPIO_PIN_4)
#define CLK_LOW()       				 gpio_bits_reset(GPIOA, GPIO_PIN_5)          
#define CLK_HIGH()       				 gpio_bits_set(GPIOA, GPIO_PIN_5)
#define MOSI_LOW()       				 gpio_bits_reset(GPIOA, GPIO_PIN_7)
#define MOSI_HIGH()       			 gpio_bits_set(GPIOA, GPIO_PIN_7)
#define MISO_READ() 						 gpio_input_data_bit_read(GPIOA,GPIO_PIN_6)

PEAK_T peak_struct;								//最终得出的峰值结构体
PEAK_T peak_ts;										//临时峰值结构体
unsigned char peak_state = 0;
unsigned int lock_state = 0;			//AD RX锁住标志
unsigned char sync_state = 0;			//同步类型标志位

PEAK_T peak_4g ={
	0,
	2690000,
	0,
};										//4g峰值结构体

extern unsigned char freq_count;
extern unsigned char fpga_select;
extern unsigned int time_out;
extern unsigned int scan_start_freq;
extern unsigned int scan_end_freq;
extern unsigned int freq_default[5];

void default_freq_select(unsigned char freq)
{
	switch(freq){
		case 0:
				printf("scan freq : 1885-1915 khz\r\n");
				scan_start_freq = 1885000;
				scan_end_freq = 1915000;
				peak_4g.freq = 1894800;
			break;
		case 1:
				printf("scan freq : 2010-2025 khz\r\n");
				scan_start_freq = 2010000;
				scan_end_freq = 2025000;
				peak_4g.freq = 2020000;
			break;
		case 2:
				printf("scan freq : 2300-2390 khz\r\n");
				scan_start_freq = 2300000;
				scan_end_freq = 2390000;
				peak_4g.freq = 2385700;
			break;
		case 3:
				printf("scan freq : 2575-2655 khz\r\n");
				scan_start_freq = 2575000;
				scan_end_freq = 2655000;
				peak_4g.freq = 2644500;
			break;
		case 4:
				printf("scan freq : 2515-2575 khz\r\n");
				scan_start_freq = 2515000;
				scan_end_freq = 2575000;
				peak_4g.freq = 2567100;
			break;
	}
	if((freq_default[freq_count] <= scan_end_freq) && (freq_default[freq_count] >= scan_start_freq)){
		peak_4g.freq = freq_default[freq_count];
	}
}

unsigned char freq_set(unsigned int freq)
{
	unsigned char ret = 0;
	unsigned int rx_state = 0;
	
	if(Time_Out(1));
	else {
		printf("AD is busy\r\n");
		ret = 1;
	}
	
	ECR8660_write(1,ECR8660_ecternal_register, 0x20004008, freq);
	
	if(Time_Out(1));
	else {
		printf("AD is busy\r\n");
		ret = 1;
	}
					
	ECR8660_write(1,ECR8660_ecternal_register, 0x00201180, 0x00000008);
	
	delay(DELAY_TIME);
	
	//printf("freq config complate\r\n");
	
	if(Time_Out(1)); 
	else {
		printf("AD is busy\r\n");
		ret = 1;
	}
		
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
		printf("AD1 RX is not lock\r\n");
		ret = 1;
	}
	
	return ret;
}


/***********************************************************************************
函数功能：延时函数
函数参数：延时时间，代表多少个nop
函数返回：无
***********************************************************************************/
void peak_delay(unsigned int time)
{
	unsigned int time_count = 0;
	while(!(PEAK_READ()) && (time_count < time)){
		time_count++;
		__nop();
	}
	if(time_count < time){
		peak_state = 1;
	}else{
		peak_state = 0;
		printf("peak io read time out\r\n");
	}
}


/***********************************************************************************
函数功能：比较
函数参数：延时时间，代表多少个nop
函数返回：峰值结构体
***********************************************************************************/
PEAK_T peak_compare(void)
{
	PEAK_T peak_value;
	unsigned char i = 0;
	unsigned int peak[12];
	unsigned int peak_average[3];
	
	memset(&peak_value,0,sizeof(PEAK_T));
	
	//读取
	for(i = 7; i < 19 ; i++){
		peak[i-7] = spi_read(i);
	}
	
	for(i = 0; i < 3; i++){
		peak_average[i] =  (peak[(4*i)+0] +  peak[(4*i)+1] + peak[(4*i)+2] + peak[(4*i)+3])/4;
		//if(peak_average[i])
			//printf("peak average : %d , value: %d\r\n",i,peak_average[i]);
	}
	
	for(i = 0; i < 3; i++){
		if(peak_average[i] > peak_value.peak){
			peak_value.peak = peak_average[i];
			peak_value.nid2 = i;
		}
	}
	
	return peak_value;
}


/***********************************************************************************
函数功能：根据频点计算频率函数
函数参数：频点
函数返回：频率
***********************************************************************************/
double compute_freq_from_gscn(int gscn){
	int M,N;
	double freq;
	if (gscn < 7499){
		for (int i = 0; i < 3; i ++){
			M = 2 * i + 1;
			if ((gscn - (M - 3)/2) % 3 == 0){
			break;
			}
		}                  
	N = (gscn - (M - 3)/2) / 3;
	
	freq = N * 1.2e6 + M * 50e3;
	
	}else if (gscn < 22256){
		N = gscn - 7499;
		freq = 3e9 + N * 1.44e6;
		
	}else {
	N = gscn - 22256;
	freq = 24250.08e6 + N * 17.28e6;
	}
	return freq;
}


/***********************************************************************************
函数功能：FPGA写spi函数
函数参数：addr->要写的地址,snd_data->要写的数据
函数返回：无
***********************************************************************************/
void spi_write(unsigned char addr,unsigned int send_data)
{
	unsigned char i = 0;
	SPI_FPGA();
	//SPI_1_SET();
	
	CLK_HIGH();
	
	__nop();
	__nop();
	
	SPI_FLASH_CS_LOW();
	
	CLK_LOW();
	
	MOSI_LOW();

	__nop();
	__nop();
		
	CLK_HIGH();
		
	__nop();
	__nop();
	
	//写入8位地址
	for(i = 0; i < 7; i++){
		
		CLK_LOW();
		
		if(addr & (0x40 >> i)){
			MOSI_HIGH();
		}else{
			MOSI_LOW();
		}
		
		__nop();
		__nop();
		
		CLK_HIGH();
		
		__nop();
		__nop();
		
	}
	
	//写入24位数据
	for(i = 0; i < 24; i++){
		
		CLK_LOW();
		
		if(send_data & (0x800000 >> i)){
			MOSI_HIGH();
		}else{
			MOSI_LOW();
		}
		
		__nop();
		__nop();
		
		CLK_HIGH();
		
		__nop();
		__nop();
		
	}
	
	SPI_FLASH_CS_HIGH();
	CLK_LOW();
	MOSI_LOW();
	
	__nop();
	__nop();
	__nop();
	__nop(); 
}


/***********************************************************************************
函数功能：FPGA读spi函数
函数参数：addr->要写的地址
函数返回：读到的spi数据
***********************************************************************************/
unsigned int spi_read(unsigned char addr)
{
	unsigned char i = 0;
	unsigned int read_data = 0;
	
	SPI_FPGA();
	//SPI_1_SET();
	
	CLK_HIGH();
	
	__nop();
	__nop();
	
	
	SPI_FLASH_CS_LOW();
	
	CLK_LOW();
	
	MOSI_HIGH();

	__nop();
	__nop();
		
	CLK_HIGH();
		
	__nop();
	__nop();
	
	//写入8位地址
	for(i = 0; i < 7; i++){
		
		CLK_LOW();
		
		if(addr & (0x40 >> i)){
			MOSI_HIGH();
		}else{
			MOSI_LOW();
		}
		
		__nop();
		__nop();
		
		CLK_HIGH();
		
		__nop();
		__nop();
		
	}
	
	//读24位数据
	for(i = 0; i < 24; i++){
		
		CLK_LOW();
	
		__nop();
		__nop();
		
		CLK_HIGH();	
		
		if(MISO_READ()){
			read_data |= (0x1 << (23 - i));
		}
		
		__nop();
		__nop();
		
	}
	
	SPI_FLASH_CS_HIGH();
	CLK_LOW();
	MOSI_LOW();
	
	__nop();
	__nop();
	__nop();
	__nop(); 
	
	return read_data;
}

/***********************************************************************************
函数功能：FPGA扫频函数
函数参数：无
函数返回：是否超时,0为不超时，1为超时
***********************************************************************************/
unsigned char Sweep_Frequency(void)
{
	unsigned char ret = 0;
	unsigned int gscn_number = 0;
	unsigned int freq_4g = 0;
	double freq = 0.0;	 
	
	//2.6频段扫频
		memset(&peak_struct,0,sizeof(peak_struct));
		memset(&peak_ts,0,sizeof(peak_ts));
		spi_write(0x16,1); //4G扫频
		sync_state = 1;
		
		default_freq_select(freq_count);
		
		printf("2.6 4G sweep default freq start\r\n");
		
		//设置默认频点
		if(freq_set(peak_4g.freq))
			ret = 1;
		
		//主同步使能
		spi_write(MASTER_SYNC,0x1);
		spi_write(MASTER_SYNC,0x0);
		
		//等待IO口脉冲
		while(!(PEAK_READ()));
		
		//确定最高峰值频点
		peak_ts = peak_compare();
		
		if(peak_ts.peak > 50){
			//主同步使能
			spi_write(MASTER_SYNC,0x1);
			spi_write(MASTER_SYNC,0x0);
			peak_ts.freq = peak_4g.freq;
			memcpy(&peak_4g,&peak_ts,sizeof(PEAK_T));
			spi_write(NID2,peak_4g.nid2);
			
			delay(2000);
		
			//扫频完成
			spi_write(FREQ_COMPLATE,0x1);
			printf("freq : %0.4f khz, peak : %d, nid2 : %d\r\n", peak_4g.freq,peak_4g.peak,peak_4g.nid2);
		}else{
			printf("2.6 4G sweep loop freq start\r\n");
#if TEST_MODE
		delay(10000);
#endif
			for(freq_4g = scan_start_freq;freq_4g <= scan_end_freq;freq_4g+=100){
			
				//printf("------------------------------\r\n");
				//printf("4g freq : %d khz\r\n",freq_4g);
				
				//设置频点
				if(freq_set(freq_4g))
					ret = 1;
				
				//主同步使能
				spi_write(MASTER_SYNC,0x1);
				spi_write(MASTER_SYNC,0x0);
				
				//等待IO口脉冲
				while(!(PEAK_READ()));
				
				//确定最高峰值频点
				peak_ts = peak_compare();
				
				peak_ts.freq = freq_4g;
				
				if((peak_struct.peak < peak_ts.peak) && (peak_ts.peak > 40)){
					memcpy(&peak_struct,&peak_ts,sizeof(PEAK_T));
					memcpy(&peak_4g,&peak_ts,sizeof(PEAK_T));
					printf("freq : %0.4f khz, peak : %d, nid2 : %d\r\n", peak_struct.freq,peak_struct.peak,peak_struct.nid2);
				}
				
			}
			
			if(peak_struct.peak == 0){
					ret = 2;
			}else{
				//设置频点
				if(freq_set(peak_struct.freq))
					ret = 1;

				//主同步使能
				spi_write(MASTER_SYNC,0x1);
				spi_write(MASTER_SYNC,0x0);
				
				spi_write(NID2,peak_struct.nid2);
				
				delay(2000);
		
				//扫频完成
				spi_write(FREQ_COMPLATE,0x1);
				
				printf("freq : %0.4f khz, peak : %d, nid2 : %d\r\n", peak_struct.freq,peak_struct.peak,peak_struct.nid2);
			}
			
		}
	
	
	
	
	return ret;
}


