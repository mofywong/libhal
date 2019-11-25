#include <unistd.h>
#include <string.h>
#include <parson/parson.h>
#include "ym_lamp_device_ability.h"
#include "ym_lamp_device.h"

#define YM_LAMP_DEVICE_READ_TIME_OUT_US       3000000

static bool_t get_channel_status_node(ym_lamp_device_ctx_t* ctx, uint8_t channel, ym_lamp_device_channel_status_t** node)
{
    ym_lamp_device_channel_status_t* pos;
    ym_lamp_device_channel_status_t* n;

    if (ctx && node)
    {
        pthread_mutex_lock(&ctx->status_list_mutex);
        he_utils_list_for_each_entry_safe(pos, n, &ctx->status_list, node)
        {
            if ( pos && (pos->channel == channel) )
            {
                *node = pos;
                pthread_mutex_unlock(&ctx->status_list_mutex);
                return true;
            }
        }
        pthread_mutex_unlock(&ctx->status_list_mutex);
    }
    return false;
}

static bool_t set_last_time_policy(ym_lamp_device_ctx_t* ctx, uint8_t channel, bool_t enable)
{
    printf("set_last_time_policy channel(%d) enable:%d\n", channel, enable);
    ym_lamp_device_channel_status_t* pos;

    if (ctx && get_channel_status_node(ctx, channel, &pos))
    {
        pos->time_policy = enable;
        return true;
    }

    return false;
}

static bool_t get_last_time_policy(ym_lamp_device_ctx_t* ctx, uint8_t channel, bool_t* enable)
{
    ym_lamp_device_channel_status_t* pos;

    if (ctx && enable && get_channel_status_node(ctx, channel, &pos))
    {
        *enable = pos->time_policy;
        printf("get_last_time_policy channel(%d) enable:%d\n", channel, *enable);
        return true;
    }

    return false;
}

static bool_t set_last_onoff(ym_lamp_device_ctx_t* ctx, uint8_t channel, bool_t onoff)
{
    printf("set_last_onoff channel(%d) onoff:%d\n", channel, onoff);
    ym_lamp_device_channel_status_t* pos;

    if (ctx && get_channel_status_node(ctx, channel, &pos))
    {
        pos->onoff = onoff;
        return true;
    }

    return false;
}

static bool_t get_last_onoff(ym_lamp_device_ctx_t* ctx, uint8_t channel, bool_t* onoff)
{
    ym_lamp_device_channel_status_t* pos;

    if (ctx && onoff && get_channel_status_node(ctx, channel, &pos))
    {
        *onoff = pos->onoff;
        printf("get_last_onoff channel(%d) onoff:%d\n", channel, *onoff);
        return true;
    }

    return false;
}

static bool_t set_last_brightness(ym_lamp_device_ctx_t* ctx, uint8_t channel, uint8_t brightness)
{
    ym_lamp_device_channel_status_t* pos;

    if (ctx && get_channel_status_node(ctx, channel, &pos))
    {
        pos->brightness = brightness;
        return true;
    }

    return false;
}

static bool_t get_last_brightness(ym_lamp_device_ctx_t* ctx, uint8_t channel, uint8_t* brightness)
{
    ym_lamp_device_channel_status_t* pos;

    if (ctx && brightness && get_channel_status_node(ctx, channel, &pos))
    {
        *brightness = pos->brightness;
        return true;
    }

    return false;
}

static uint8_t hex2dec(uint8_t hex)
{
    /* 字面数字转换，非实际数值，比如0x21转换成21 */
    return (hex >> 4) * 10 + (hex & 0x0F);
}

static uint8_t check_sum(uint8_t* buffer, uint32_t size)
{
    uint64_t sum = 0;

    if (buffer)
    {
        for (uint8_t i = 0; i <= size; ++i)
        {
            sum += buffer[i];
        }
        return (uint8_t)(sum & 0xFF);
    }
    return 0;
}

