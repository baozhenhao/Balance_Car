#include "mpu6050.h"
// ==================== 软件模拟 I2C 引脚控制宏 ====================
#define SW_SCL_H()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET)
#define SW_SCL_L()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET)
#define SW_SDA_H()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET)
#define SW_SDA_L()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET)
#define SW_SDA_READ()  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12)
// ================================================================
float MPU_Pitch = 0.0f;
float MPU_Roll  = 0.0f;
float MPU_Gyro_X = 0.0f;
float MPU_Gyro_Y = 0.0f;

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
    uint8_t raw[14] = {0};
    uint32_t d;

    // 1. 发送起始信号
    SW_SDA_H(); SW_SCL_H(); d = 30; while(d--);
    SW_SDA_L(); d = 30; while(d--); SW_SCL_L(); d = 30; while(d--);

    // 2. 写 MPU6050 写地址 0xD0
    uint8_t temp_addr = 0xD0;
    for(int i=0; i<8; i++) {
        if(temp_addr & 0x80) SW_SDA_H(); else SW_SDA_L();
        temp_addr <<= 1; d = 15; while(d--); SW_SCL_H(); d = 15; while(d--); SW_SCL_L(); d = 15; while(d--);
    }
    SW_SDA_H(); SW_SCL_H(); d = 15; while(d--); SW_SCL_L(); d = 15; while(d--);

    // 3. 发送加速度计起始寄存器地址 0x3B
    uint8_t start_reg = 0x3B;
    for(int i=0; i<8; i++) {
        if(start_reg & 0x80) SW_SDA_H(); else SW_SDA_L();
        start_reg <<= 1; d = 15; while(d--); SW_SCL_H(); d = 15; while(d--); SW_SCL_L(); d = 15; while(d--);
    }
    SW_SDA_H(); SW_SCL_H(); d = 15; while(d--); SW_SCL_L(); d = 15; while(d--);

    // 4. 重新启动信号 (Restart)
    SW_SDA_H(); SW_SCL_H(); d = 30; while(d--); SW_SDA_L(); d = 30; while(d--); SW_SCL_L(); d = 30; while(d--);

    // 5. 写 MPU6050 读地址 0xD1
    temp_addr = 0xD1;
    for(int i=0; i<8; i++) {
        if(temp_addr & 0x80) SW_SDA_H(); else SW_SDA_L();
        temp_addr <<= 1; d = 15; while(d--); SW_SCL_H(); d = 15; while(d--); SW_SCL_L(); d = 15; while(d--);
    }
    SW_SDA_H(); SW_SCL_H(); d = 15; while(d--); SW_SCL_L(); d = 15; while(d--);

    // 6. 连续读取 14 个字节
    for(int j=0; j<14; j++) {
        uint8_t read_byte = 0;
        SW_SDA_H();
        for(int i=0; i<8; i++) {
            read_byte <<= 1; SW_SCL_H(); d = 15; while(d--);
            if(SW_SDA_READ()) read_byte |= 0x01;
            SW_SCL_L(); d = 15; while(d--);
        }
        raw[j] = read_byte;

        if(j == 13) SW_SDA_H(); else SW_SDA_L();
        SW_SCL_H(); d = 15; while(d--); SW_SCL_L(); d = 15; while(d--);
    }

    // 7. 停止信号
    SW_SDA_L(); SW_SCL_H(); d = 30; while(d--); SW_SDA_H(); d = 30; while(d--);

    // 8. 赋值合成数据
    accel->ax = (int16_t)((raw[0] << 8) | raw[1]);
    accel->ay = (int16_t)((raw[2] << 8) | raw[3]);
    accel->az = (int16_t)((raw[4] << 8) | raw[5]);

    gyro->gx = (int16_t)((raw[8]  << 8) | raw[9]);
    gyro->gy = (int16_t)((raw[10] << 8) | raw[11]);
    gyro->gz = (int16_t)((raw[12] << 8) | raw[13]);
}
#include <math.h>

// 定义两个全局变量存角度


// ---------------------------------------------------------
// 核心：一阶互补滤波算法 (把生肉变成牛排)
// 参数 dt: 两次调用的时间间隔 (单位：秒)
// ---------------------------------------------------------
void MPU6050_GetAngle(float dt)
{
    MPU6050_Accel_t accel;
    MPU6050_Gyro_t gyro;

    MPU6050_ReadAll(&accel, &gyro);

    float acc_x = accel.ax / 16384.0f;
    float acc_y = accel.ay / 16384.0f;
    float acc_z = accel.az / 16384.0f;

    MPU_Gyro_X = gyro.gx / 131.0f;
    MPU_Gyro_Y = gyro.gy / 131.0f;

    float accel_pitch = atan2(acc_y, acc_z) * 57.296f;
    float accel_roll  = atan2(-acc_x, acc_z) * 57.296f;

    MPU_Pitch = 0.98f * (MPU_Pitch + MPU_Gyro_X * dt) + 0.02f * accel_pitch;
    MPU_Roll  = 0.98f * (MPU_Roll  + MPU_Gyro_Y * dt) + 0.02f * accel_roll;
}



























