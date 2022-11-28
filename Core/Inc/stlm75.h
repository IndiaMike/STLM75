/*
 * @file	stlm75.h
 * @created	on: 24 lis 2022
 * @author:	Grzegorz S
 * 			github.com/IndiaMike
 * @brief	STLM75 thermometer library DEFINITIONS and FUNCTIONS PROTOTYPES.
 */

#ifndef INC_STLM75_H_
#define INC_STLM75_H_


#define STLM75_TIMEOUT 10	/*!< timeout for blocking functions */

//registers
#define	STLM75_REG_TEMPERATURE			0x00
#define	STLM75_REG_CONFIGURATION		0x01
#define	STLM75_REG_HYSTERESIS			0x02
#define	STLM75_REG_TEMP_OVERLIMIT		0x03

//mode
#define STLM75_MODE_SHUTDOWN			0x01
#define STLM75_MODE_NORMAL				0x00

//fault tolerance
#define STLM75_ALARM_FT_1				0x00
#define STLM75_ALARM_FT_2				0x01
#define STLM75_ALARM_FT_4				0x02
#define STLM75_ALARM_FT_6				0x03



typedef struct
{
	I2C_HandleTypeDef* 	stlm75_i2c;
	uint8_t				adress;
}STLM75_t;


void STLM75_Init(STLM75_t *stlm, I2C_HandleTypeDef* stlm75_i2c, uint8_t address);
void STLM75_SetMode(STLM75_t *stlm, uint8_t mode);
int8_t STLM75_SetTepmerature(STLM75_t *stlm, uint8_t mem_address, int8_t temperature);
int8_t STLM75_SetFaultTolerance(STLM75_t *stlm, uint8_t consecutive_faults);
float STLM75_ReadTemperature(STLM75_t *stlm);



#endif /* INC_STLM75_H_ */