static bool_t create_frame(ym_lamp_device_addr_t addr,
                            char code,
                            const bytes_t* data,
                            bytes_t* out)
{
    uint8_t index = 0;
    uint8_t check_sum_num = 0;
    char* out_data = NULL;
    char* data_data = NULL;

    if (data && out)
    {
        data_data = (char*)data->data;
        out_data = (char*)out->data;

        out->size = 12 + data->size;
        out->data = malloc(out->size);
        out_data = (char*)out->data;

        /* 帧起始符 */
        out_data[0] = 0x68;
        /* 地址域  传递时低字节在前 */
        out_data[1] = addr[0];
        out_data[2] = addr[1];
        out_data[3] = addr[2];
        out_data[4] = addr[3];
        out_data[5] = addr[4];
        out_data[6] = addr[5];
        /* 帧起始符 */
        out_data[7] = 0x68;
        /* 控制码 */
        out_data[8] = code;
        /* 数据域长度 */
        out_data[9] = data->size;
        /* 数据域 */
        index = 9;
        for (uint8_t i = 0; i < data->size; ++i)
        {
            out_data[++index] = data_data[i] + 0x33;
        }
        /* 校验和 */
        check_sum_num = check_sum((uint8_t*)out_data, index);
        out_data[++index] = check_sum_num;
        /* 结束符 */
        out_data[++index] = 0x16;
        return true;
    }
    return false;
}

static uint32_t serial_read(he_utils_serial_t* serial, const bytes_t* buf, uint32_t us)
{
    bytes_t start_bytes;
    bytes_t tmp_bytes;
    start_bytes.size = 1;
    start_bytes.data = malloc(1);
    tmp_bytes.size = buf->size - 1;
    tmp_bytes.data = malloc(tmp_bytes.size);
    uint32_t read_size = 0;

    if (serial && buf)
    {
        while (1)
        {
            read_size = he_utils_serial_read(serial, &start_bytes, us);
            if (0 == read_size)
            {
                free(start_bytes.data);
                return 0;
            }
            if ((1 == read_size) && (((char*)start_bytes.data)[0]) == 0x68)
            {
                // printf("recv start 0x68\n");
                break;
            }
        }

        free(start_bytes.data);

        ((char*)buf->data)[0] = 0x68;

        read_size = he_utils_serial_read(serial, &tmp_bytes, us);
        memcpy(buf->data + 1, tmp_bytes.data, read_size);

        free(tmp_bytes.data);

        return read_size + 1;
    }
}

static bool_t parse_frame(ym_lamp_device_addr_t addr,
                            char code,
                            const bytes_t* recv_data,
                            bytes_t* data)
{
    char* data_data = NULL;

    if (recv_data && data)
    {
        char* recv_data_data = (char*)recv_data->data;

        if ((recv_data->size > 12) &&
            (recv_data_data[0] == 0x68) &&
            (recv_data_data[1] == addr[0]) &&
            (recv_data_data[2] == addr[1]) &&
            (recv_data_data[3] == addr[2]) &&
            (recv_data_data[4] == addr[3]) &&
            (recv_data_data[5] == addr[4]) &&
            (recv_data_data[6] == addr[5]) &&
            (recv_data_data[7] == 0x68) &&
            (recv_data_data[8] == code) &&
            (recv_data_data[9] == (recv_data->size - 12)) &&
            (recv_data_data[recv_data->size - 1] == 0x16)
            )
        {
            data->size = recv_data_data[9];
            data->data = malloc(data->size);
            data_data = (char*)data->data;
            for (uint8_t i = 0; i < data->size; ++i)
            {
                data_data[i] = recv_data_data[10 + i] - 0x33;
            }
            // printf("parse_frame return true\n");
            return true;
        }
    }
    printf("parse_frame return false\n");
    return false;
}

static bool_t init_device_setting(ym_lamp_device_ctx_t* ctx)
{
    bytes_t data;
    bytes_t send_data;
    bytes_t recv_data;
    uint8_t enable_channel = 0;
    char* data_data = NULL;
    char* recv = NULL;

    if (ctx)
    {
        data.size = 6 + ctx->ability->channel;
        data.data = malloc(data.size);
        data_data = (char*)data.data;

        for (uint8_t i = 0; i < ctx->ability->channel; ++i)
        {
            enable_channel += (1 << i);
        }

        data_data[0] = 0x00;
        data_data[1] = 0x00;
        data_data[2] = 0x60;
        data_data[3] = 0x01;
        data_data[4] = enable_channel;
        data_data[5] = 0x00; // 所有channel默认上电关灯
        /* 接下来的设置组号这边无所谓 */

        /* 创建下行数据帧 */
        create_frame(ctx->addr, 0x1E, &data, &send_data);

        pthread_mutex_lock(&ctx->serial_mutex);
        if (send_data.size == he_utils_serial_write(ctx->serial_ctx, &send_data))
        {
            bytes_t recv_data_data;
            recv_data_data.data = NULL;

            recv_data.size = 17;
            recv_data.data = malloc(recv_data.size);

            uint32_t recv_size = serial_read(ctx->serial_ctx, &recv_data, YM_LAMP_DEVICE_READ_TIME_OUT_US);
            printf("init_device_setting read recv size = %d\n", recv_size);
            if ( (17 == recv_size) &&
                parse_frame(ctx->addr, 0x9E, &recv_data, &recv_data_data) &&
                (recv = (char*)recv_data_data.data) &&
                (recv[0] == 0x00) &&
                (recv[1] == 0x00) &&
                (recv[2] == 0x60) &&
                (recv[3] == 0x01) &&
                (recv[4] == 'Y') )
            {
                pthread_mutex_unlock(&ctx->serial_mutex);
                free(recv_data.data);
                free(send_data.data);
                free(data.data);
                free(recv_data_data.data);
                return true;
            }
        }
    }
    free(recv_data.data);
    free(send_data.data);
    free(data.data);
    pthread_mutex_unlock(&ctx->serial_mutex);
    return false;
}


