#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include <stdint.h>

/* ===================== 发送 ===================== */
void BT_SendString(const char *str);
void BT_SendPose(int16_t pitch_filt, int16_t roll_filt,
                 int16_t pitch_raw,  int16_t roll_raw);

/* ===================== 接收（实时调参） ===================== */
void BT_RxStart(void);          /* 初始化后调用一次，开启接收 */
void BT_ParseLine(char *line);  /* 解析一行命令 */

#endif /* __BLUETOOTH_H */
