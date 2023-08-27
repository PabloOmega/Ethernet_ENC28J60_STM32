
#include "enc28_spi.h"

extern SPI_HandleTypeDef hspi1;

uint8_t ETH_SPI_READ8(){
	uint8_t v[1];
	//if(HAL_SPI_Receive(&hspi1,v,1,100) != HAL_OK) return 0;
	HAL_SPI_Receive(&hspi1,v,1,100);
	return v[0];
}

void ETH_SPI_WRITE8(uint8_t a){
	uint8_t v[1];
	v[0] = a;
	//if(HAL_SPI_Transmit(&hspi1,v,1,100) != HAL_OK) while(1);
	HAL_SPI_Transmit(&hspi1,v,1,100);
}