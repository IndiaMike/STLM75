/*
 * stlm75.c
 *
 *  Created on: 24 lis 2022
 *      Author: Programista
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

void STLM75_Init(STLM75_t *stlm, I2C_HandleTypeDef* stlm75_i2c, uint8_t address)
{
	stlm->stlm75_i2c = stlm75_i2c;

	uint8_t tmp = 0b10010001;
	tmp |=(address & 0x07) <<1;
	stlm->adress = tmp;
}

void STLM75_SetMode(STLM75_t *stlm, uint8_t mode)
{
	if(mode > 1) return;
	uint8_t tmp = 0x00;
	tmp = STLM75_Read8(stlm, STLM75_REG_CONFIGURATION);
	tmp &=0xFE;
	tmp |=(mode & 0x01);
	STLM75_Write8(stlm, STLM75_REG_CONFIGURATION, tmp);
}

int8_t STLM75_SetTepmerature(STLM75_t *stlm, uint8_t mem_address, int8_t temperature)
{
	if(mem_address != STLM75_REG_HYSTERESIS && mem_address != STLM75_REG_TEMP_OVERLIMIT) return -1;

	int8_t temp[2];
	temp[0] = temperature;
	temp[1] = 0;

	STLM75_Write16(stlm, mem_address, temp);

	return 0;
}

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

float STLM75_ReadTemperature(STLM75_t *stlm)
{
	float temp=0.0;
	int16_t x=STLM75_Read16(stlm, STLM75_REG_TEMPERATURE);

	temp = (float)(x>>7);

	return (temp * 0.5f);
}
