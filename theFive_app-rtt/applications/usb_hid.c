#include <board.h>
#include <rtdevice.h>
#include "usb_hid.h"

rt_device_t dhidd = RT_NULL;
static struct rt_mutex hid_lock;
rt_uint8_t hid_write_buf[128];
static rt_uint8_t hid_write_out_time = 0;
static struct rt_ringbuffer hid_buf;

int open_usb_hid(void)
{
	dhidd = rt_device_find(HID_DEVICE);
	if(dhidd == RT_NULL)
		return RT_ERROR;
	rt_device_open(dhidd, RT_DEVICE_FLAG_RDWR);
	rt_mutex_init(&hid_lock, "usb_hid_lock", RT_IPC_FLAG_FIFO);
	rt_ringbuffer_init(&hid_buf, hid_write_buf, sizeof(hid_write_buf));

	return RT_EOK;
}

rt_err_t dump_hid_data(const uint8_t *data, rt_size_t size)
{
    rt_size_t i;

    for (i = 0; i < size; i++)
    {
        rt_kprintf("%02x ", *data++);
        if ((i + 1) % 8 == 0)
        {
            rt_kprintf("\n");
        }else if ((i + 1) % 4 == 0){
            rt_kprintf(" ");
        }
    }
    return RT_EOK;
    return -RT_ERROR;
}

//void HID_Report_Received(hid_report_t report)
//{
//    rt_kprintf("\nHID Recived:");
//    rt_kprintf("\nReport ID %02x \n", report->report_id);
//    dump_hid_data(report->report, report->size);
//}

void usb_hid_write_buf(const rt_uint8_t* _data, rt_size_t size)
{
	rt_uint8_t _buf[64] = {0};
	rt_size_t buf_size;

	hid_write_out_time = 0;
	rt_mutex_take(&hid_lock, RT_WAITING_FOREVER);
	rt_ringbuffer_put(&hid_buf, _data, size);
	rt_mutex_release(&hid_lock);
	buf_size = rt_ringbuffer_data_len(&hid_buf);
	if(buf_size > 50)
	{
		//rt_kprintf("full\n");
		rt_ringbuffer_get(&hid_buf, _buf, buf_size);
		usb_hid_write(_buf, buf_size);
	}
}
/**
 *	超时或者缓存满 是触发条件
 */
void hid_write_timeout(void)
{
	rt_uint8_t* buf = RT_NULL;
	rt_uint8_t buf_size;
	hid_write_out_time++;

	if(hid_write_out_time > 10 && rt_ringbuffer_data_len(&hid_buf))
	{
		hid_write_out_time = 0;

		rt_mutex_take(&hid_lock, RT_WAITING_FOREVER);
		buf_size = rt_ringbuffer_data_len(&hid_buf);
		buf = rt_malloc(buf_size);
		rt_ringbuffer_get(&hid_buf, buf, buf_size);
		rt_mutex_release(&hid_lock);
		//rt_kprintf("send %d\n",buf_size);
		usb_hid_write(buf, buf_size);
		rt_free(buf);
	}
}
/**
 *	发送长度为64, 第一位为report id,最后一位为0
 *	所以实际有用的数据为 62 bytes
 *
 */
rt_err_t usb_hid_write(const rt_uint8_t* buffer, rt_size_t size)
{
	rt_uint8_t _use_count;
	rt_uint8_t report_id = 5;
	char data_buf[63] = {0};

	if(dhidd == RT_NULL)
	{
		rt_kprintf("please open hid first\n");
		return RT_ERROR;
	}
	do{
		rt_memset(data_buf, 0, 63);
		_use_count = size > 62 ? 62 : size;		//最后一位为0
		rt_memcpy(data_buf, buffer, _use_count);
		buffer += _use_count;
		size -= _use_count;

	    rt_device_write(dhidd, report_id, data_buf, 63);
	    rt_thread_delay(10);
	}
	while (size);

    return RT_EOK;
}

void send_start_windos(rt_uint8_t _ch, rt_uint8_t _status)
{
	char dbuf[6] = {0xd5, 0xc0, 2, _ch, _status, 0x0d};
	usb_hid_write_buf(dbuf, sizeof(dbuf));
}
/**
 *	发送测量数据到上位机
 *	_ch：通道
 *	_data:数据
 *	unit：单位（0 –>普通数据   1-->A1、A2标记点）
 */
void send_data_windos(rt_uint8_t _ch, rt_uint16_t _data, rt_uint8_t unit)
{
	char dbuf[8] = {0xd5, 0xa0, 4, _ch, 0, 100, unit, 0x0d};

	dbuf[4] = (_data >> 8) & 0x00ff;
	dbuf[5] = _data & 0x00ff;
	usb_hid_write_buf(dbuf, sizeof(dbuf));
}
void send_result_windos(rt_uint8_t _ch, rt_uint16_t _a1, rt_uint16_t _a2, float _log)
{
	char dbuf[13] = {0xd5, 0xb1, 9, _ch, 0, 0, 0, 0, 0, 0, 0, 0, 0x0d};

	dbuf[4] = (_a1 >> 8) & 0x00ff;
	dbuf[5] = _a1 & 0x00ff;
	dbuf[6] = (_a2 >> 8) & 0x00ff;
	dbuf[7] = _a2 & 0x00ff;
	rt_memcpy(&dbuf[8], &_log, 4);

	usb_hid_write_buf(dbuf, sizeof(dbuf));
}
