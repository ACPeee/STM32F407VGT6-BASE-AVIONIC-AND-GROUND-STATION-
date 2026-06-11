#include "BMP180.h"
#include "math.h"

extern I2C_HandleTypeDef hi2c3;

// PV values
uint8_t value = 0;
uint8_t address;
uint8_t data[22];
uint8_t current_delay;
uint8_t current_oss;

extern float altitude;
extern float temperature;
extern float pressure_bmp;
float filtered_pressure = 0;
float filtered_altitude = 0;

bmp180 bmp;
TEMP temp;
pressure press;

// -------- CONFIG --------
void BMP_Config(BMP_os_value value){
    current_oss = value;

    switch(current_oss)
    {
        case oss0: current_delay = oss0_delay; break;
        case oss1: current_delay = oss1_delay; break;
        case oss2: current_delay = oss2_delay; break;
        case oss3: current_delay = oss3_delay; break;
    }
}

// -------- CALIBRATION --------
void BMP_Calculate(){
    HAL_I2C_Mem_Read(&hi2c3, BMP180_ADDR, AC1_MSB_ADRESS, I2C_MEMADD_SIZE_8BIT, data, 22, 100);

    bmp.AC1 = (data[0] << 8) | data[1];
    bmp.AC2 = (data[2] << 8) | data[3];
    bmp.AC3 = (data[4] << 8) | data[5];
    bmp.AC4 = (data[6] << 8) | data[7];
    bmp.AC5 = (data[8] << 8) | data[9];
    bmp.AC6 = (data[10] << 8) | data[11];
    bmp.B1  = (data[12] << 8) | data[13];
    bmp.B2  = (data[14] << 8) | data[15];
    bmp.MB  = (data[16] << 8) | data[17];
    bmp.MC  = (data[18] << 8) | data[19];
    bmp.MD  = (data[20] << 8) | data[21];
}

// -------- TEMPERATURE --------
void BMP_ReadTemp(){
    uint8_t cmd = 0x2E;

    HAL_I2C_Mem_Write(&hi2c3, BMP180_ADDR, 0xF4, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 100);
    HAL_Delay(5);

    uint8_t temp_data[2];
    HAL_I2C_Mem_Read(&hi2c3, BMP180_ADDR, 0xF6, I2C_MEMADD_SIZE_8BIT, temp_data, 2, 100);

    temp.UT = (temp_data[0] << 8) | temp_data[1];

    temp.X1 = ((temp.UT - bmp.AC6) * bmp.AC5) >> 15;
    temp.X2 = (bmp.MC << 11) / (temp.X1 + bmp.MD);
    temp.B5 = temp.X1 + temp.X2;

    temp.T = (temp.B5 + 8) >> 4;
}

float Read_Temperature(){
    return (float)temp.T / 10.0f;
}

// -------- PRESSURE --------
void BMP_ReadPressure()
{
    // ?? D�ZELTILDI
    uint8_t cmd = 0x34 + (current_oss << 6);

    HAL_I2C_Mem_Write(&hi2c3, BMP180_ADDR, 0xF4, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 100);
    HAL_Delay(current_delay);

    uint8_t bmp_data[3];
    HAL_I2C_Mem_Read(&hi2c3, BMP180_ADDR, 0xF6, I2C_MEMADD_SIZE_8BIT, bmp_data, 3, 100);

    // ?? D�ZELTILDI
    temp.UP = (((int32_t)bmp_data[0] << 16) | ((int32_t)bmp_data[1] << 8) | bmp_data[2]) >> (8 - current_oss);

    press.B6 = temp.B5 - 4000;

    press.X1 = (bmp.B2 * ((press.B6 * press.B6) >> 12)) >> 11;
    press.X2 = (bmp.AC2 * press.B6) >> 11;
    press.X3 = press.X1 + press.X2;

    press.B3 = (((bmp.AC1 * 4 + press.X3) << current_oss) + 2) >> 2;

    press.X1 = (bmp.AC3 * press.B6) >> 13;
    press.X2 = (bmp.B1 * ((press.B6 * press.B6) >> 12)) >> 16;
    press.X3 = (press.X1 + press.X2 + 2) >> 2;

    press.B4 = (bmp.AC4 * (uint32_t)(press.X3 + 32768)) >> 15;
    press.B7 = ((uint32_t)temp.UP - press.B3) * (50000 >> current_oss);

    if (press.B7 < 0x80000000)
        press.p = (press.B7 * 2) / press.B4;
    else
        press.p = (press.B7 / press.B4) * 2;

    press.X1 = (press.p >> 8) * (press.p >> 8);
    press.X1 = (press.X1 * 3038) >> 16;
    press.X2 = (-7357 * press.p) >> 16;

    press.p = press.p + ((press.X1 + press.X2 + 3791) >> 4);

    press.p = LowPassFilter(press.p, filtered_pressure, 0.1f);
    filtered_pressure = press.p;
}

// -------- ALTITUDE --------
float BMP_GetAltitude()
{
    float sea_level_pressure = 101325.0f;

    altitude = 44330.0f * (1.0f - powf((float)press.p / sea_level_pressure, 0.1903f));

    altitude = LowPassFilter(altitude, filtered_altitude, 0.1f);
    filtered_altitude = altitude;

    return altitude;
}

// -------- FILTER --------
float LowPassFilter(float new_value, float old_value, float alpha)
{
    return alpha * new_value + (1.0f - alpha) * old_value;
}

// -------- ALL --------
void ReadAll(){
    BMP_ReadTemp();                 // ?? �nce temp
    pressure_bmp = press.p;
    temperature = Read_Temperature();

    BMP_ReadPressure();             // ?? sonra pressure
    altitude = BMP_GetAltitude();
}
