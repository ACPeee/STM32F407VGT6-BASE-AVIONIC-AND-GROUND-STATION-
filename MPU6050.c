#include "MPU6050.h"

extern I2C_HandleTypeDef hi2c2;
extern uint8_t address;
uint8_t found = 0;
uint8_t Data[6];
uint8_t gyro_Data[6];
uint8_t a_Data[1];
mpu6050 mpu;
MPU6050 mpu_filter;
void MPU_SetAccelRange(MPU_Accel_Range range)
{
    uint8_t data = range;

    HAL_I2C_Mem_Write(&hi2c2,MPU_ADRESS,ACCEL_CONFIG_ADRESS,I2C_MEMADD_SIZE_8BIT,&data,1,100);
}

void MPU_SetGyroRange(MPU_Gyro_Range range)
{
    uint8_t data = range;

    HAL_I2C_Mem_Write(&hi2c2,MPU_ADRESS,GYRO_CONFIG_ADRESS,I2C_MEMADD_SIZE_8BIT,&data,1,100);
}

void MPU_who_am_i()
{
	for(address = 1; address < 128; address++)
{
    if(HAL_I2C_IsDeviceReady(&hi2c2, address<<1, 1, 10) == HAL_OK)
    {
        found = address;
        break;
    }
	}
}
void MPU_SetDLPF(dlpf_value value){
	HAL_I2C_Mem_Write(&hi2c2,MPU_ADRESS,LOWPASS_ADRESS,I2C_MEMADD_SIZE_8BIT,&value,1,100);
}
void MPU_init()
{
    uint8_t data = 0;

    HAL_I2C_Mem_Write(&hi2c2, MPU_ADRESS, 0x6B, 1, &data, 1, 100);
}

void MPU_accel_calc()
{
    HAL_I2C_Mem_Read(&hi2c2,found<<1,ACCEL_XOUT_H,1,Data,6,100);

    mpu.accel_x = (Data[0] << 8) | Data[1];
    mpu.accel_y = (Data[2] << 8) | Data[3];
    mpu.accel_z = (Data[4] << 8) | Data[5];
}
void MPU_gyro_calc()
{
    HAL_I2C_Mem_Read(&hi2c2,found<<1,GYRO_XOUT_H,1,gyro_Data,6,100);

    mpu.gyro_x = (gyro_Data[0] << 8) | gyro_Data[1];
    mpu.gyro_y = (gyro_Data[2] << 8) | gyro_Data[3];
    mpu.gyro_z = (gyro_Data[4] << 8) | gyro_Data[5]; 
}
float Value_LowPass_Filter(float old_value, float new_value)
{
    float alpha = 0.9;

    return (old_value * alpha) + (new_value * (1 - alpha));
}

void MPU6050_General_Calc()
{
    mpu_filter.accel_x_filter = Value_LowPass_Filter(mpu_filter.accel_x_filter, mpu.accel_x);
    mpu_filter.accel_y_filter = Value_LowPass_Filter(mpu_filter.accel_y_filter, mpu.accel_y);
    mpu_filter.accel_z_filter = Value_LowPass_Filter(mpu_filter.accel_z_filter, mpu.accel_z);

    mpu_filter.gyro_x_filter = Value_LowPass_Filter(mpu_filter.gyro_x_filter, mpu.gyro_x);
    mpu_filter.gyro_y_filter = Value_LowPass_Filter(mpu_filter.gyro_y_filter, mpu.gyro_y);
    mpu_filter.gyro_z_filter = Value_LowPass_Filter(mpu_filter.gyro_z_filter, mpu.gyro_z);
}