static bool_t device_connect(ym_lamp_device_ctx_t* ctx)
{
    if (ctx)
    {
        if (ctx->serial_ctx)
        {
            printf("device_connect serial has been open\n");
            return true;
        }

        printf("dev:%s, baud:%d, parity:%d, data:%d, stop:%d, flow:%d\n",
            ctx->dev, ctx->port.baud, ctx->port.parity, ctx->port.data, ctx->port.stop, ctx->port.flow);
        he_utils_serial_t* serial = he_utils_serial_open(ctx->dev, &ctx->port);
        if (serial)
        {
            ctx->serial_ctx = serial;
            sleep(1);
            printf("device_connect success\n");
            return true;
        }
    }
    return false;
}

static void device_disconnect(ym_lamp_device_ctx_t* ctx)
{
    if (ctx)
    {
        pthread_mutex_lock(&ctx->serial_mutex);
        he_utils_serial_close(ctx->serial_ctx);
        free(ctx->serial_ctx);
        ctx->serial_ctx = NULL;
        pthread_mutex_unlock(&ctx->serial_mutex);
    }
}

typedef struct open_input_s{
    ym_lamp_device_model_t model;
    ym_lamp_device_addr_t addr;
    char dev[24];
    he_utils_serial_port_t* port;
}open_input_t;

bool_t get_input_param(const_string_t device,open_input_t* input)
{
    JSON_Value* root = NULL;
    JSON_Object* root_object = NULL;

    root = json_parse_string(device);
    if(root) {
        root_object = json_object(root);
        if (root_object) {
            int model = json_object_get_number(root_object,"Model");
            input->model = model;
            const_string_t addr = json_object_get_string(root_object,"Addr");
            snprintf(input->addr, 6, "%s", addr);
            const_string_t dev = json_object_get_string(root_object,"Dev");
            snprintf(input->dev, 24, "%s", dev);
            int baud = json_object_dotget_number(root_object,"Port.Baud");
            input->port->baud = baud;
            int parity = json_object_dotget_number(root_object,"Port.Parity");
            input->port->parity = parity;
            int data = json_object_dotget_number(root_object,"Port.Data");
            input->port->data = data;
            int stop = json_object_dotget_number(root_object,"Port.Stop");
            input->port->stop = stop;
            return true;
        }
        json_value_free(root);
    }
    return false;
}


ym_lamp_device_ctx_t* ym_lamp_device_new (const_string_t device)
{
    open_input_t input;
    he_utils_serial_port_t port;
    input.port = &port;
    
    if (!get_input_param(device,&input)) {
        return NULL;
    }
    printf("########stop:%d\n",input.port->stop);
    ym_lamp_device_ctx_t* ctx = (ym_lamp_device_ctx_t*)malloc(sizeof(ym_lamp_device_ctx_t));
    if (ctx)
    {
        ctx->ability = ym_lamp_device_ability_create(input.model);
        if (!ctx->ability)
        {
            free(ctx);
            return NULL;
        }

        memcpy(ctx->addr, input.addr, sizeof(ym_lamp_device_addr_t));
        snprintf(ctx->dev, 32, "%s", input.dev);
        pthread_mutex_init(&ctx->serial_mutex, NULL);
        pthread_mutex_init(&ctx->status_list_mutex, NULL);
        HE_UTILS_INIT_LIST_HEAD(&ctx->status_list);
        ctx->port = *input.port;
        ctx->model = input.model;
        ctx->serial_ctx = NULL;

        /* 初始化状态链表 */
        for (uint8_t i = 1; i <= ctx->ability->channel; ++i)
        {
            ym_lamp_device_channel_status_t* channel_status_node = NULL;
            channel_status_node = (ym_lamp_device_channel_status_t*)malloc(sizeof(ym_lamp_device_channel_status_t));
            HE_UTILS_INIT_LIST_HEAD(&channel_status_node->node);
            channel_status_node->channel = i;
            channel_status_node->time_policy = false;
            channel_status_node->onoff = true;
            channel_status_node->brightness = 100;
            pthread_mutex_lock(&ctx->status_list_mutex);
            he_utils_list_add_tail(&channel_status_node->node, &ctx->status_list);
            pthread_mutex_unlock(&ctx->status_list_mutex);
        }

        if (device_connect(ctx))
        {
            init_device_setting(ctx);
        }
    }
    return ctx;
}

