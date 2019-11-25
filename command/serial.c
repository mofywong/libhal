#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include "serial.h"


struct he_utils_serial_s {
    int fd;
    struct termios new;
    struct termios old;
};

/*串口打開函數*/
he_utils_serial_t*
he_utils_serial_open(const_string_t dev, const he_utils_serial_port_t* port) {
    he_utils_serial_t* serial;
    int flags;

    if (!dev || !port)
        return NULL;

    serial = (he_utils_serial_t*)malloc(sizeof(he_utils_serial_t));
    if (serial) {
        serial->fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK);
        if (serial->fd > 0) {
            flags  = fcntl(serial->fd, F_GETFL, 0);
            if ((flags >= 0) && (!(flags & O_NONBLOCK)))
               fcntl(serial->fd, F_SETFL, flags | O_NONBLOCK);

            /* 设置串口各项参数:波特率、校验位、停止位、数据位 */
            memset(&serial->new, 0, sizeof(struct termios));
            memset(&serial->old, 0, sizeof(struct termios));
            if (tcgetattr(serial->fd, &serial->old) != 0) {
                printf("%s: Can't get attribute!!!", dev);
                goto out;
            }

            /* 使用 CLOCAL 用于忽略所有 MODEM 状态信号线，CREAD
             * 标志用于使能接收。CSIZE 为数据位掩码 */
            serial->new.c_cflag |= CLOCAL | CREAD;
            serial->new.c_cflag &= ~CSIZE;

            /*设置数据位数*/
            switch (port->data) {
            case HE_UTILS_SERIAL_DATA_5BIT:
                serial->new.c_cflag |= CS5;
                break;
            case HE_UTILS_SERIAL_DATA_6BIT:
                serial->new.c_cflag |= CS6;
                break;
            case HE_UTILS_SERIAL_DATA_7BIT:
                serial->new.c_cflag |= CS7;
                break;
            case HE_UTILS_SERIAL_DATA_8BIT:
                serial->new.c_cflag |= CS8;
                break;
            default:
                printf("%s: Unkown data bits(%d)!!!", dev,
                                 port->data);
                goto out;
            }

            /* 设置停止位 */
            switch (port->stop) {
            case HE_UTILS_SERIAL_STOP_1BIT:
                break;
            case HE_UTILS_SERIAL_STOP_2BIT:
                serial->new.c_cflag |= CSTOPB;
                break;
            default:
                printf("%s: Unkown stop bits(%d)!!!", dev,
                                 port->stop);
                goto out;
            }

            /* 设置校验位 */
            switch (port->parity) {
            case HE_UTILS_SERIAL_PARITY_ODD:
                serial->new.c_cflag |= (PARENB | PARODD);
                serial->new.c_iflag |= INPCK;
                break;
            case HE_UTILS_SERIAL_PARITY_EVEN:
                serial->new.c_cflag |= PARENB;
                serial->new.c_iflag |= INPCK;
                break;
            case HE_UTILS_SERIAL_PARITY_NONE:
                break;
            default:
                printf("%s: Unkown parity bits(%d)!!!", dev,
                                 port->parity);
                goto out;
            }

            /* 设置波特率 */
            switch (port->baud) {
            case HE_UTILS_SERIAL_BAUD_1200:
                cfsetispeed(&serial->new, B1200);
                cfsetospeed(&serial->new, B1200);
                break;
            case HE_UTILS_SERIAL_BAUD_2400:
                cfsetispeed(&serial->new, B2400);
                cfsetospeed(&serial->new, B2400);
                break;
            case HE_UTILS_SERIAL_BAUD_4800:
                cfsetispeed(&serial->new, B4800);
                cfsetospeed(&serial->new, B4800);
                break;
            case HE_UTILS_SERIAL_BAUD_9600:
                cfsetispeed(&serial->new, B9600);
                cfsetospeed(&serial->new, B9600);
                break;
            case HE_UTILS_SERIAL_BAUD_19200:
                cfsetispeed(&serial->new, B19200);
                cfsetospeed(&serial->new, B19200);
                break;
            case HE_UTILS_SERIAL_BAUD_38400:
                cfsetispeed(&serial->new, B38400);
                cfsetospeed(&serial->new, B38400);
                break;
            case HE_UTILS_SERIAL_BAUD_57600:
                cfsetispeed(&serial->new, B57600);
                cfsetospeed(&serial->new, B57600);
                break;
            case HE_UTILS_SERIAL_BAUD_115200:
                cfsetispeed(&serial->new, B115200);
                cfsetospeed(&serial->new, B115200);
                break;
            default:
                printf("%s: Unkown baudrate(%d)!!!", dev, port->baud);
                goto out;
            }


            /*
                VMIN定义了要读取的最小字节数，read()只有在读取了VMIN个字节数据或收到一个信号才会返回
                watch out ,如果VMIM为0,则read函数会以非阻塞形式读取。
            */
            serial->new.c_cc[VTIME] = 0;
            serial->new.c_cc[VMIN] = 1;

            /* 通过 tcflush清空输入和输出缓冲区,最后通过 tcsetattr
             * 函数对将配置实际作用于串口 */
            tcflush(serial->fd, TCIOFLUSH);
            if (!tcsetattr(serial->fd, TCSANOW, &serial->new)) {
                return serial;
            }

            printf("%s: Can't set attribute!!!", dev);
        out:
            close(serial->fd);
        }

       free(serial);
    }

    return NULL;
}

