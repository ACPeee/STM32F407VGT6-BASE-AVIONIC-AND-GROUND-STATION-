#ifndef __BMP180_H
#define __BMP180_H

#include "stm32f4xx_hal.h"
#include "main.h"

#define BMP180_ADDR (0x77 << 1)
#define OVER_SAMPLING_REGISTER 0xF4
#define AC1_MSB_ADRESS 0xAA
void BMP_Calculate(void);
void BMP_Config(uint8_t);
void BMP_ReadTemp(void);
void BMP_ReadPressure(void);
float Read_Temperature(void);
float BMP_GetAltitude(void);
void ReadAll(void);
float LowPassFilter(float, float, float);

typedef enum{
    oss0 = 0,
    oss1 = 1,
    oss2 = 2,
    oss3 = 3
}BMP_os_value;

typedef enum{
    oss0_delay = 5,
    oss1_delay = 8,
    oss2_delay = 14,
    oss3_delay = 26
}oss_delay;
// Structlar
typedef struct {
    int16_t AC1;
    int16_t AC2;
    int16_t AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t B1;
    int16_t B2;
    int16_t MB;
    int16_t MC;
    int16_t MD;
}bmp180;

typedef struct{
    int32_t UT;
    int32_t UP;
    int32_t X1;
    int32_t X2;
    int32_t B5;
    int32_t T;
}TEMP;

typedef struct {
    int32_t B6;
    int32_t X1;
    int32_t X2;
    int32_t X3;
    int32_t B3;
    uint32_t B4;
    uint32_t B7;
    int32_t p;
}pressure;

extern bmp180 bmp;
extern TEMP temp;
extern pressure press;
#endif