void ym_lamp_device_free(ym_lamp_device_ctx_t* ctx)
{
    if (ctx)
    {
        pthread_mutex_lock(&ctx->serial_mutex);
        he_utils_serial_close(ctx->serial_ctx);
        free(ctx->serial_ctx);
        pthread_mutex_unlock(&ctx->serial_mutex);
        free(ctx->ability);
        free(ctx);
    }
}

bool_t ym_lamp_device_set_enable_time_policy(ym_lamp_device_ctx_t* ctx, uint8_t channel, bool_t enable)
{
    if (ctx)
    {
        set_last_time_policy(ctx, channel, enable);
    }
}

bool_t ym_lamp_device_set_onoff(ym_lamp_device_ctx_t* ctx, uint8_t channel, bool_t onoff)
{
    printf("ym_lamp_device_set_onoff: %d\n", onoff);
    bytes_t data;
    bytes_t send_data;
    bytes_t recv_data;
    char* data_data = NULL;
    char* recv = NULL;

    if (ctx)
    {
        data.size = 6;
        data.data = malloc(data.size);
        data_data = (char*)data.data;

        data_data[0] = 0x00;
        data_data[1] = 0x00;
        data_data[2] = 0x60;
        data_data[3] = 0x07;
        data_data[4] = 1 << (channel - 1);
        if (onoff)
        {
            data_data[5] = data_data[4];
        }
        else
        {
            data_data[5] = ~data_data[4];
        }

        /* 创建下行数据帧 */
        create_frame(ctx->addr, 0x1E, &data, &send_data);

        for (uint8_t count =0; count < 3; ++count)
        {
            printf("ym_lamp_device_set_onoff:%d \n", onoff);
            pthread_mutex_lock(&ctx->serial_mutex);
            if (send_data.size == he_utils_serial_write(ctx->serial_ctx, &send_data))
            {
                bytes_t recv_data_data;
                recv_data_data.data = NULL;

                recv_data.size = 17;
                recv_data.data = malloc(recv_data.size);

                printf("ym_lamp_device_set_onoff recv\n");
                sleep(1);
                uint32_t recv_size = serial_read(ctx->serial_ctx, &recv_data, YM_LAMP_DEVICE_READ_TIME_OUT_US);
                pthread_mutex_unlock(&ctx->serial_mutex);

                if ( (17 == recv_size) &&
                    parse_frame(ctx->addr, 0x9E, &recv_data, &recv_data_data) &&
                    (recv = (char*)recv_data_data.data) &&
                    (recv[0] == 0x00) &&
                    (recv[1] == 0x00) &&
                    (recv[2] == 0x60) &&
                    (recv[3] == 0x07) &&
                    (recv[4] == 'Y') )
                {
                    /* 开关灯操作收到回复后，需要至少等2s读取到的状态才会改变 */
                    sleep(2);
                    set_last_onoff(ctx, channel, onoff);
                    if (!onoff)
                    {
                        set_last_brightness(ctx, channel, 0);
                    }

                    free(recv_data.data);
                    free(send_data.data);
                    free(data.data);
                    free(recv_data_data.data);
                    return true;
                }
                free(recv_data_data.data);
            }
            else
            {
                pthread_mutex_unlock(&ctx->serial_mutex);
            }
            sleep(3);
        }
    }

    free(recv_data.data);
    free(send_data.data);
    free(data.data);
    return false;
}

