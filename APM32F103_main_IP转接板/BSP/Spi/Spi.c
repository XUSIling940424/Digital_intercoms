#include "Spi.h"

void SPI1_GPIO_Configuration(void)
{
	GPIO_Config_T gpioConfig;	
	
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA);
  /* config PIN_5->SCK , PIN_7->MOSI PIN_6  MISO*/
	gpioConfig.pin =  GPIO_PIN_5 |  GPIO_PIN_6 | GPIO_PIN_7;
	gpioConfig.mode = GPIO_MODE_AF_PP;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	GPIO_Config(GPIOA, &gpioConfig);
	
	/* config PIN_4  CS*/
	gpioConfig.pin =  GPIO_PIN_4 ;
	gpioConfig.mode = GPIO_MODE_OUT_PP;
	gpioConfig.speed = GPIO_SPEED_10MHz;
	GPIO_Config(GPIOA, &gpioConfig);	
}


void Spi1_Config (void)
{
	SPI_Config_T spiConfig;
	
	RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SPI1);	
	
	SPI_ConfigStructInit(&spiConfig);

	spiConfig.length = SPI_DATA_LENGTH_8B;   //设置SPI的数据大小:SPI发送接收8位帧结构
	spiConfig.baudrateDiv = SPI_BAUDRATE_DIV_4;//定义波特率预分频的值:波特率预分频值为256
	spiConfig.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	spiConfig.firstBit = SPI_FIRSTBIT_MSB;//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	spiConfig.mode = SPI_MODE_MASTER;  //设置SPI的工作模式
	spiConfig.polarity = SPI_CLKPOL_HIGH;  //串行同步时钟的空闲状态为高电平
	spiConfig.nss = SPI_NSS_SOFT;//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	spiConfig.phase = SPI_CLKPHA_2EDGE;
	spiConfig.crcPolynomial = 7;//CRC值计算的多项式
	/*  SPI config  */
	SPI_Config(SPI1, &spiConfig);

	SPI_ConfigDataSize(SPI1, SPI_DATA_LENGTH_8B);

	SPI_Enable(SPI1);	
}


void cs_high(void)
{
  GPIO_SetBit(GPIOA,GPIO_PIN_4);
}

void cs_low(void)
{
  GPIO_ResetBit(GPIOA,GPIO_PIN_4);
}


u8 spi_read_send_byte(u8 byte)
{
  while (SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_TXBE) == RESET);
  SPI_I2S_TxData(SPI1, byte);
  while (SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_RXBNE) == RESET);	
  return SPI_I2S_RxData(SPI1);
}

