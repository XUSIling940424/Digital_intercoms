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

	spiConfig.length = SPI_DATA_LENGTH_8B;   //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	spiConfig.baudrateDiv = SPI_BAUDRATE_DIV_4;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	spiConfig.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	spiConfig.firstBit = SPI_FIRSTBIT_MSB;//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	spiConfig.mode = SPI_MODE_MASTER;  //����SPI�Ĺ���ģʽ
	spiConfig.polarity = SPI_CLKPOL_HIGH;  //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	spiConfig.nss = SPI_NSS_SOFT;//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	spiConfig.phase = SPI_CLKPHA_2EDGE;
	spiConfig.crcPolynomial = 7;//CRCֵ����Ķ���ʽ
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

