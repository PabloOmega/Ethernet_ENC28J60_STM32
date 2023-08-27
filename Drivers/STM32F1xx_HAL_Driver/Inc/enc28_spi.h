
#include "stm32f1xx_hal.h"

#define ETH_PORT  GPIOA

#define ETH_CS    GPIO_PIN_4   //PORTA
#define ETH_IRQ   GPIO_PIN_3   //PORTA

#define ETH_NCS_LOW() 	HAL_GPIO_WritePin(ETH_PORT,ETH_CS,GPIO_PIN_RESET)
#define ETH_NCS_HIGH()  HAL_GPIO_WritePin(ETH_PORT,ETH_CS,GPIO_PIN_SET)

#define ETH_IRQ_LOW()   ((HAL_GPIO_ReadPin(ETH_PORT,ETH_CS))?1:0)

uint8_t ETH_SPI_READ8(void);
void ETH_SPI_WRITE8(uint8_t a);