#ifndef __USB_HID_H__
#define __USB_HID_H__

#define HID_DEVICE "hidd"

int open_usb_hid(void);
void hid_write_timeout(void);
rt_err_t usb_hid_write(const rt_uint8_t *buffer, rt_size_t size);
void send_start_windos(rt_uint8_t _ch, rt_uint8_t _status);
void send_data_windos(rt_uint8_t _ch, rt_uint16_t _data, rt_uint8_t unit);
void send_result_windos(rt_uint8_t _ch, rt_uint16_t _a1, rt_uint16_t _a2, float _log);

#endif
