#ifndef __BSP_DS18B20_H__
#define __BSP_DS18B20_H__

#include <rthw.h>
#include <rtdevice.h>
#include <board.h>

/**
 * @brief  Sets pin(s) low
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin low
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them low
 * @retval None
 */
#define GPIO_SetPinLow(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (uint32_t)GPIO_Pin << 16U)
/**
 * @brief  Sets pin(s) high
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin high
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them high
 * @retval None
 */
#define GPIO_SetPinHigh(GPIOx, GPIO_Pin) 		((GPIOx)->BSRR = (GPIO_Pin))
/**
 * @brief  Gets input data bit
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read input bit value
 * @param  GPIO_Pin: GPIO pin where you want to read value
 * @retval 1 in case pin is high, or 0 if low
 */
#define GPIO_GetInputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->IDR & (GPIO_Pin)) == 0 ? 0 : 1)


/* Pin settings */
#define ONEWIRE_LOW(structure)			GPIO_SetPinLow((structure)->GPIOx, (structure)->GPIO_Pin)
#define ONEWIRE_HIGH(structure)			GPIO_SetPinHigh((structure)->GPIOx, (structure)->GPIO_Pin)
#define ONEWIRE_INPUT(structure)		
#define ONEWIRE_OUTPUT(structure)	

/**
 * @brief  OneWire working struct
 * @note   Except ROM_NO member, everything is fully private and should not be touched by user
 */
typedef struct {
	GPIO_TypeDef* GPIOx;           /*!< GPIOx port to be used for I/O functions */
	uint16_t GPIO_Pin;             /*!< GPIO Pin to be used for I/O functions */
	uint8_t LastDiscrepancy;       /*!< Search private */
	uint8_t LastFamilyDiscrepancy; /*!< Search private */
	uint8_t LastDeviceFlag;        /*!< Search private */
	uint8_t ROM_NO[8];             /*!< 8-bytes address of last search device */
} OneWire_t;


extern OneWire_t OneWire1;

rt_uint16_t ds18b20_read_reg(OneWire_t* OneWire);
rt_uint8_t ds18b20_read_ID(OneWire_t* OneWire);
int ds18b20_hw_init(void);

#endif
