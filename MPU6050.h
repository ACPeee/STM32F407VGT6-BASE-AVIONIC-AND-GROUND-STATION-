#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f4xx_hal.h"
#include "main.h"

void MPU_who_am_i(void);
void MPU_init(void);
void MPU_gyro_calc(void);
void MPU_accel_calc(void);
void MPU_SetGyroRange(uint8_t);
void MPU_SetAccelRange(uint8_t);
extern uint8_t MPU_state;
extern uint8_t mpu_ID;
void MPU_SetDLPF(uint8_t);
float Value_LowPass_Filter(float,float);
void MPU6050_General_Calc(void);
#define WHO_AM_I 0x75
#define MPU_ADRESS (0x68 << 1)
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define ACCEL_CONFIG_ADRESS 0x1C
#define GYRO_CONFIG_ADRESS 0x1B
#define LOWPASS_ADRESS 0x1A
typedef enum{
    MPU_ACCEL_2G = 0x00,
    MPU_ACCEL_4G = 0x08,
    MPU_ACCEL_8G = 0x10,
    MPU_ACCEL_16G = 0x18
} MPU_Accel_Range;
typedef enum{
    MPU_GYRO_250 = 0x00,
    MPU_GYRO_500 = 0x08,
    MPU_GYRO_1000 = 0x10,
    MPU_GYRO_2000 = 0x18
} MPU_Gyro_Range;
typedef enum{
    dlpf_250Hz = 0x00,
    dlpf_185Hz = 0x01,
    dlpf_95Hz = 0x02,
    dlpf_43Hz = 0x03,
		dlpf_20Hz = 0x04,
		dlpf_10Hz = 0x05,
		dlpf_5Hz = 0x06,
} dlpf_value;
typedef struct {
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
}mpu6050;
typedef struct{
	float accel_x_filter;
	float accel_y_filter;
	float accel_z_filter;
	float gyro_x_filter;
	float gyro_y_filter;
	float gyro_z_filter; 
}MPU6050;
extern mpu6050 mpu;
extern MPU6050 mpu_filter;
#endif
