#include "mpu6050.h"

static uint8_t MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t buf[2] = {reg, data};
    return HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, buf, 2, 100);
}

static uint8_t MPU6050_ReadReg(uint8_t reg, uint8_t *data)
{
    if (HAL_OK == HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, &reg, 1, 100))
    {
        return HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, data, 1, 100);
    }
    return HAL_ERROR;
}

uint8_t MPU6050_Init(void)
{
    uint8_t who_am_i = 0;

    if (MPU6050_ReadReg(MPU6050_REG_WHO_AM_I, &who_am_i) != HAL_OK)
        return 1;
    if (who_am_i != 0x68)
        return 2;
    if (MPU6050_WriteReg(MPU6050_REG_PWR_MGMT1, 0x00) != HAL_OK)
        return 3;

    return 0;
}

uint8_t MPU6050_ReadAccel(MPU6050_Accel_t *accel)
{
    uint8_t raw[6];
    uint8_t reg = MPU6050_REG_ACCEL_XOUT_H;

    if (HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, &reg, 1, 100) != HAL_OK)
        return HAL_ERROR;
    if (HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, raw, 6, 100) != HAL_OK)
        return HAL_ERROR;

    accel->ax = (int16_t)((raw[0] << 8) | raw[1]);
    accel->ay = (int16_t)((raw[2] << 8) | raw[3]);
    accel->az = (int16_t)((raw[4] << 8) | raw[5]);

    return HAL_OK;
}

uint8_t MPU6050_ReadGyro(MPU6050_Gyro_t *gyro)
{
    uint8_t raw[6];
    uint8_t reg = MPU6050_REG_GYRO_XOUT_H;

    if (HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, &reg, 1, 100) != HAL_OK)
        return HAL_ERROR;
    if (HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, raw, 6, 100) != HAL_OK)
        return HAL_ERROR;

    gyro->gx = (int16_t)((raw[0] << 8) | raw[1]);
    gyro->gy = (int16_t)((raw[2] << 8) | raw[3]);
    gyro->gz = (int16_t)((raw[4] << 8) | raw[5]);

    return HAL_OK;
}

void MPU6050_ReadAll(MPU6050_Accel_t *accel, MPU6050_Gyro_t *gyro)
{
    uint8_t raw[14];
    uint8_t reg = MPU6050_REG_ACCEL_XOUT_H;

    HAL_I2C_Master_Transmit(&hi2c1, MPU6050_ADDR, &reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, MPU6050_ADDR, raw, 14, 100);

    accel->ax = (int16_t)((raw[0] << 8) | raw[1]);
    accel->ay = (int16_t)((raw[2] << 8) | raw[3]);
    accel->az = (int16_t)((raw[4] << 8) | raw[5]);

    gyro->gx = (int16_t)((raw[8]  << 8) | raw[9]);
    gyro->gy = (int16_t)((raw[10] << 8) | raw[11]);
    gyro->gz = (int16_t)((raw[12] << 8) | raw[13]);
}



























