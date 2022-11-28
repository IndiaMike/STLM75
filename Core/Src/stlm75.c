/*
 * @file	stlm75.c
 * @created	on: 24 lis 2022
 * @author:	Grzegorz S
 * 			github.com/IndiaMike
 * @brief	STLM75 thermometer library.
 * 			This is simple library to use STLM75 thermometer.
 * 			Not all functionals are implemented.
 * 			1. Create	STLM75_t structure
 * 			2. Call		STLM75_Init(,,)
 */
#include "main.h"
#include "stlm75.h"


static uint8_t	STLM75_Read8(STLM75_t *stlm, uint8_t Register)
{
	uint8_t Value;

	HAL_I2C_Mem_Read(stlm->stlm75_i2c, stlm->adress, Register, 1, &Value, 1, STLM75_TIMEOUT);

	return Value;
}

static uint16_t STLM75_Read16(STLM75_t *stlm, uint8_t Register)
{
	uint8_t Value[2];

	if(HAL_ERROR!=HAL_I2C_Mem_Read(stlm->stlm75_i2c, stlm->adress, Register, 1, Value, 2, STLM75_TIMEOUT))
		return (Value[0]<<8 | Value[1]);
	else
		return 0x8000;
}

static uint8_t STLM75_Write8(STLM75_t *stlm, uint8_t Register, uint8_t Value)
{
	HAL_I2C_Mem_Write(stlm->stlm75_i2c,stlm->adress,Register,1,&Value,1,STLM75_TIMEOUT);
	return 0;
}

static uint8_t STLM75_Write16(STLM75_t *stlm, uint8_t Register, uint8_t* Value)
{
	HAL_I2C_Mem_Write(stlm->stlm75_i2c, stlm->adress, Register, 1, Value, 2, STLM75_TIMEOUT);
	return 0;
}

/**
 *  Initializing function.
 * 	Function to fill the sensor structure.
 *
 *  \param[in,out] *stlm	pointer to sensor structure.
 *  \param[in] stlm75_i2c	i2c handler
 *  \param[in]	address		device address set on the hardware (range 0 - 7). (configured by Pull-up / down resistors A0,A1,A2, see datasheet)
 */
void STLM75_Init(STLM75_t *stlm, I2C_HandleTypeDef* stlm75_i2c, uint8_t address)
{
	stlm->stlm75_i2c = stlm75_i2c;

	uint8_t tmp = 0b10010001;
	tmp |=(address & 0x07) <<1;
	stlm->adress = tmp;
}

/**
 * Mode change (Shutdown, Normal)
 * This function is used to put the device to shutdown (low-power) or normal mode.
 *
 * \param[in] *stlm	Pointer to sensor structure.
 * \param[in] mode	Mode to be set (use macro STLM75_MODE_SHUTDOWN or STLM75_MODE_NORMAL).
 */
void STLM75_SetMode(STLM75_t *stlm, uint8_t mode)
{
	if(mode > 1) return;
	uint8_t tmp = 0x00;
	tmp = STLM75_Read8(stlm, STLM75_REG_CONFIGURATION);
	tmp &=0xFE;
	tmp |=(mode & 0x01);
	STLM75_Write8(stlm, STLM75_REG_CONFIGURATION, tmp);
}

/**
 * Set termostat temperature.
 * Use to set Overlimit temperature or Hysteresis temperature. Only INT value
 *
 * \param[in] *stlm			Pointer to sensor structure.
 * \param[in] mem_address	To select which temperature we want to set (Overlimit or Hysteresis).
 * 							Use macro STLM75_REG_HYSTERESIS or STLM75_REG_TEMP_OVERLIMIT.
 * \param[in] temperature 	Temperature value to set. (Device allows to set temp with 0.5 deg accuracy
 * 							but only int value possibility on this function).
 */
int8_t STLM75_SetTepmerature(STLM75_t *stlm, uint8_t mem_address, int8_t temperature	)
{
	if(mem_address != STLM75_REG_HYSTERESIS && mem_address != STLM75_REG_TEMP_OVERLIMIT) return -1;

	int8_t temp[2];
	temp[0] = temperature;
	temp[1] = 0;

	STLM75_Write16(stlm, mem_address, temp);

	return 0;
}

/**
 * Set Fault Tolerance
 * Use to set FaultTolerance (see datasheet table 2) How many measurement are take into account
 * for thermal alarm function to minimize noise.
 *
 * \param[in] *stlm					Pointer to sensor structure.
 * \param[in] consecutive_faults 	(0-3). Use macro STLM75_ALARM_FT_x (x = 1, 2, 4 or 6)
 * \retval error status
 */
int8_t STLM75_SetFaultTolerance(STLM75_t *stlm, uint8_t consecutive_faults)
{
	if(consecutive_faults > 3) return -1;	// parameter error value

	uint8_t tmp = 0x00;
	tmp = STLM75_Read8(stlm, STLM75_REG_CONFIGURATION);
	tmp &=0xE7;
	tmp |=((consecutive_faults << 3) & 0x18);
	STLM75_Write8(stlm, STLM75_REG_CONFIGURATION, tmp);
	return 0;
}

/**
 * Read temperature from STLM75 sensor.
 * This function read the temperature from STLM75 sensor
 * In case of miscommunication returns -128*C
 *
 * \param[in] *stlm		Pointer to sensor structure.
 * \retval Temperatre with 0.5 accuracy in Celsius
 */
float STLM75_ReadTemperature(STLM75_t *stlm)
{
	float temp=0.0;
	int16_t x=STLM75_Read16(stlm, STLM75_REG_TEMPERATURE);

	temp = (float)(x>>7);

	return (temp * 0.5f);
}