/* 串口接收函数 */
uint32_t
he_utils_serial_read(he_utils_serial_t* serial, const bytes_t* buf, uint32_t us) {
    printf("he_utils_serial_read expect size:%ld\n", buf->size);
    ssize_t total_size = 0;
    ssize_t read_size = 0;
    struct timeval tv;
    fd_set rd;
    struct timeval start_tv;
    struct timeval current_tv;
    uint32_t time_interval_us = 0;
    uint32_t left_us = us;

    if (serial && buf && buf->data && (buf->size > 0)) {
        FD_ZERO(&rd);
        FD_SET(serial->fd, &rd);

        gettimeofday(&start_tv, NULL);
        while ((total_size < buf->size) && (time_interval_us < us))
        {
            left_us = us - time_interval_us;
            tv.tv_sec = left_us / 1000000;
            tv.tv_usec = left_us % 1000000;
            if (select(serial->fd + 1, &rd, NULL, NULL, &tv) > 0)
            {
                read_size = read(serial->fd, buf->data + total_size, buf->size - total_size);
                total_size += read_size;
            }

            gettimeofday(&current_tv, NULL);
            time_interval_us = (uint32_t) ((long long)((long long)current_tv.tv_sec * 1000000 + current_tv.tv_usec) -
                                                    (long long)((long long)start_tv.tv_sec * 1000000 + start_tv.tv_usec));
        }
    }

    printf("read size = %ld\n", total_size);
    printf("=============serial read================\n");
    for (uint8_t i = 0; i < total_size; i++)
    {
        printf("0x%x ", ((char*)(buf->data))[i]);
    }
    printf("\n=========================================\n");

    return total_size;
}

/* 串口发送函数 */
uint32_t
he_utils_serial_write(he_utils_serial_t* serial, const bytes_t* buf) {
    printf("=============serial write================\n");
    char* d = (char*)buf->data;
    for (uint8_t i = 0; i < buf->size; i++)
    {
        printf("0x%x ", d[i]);
    }
    printf("\n=========================================\n");
    ssize_t size = -1;
    int flags;

    if (serial && buf && buf->data && (buf->size > 0)) {
        flags  = fcntl(serial->fd, F_GETFL, 0);
        fcntl(serial->fd, F_SETFL, flags & ~O_NONBLOCK);
        size = write(serial->fd, buf->data, buf->size);
        fcntl(serial->fd, F_SETFL, flags);
    }

    return size > 0 ? size : 0;
}

/*串口关闭函数*/
void
he_utils_serial_close(he_utils_serial_t* serial) {
    if (serial) {
        tcflush(serial->fd, TCIOFLUSH);
        tcsetattr(serial->fd, TCSANOW, &serial->old);
        close(serial->fd);
    }
}