bool_t ym_lamp_device_set_brightness(ym_lamp_device_ctx_t* ctx, uint8_t channel, uint8_t brightness)
{
    printf("ym_lamp_device_set_brightness:%d\n", brightness);
    bytes_t data;
    bytes_t send_data;
    bytes_t recv_data;
    char* data_data = NULL;
    char* recv = NULL;

    if (brightness > 100)
    {
        brightness = 100;
    }

    if (0 == brightness)
    {
        /* 关灯 */
        return ym_lamp_device_set_onoff(ctx, channel, false);
    }

    if ( ctx )
    {
        /* 设置亮度要先开灯 */
        if (!ym_lamp_device_set_onoff(ctx, channel, true))
        {
            return false;
        }

        data.size = 5 + ctx->ability->channel;
        data.data = malloc(data.size);
        data_data = (char*)data.data;

        data_data[0] = 0x00;
        data_data[1] = 0x00;
        data_data[2] = 0x60;
        data_data[3] = 0x09;
        data_data[4] = 1 << (channel - 1);
        data_data[5] = brightness;

        /* 创建下行数据帧 */
        create_frame(ctx->addr, 0x1E, &data, &send_data);

        for (uint8_t count = 0; count < 3; ++count)
        {
            printf("ym_lamp_device_set_brightness %d\n", brightness);
            pthread_mutex_lock(&ctx->serial_mutex);
            if (send_data.size == he_utils_serial_write(ctx->serial_ctx, &send_data))
            {
                bytes_t recv_data_data;
                recv_data_data.data = NULL;

                recv_data.size = 17;
                recv_data.data = malloc(recv_data.size);

                sleep(1);
                uint32_t recv_size = serial_read(ctx->serial_ctx, &recv_data, YM_LAMP_DEVICE_READ_TIME_OUT_US);
                pthread_mutex_unlock(&ctx->serial_mutex);

                if ( (17 == recv_size) &&
                    parse_frame(ctx->addr, 0x9E, &recv_data, &recv_data_data) &&
                    (recv = (char*)recv_data_data.data) &&
                    (recv[0] == 0x00) &&
                    (recv[1] == 0x00) &&
                    (recv[2] == 0x60) &&
                    (recv[3] == 0x09) &&
                    (recv[4] == 'Y') )
                {
                    printf("set brightness:%d success\n", brightness);

                    /* 调光操作收到回复后，需要至少等2s读取到的状态才会改变 */
                    sleep(2);
                    set_last_brightness(ctx, channel, brightness);
                    free(recv_data.data);
                    free(send_data.data);
                    free(data.data);
                    free(recv_data_data.data);
                    return true;
                }
                free(recv_data_data.data);
            }
            else
            {
                pthread_mutex_unlock(&ctx->serial_mutex);
            }

            sleep(3);
        }
    }

    free(recv_data.data);
    free(send_data.data);
    free(data.data);
    return false;
}

bool_t ym_lamp_device_get_ability(ym_lamp_device_ctx_t* ctx, ym_lamp_device_ability_t** ability)
{
    if (ctx && ability)
    {
        *ability = ctx->ability;
        return true;
    }
    return false;
}


