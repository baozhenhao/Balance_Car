#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "main.h"
#include "i2c.h"

//mpu6050 标准7位i2c地址
#define MPU6050_ADDR 0x68<<1

//关键寄存器地址
#define MPU6050_REG_WHO_AM_I 0x75
#define MPU6050_REG_PWR_MGMT1 0x6B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B
#define MPU6050_REG_GYRO_XOUT_H 0x43

//加速度计原始数据（有符号16位）
typedef struct {
	int16_t ax;
	int16_t ay;
	int16_t az;
} MPU6050_Accel_t;

//陀螺仪原始数据（有符号16位）
typedef struct {
	int16_t gx;
	int16_t gy;
	int16_t gz;
} MPU6050_Gyro_t;

uint8_t MPU6050_Init(void);
uint8_t MPU6050_ReadAccel(MPU6050_Accel *accel);
uint8_t MPU6050_ReadGyro(MPU6050_Gyro_t *gyro);
void MPU6050_ReadALL(MPU6050_Accel,MPU6050_Gyro_t *gyro);

#endif /* INC_MPU6050_H_ */
