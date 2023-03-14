#ifndef  __SPI_H__
#define  __SPI_H__

#include <rtthread.h>
#include "Board.h"
#include "apm32f10x.h"
#include "apm32f10x_spi.h"

void SPI1_GPIO_Configuration(void);
void Spi1_Config (void);
void cs_high(void);
void cs_low(void);
u8 spi_read_send_byte(u8 byte);



#endif