bool_t ym_lamp_device_get_status(ym_lamp_device_ctx_t* ctx)
{
    printf("ym_lamp_device_get_status\n");
    bytes_t data;
    bytes_t send_data;
    bytes_t recv_data;
    bytes_t recv_data_data;
    recv_data_data.data = NULL;
    char* data_data = NULL;
    char* recv = NULL;

    if (ctx)
    {
        data.size = 5;
        data.data = malloc(data.size);
        data_data = (char*)data.data;

        data_data[0] = 0x00;
        data_data[1] = 0x00;
        data_data[2] = 0x60;
        data_data[3] = 0x13;
        data_data[4] = 0x01;

        /* 创建下行数据帧 */
        create_frame(ctx->addr, 0x1E, &data, &send_data);

        for (uint8_t count = 0; count < 3; ++count)
        {
            pthread_mutex_lock(&ctx->serial_mutex);
            if (send_data.size == he_utils_serial_write(ctx->serial_ctx, &send_data))
            {
                recv_data.size = 12 + 6 + 15 * ctx->ability->channel;
                recv_data.data = malloc(recv_data.size);

                uint32_t recv_size = serial_read(ctx->serial_ctx, &recv_data, YM_LAMP_DEVICE_READ_TIME_OUT_US);
                pthread_mutex_unlock(&ctx->serial_mutex);

                if ( (recv_data.size == recv_size) &&
                    parse_frame(ctx->addr, 0x9E, &recv_data, &recv_data_data) &&
                    (recv = (char*)recv_data_data.data) &&
                    (recv[0] == 0x00) &&
                    (recv[1] == 0x00) &&
                    (recv[2] == 0x60) &&
                    (recv[3] == 0x13) &&
                    (recv[4] == 0x03) )
                {
                    printf("****************************\n");
                    for (uint8_t index = 1; index <= ctx->ability->channel; ++index)
                    {
                        uint8_t start_index = 6 + (index - 1) * 15;
                        ym_lamp_device_channel_status_t*  status_node;                
                        
                        get_channel_status_node(ctx, index, &status_node);
                        status_node->onoff = (bool_t)(recv[start_index] & 0x01);
                        printf("channel:%d, onoff:%d\n", index, status_node->onoff);

                        status_node->voltage = (uint32_t)
                        ((uint32_t)hex2dec(recv[start_index + 2]) * 100 + 
                        (uint32_t)hex2dec(recv[start_index + 1])
                        ) * 100; // mv
                        printf("channel:%d, voltage:%d\n", index, status_node->voltage);

                        status_node->current = (uint32_t)
                        ((uint32_t)hex2dec(recv[start_index + 4]) * 100 + 
                        (uint32_t)hex2dec(recv[start_index + 3])
                        ) * 10; // ma
                        printf("channel:%d, current:%d\n", index, status_node->current);

                        status_node->active_power = (uint16_t)
                        ((uint16_t)hex2dec(recv[start_index + 6]) * 100 + 
                        (uint16_t)hex2dec(recv[start_index + 5]) );
                        printf("channel:%d, active_power:%d\n", index, status_node->active_power);

                        get_last_time_policy(ctx, index, &status_node->time_policy);
                        printf("channel:%d, time_policy:%d\n", index, status_node->time_policy);

                        /* 关灯后返回的亮度还是开灯时候的亮度值，并不是0，这边自己做处理 */
                        if (status_node->onoff)
                        {
                            status_node->brightness = recv[start_index + 9];
                        }
                        else
                        {
                            status_node->brightness = 0;
                        }
                        printf("channel:%d, brightness:%d\n", index, status_node->brightness);

                        status_node->active_total_energy = (uint32_t)
                        ((uint32_t)hex2dec(recv[start_index + 14]) * 1000000 + 
                        (uint32_t)hex2dec(recv[start_index + 13]) * 10000+
                        (uint32_t)hex2dec(recv[start_index + 12]) * 100 +
                        (uint32_t)hex2dec(recv[start_index + 11])
                        ) * 10; // wh
                        printf("channel:%d, active_total_energy:%d\n", index, status_node->active_total_energy);
                    }
                    free(recv_data.data);
                    free(send_data.data);
                    free(data.data);
                    free(recv_data_data.data);
                    return true;
                }
                free(recv_data_data.data);
            }
            else
            {
                pthread_mutex_unlock(&ctx->serial_mutex);
            }

            sleep(3);
        }
    }
    free(recv_data.data);
    free(send_data.data);
    free(data.data);
    printf("ym_lamp_device_get_status failed\n");
    return false;
}

bool_t creat_output_message(ym_lamp_device_channel_status_t* status_node, char* output)
{
    JSON_Value* root = NULL;
    JSON_Object* root_object = NULL;

    root = json_value_init_object();
    root_object = json_object(root);
    if (root_object) {
        if (status_node->onoff) {
            json_object_set_number(root_object,"OnOff",1);
        } else {
            json_object_set_number(root_object,"OnOff",0);
        }
        json_object_set_number(root_object,"Brightness",status_node->brightness);
        json_object_set_number(root_object,"Voltage",status_node->voltage);
        json_object_set_number(root_object,"Current",status_node->current);
        json_object_set_number(root_object,"ActivePower",status_node->active_power);
        json_object_set_number(root_object,"TotalEnergy",status_node->active_total_energy);
        output = json_serialize_to_string_pretty(root);
        return true;
    }
    return false;
}

bool_t ym_lamp_device_get_attr(ym_lamp_device_ctx_t* ctx, void* output)
{
    bool_t ret = false;
    if(ym_lamp_device_get_status(ctx)) {
        ym_lamp_device_channel_status_t*  status_node; 
        if (get_channel_status_node(ctx, 1, &status_node)) {
            ret = creat_output_message(status_node, (char*)output);
        }
    }
    return ret;
}