#ifndef __HE_UTILS_SERIAL_H__
#define __HE_UTILS_SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "types.h"
typedef struct he_utils_serial_s he_utils_serial_t;

typedef enum {
    HE_UTILS_SERIAL_BAUD_UNKNOWN = 0,

    HE_UTILS_SERIAL_BAUD_1200 = 1,
    HE_UTILS_SERIAL_BAUD_2400 = 2,
    HE_UTILS_SERIAL_BAUD_4800 = 3,
    HE_UTILS_SERIAL_BAUD_9600 = 4,
    HE_UTILS_SERIAL_BAUD_14400 = 5,
    HE_UTILS_SERIAL_BAUD_19200 = 6,
    HE_UTILS_SERIAL_BAUD_38400 = 7,
    HE_UTILS_SERIAL_BAUD_57600 = 8,
    HE_UTILS_SERIAL_BAUD_115200 = 9,
} he_utils_serial_baud_t;

typedef enum {
    HE_UTILS_SERIAL_PARITY_UNKNOWN = 0,

    HE_UTILS_SERIAL_PARITY_NONE = 1,
    HE_UTILS_SERIAL_PARITY_ODD = 2,
    HE_UTILS_SERIAL_PARITY_EVEN = 3,
} he_utils_serial_parity_t;

typedef enum {
    HE_UTILS_SERIAL_DATA_UNKNOWN = 0,

    HE_UTILS_SERIAL_DATA_5BIT = 1,
    HE_UTILS_SERIAL_DATA_6BIT = 2,
    HE_UTILS_SERIAL_DATA_7BIT = 3,
    HE_UTILS_SERIAL_DATA_8BIT = 4,
} he_utils_serial_data_bit_t;

typedef enum {
    HE_UTILS_SERIAL_STOP_UNKNOWN = 0,

    HE_UTILS_SERIAL_STOP_1BIT = 1,
    HE_UTILS_SERIAL_STOP_1BIT_HALF = 2,
    HE_UTILS_SERIAL_STOP_2BIT = 3,
} he_utils_serial_stop_bit_t;

typedef enum {
    HE_UTILS_SERIAL_FLOW_UNKNOWN = 0,

    HE_UTILS_SERIAL_FLOW_NONE = 1,
    HE_UTILS_SERIAL_FLOW_SOFTWARE = 2,
    HE_UTILS_SERIAL_FLOW_HARDWARE = 3,
} he_utils_serial_flow_t;

typedef struct {
    he_utils_serial_baud_t baud;
    he_utils_serial_parity_t parity;
    he_utils_serial_data_bit_t data;
    he_utils_serial_stop_bit_t stop;
    he_utils_serial_flow_t flow;
} he_utils_serial_port_t;

//串口打开函数
he_utils_serial_t*
he_utils_serial_open(const_string_t dev, const he_utils_serial_port_t* port);
//串口读取数据函数
uint32_t
he_utils_serial_read(he_utils_serial_t* serial, const bytes_t* buf, uint32_t us);
//串口发送函数
uint32_t
he_utils_serial_write(he_utils_serial_t* serial, const bytes_t* data);
//串口关闭函数
void
he_utils_serial_close(he_utils_serial_t* serial);

#ifdef __cplusplus
}
#endif
#endif
