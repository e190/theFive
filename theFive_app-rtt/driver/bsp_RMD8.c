#include "bsp_RMD8.h"

/* �����豸��� */
static rt_device_t printer_uart_device = RT_NULL;

void printer_uart_putchar(const rt_uint8_t c)
{
    rt_size_t len = 0;
    rt_uint32_t timeout = 0;
    do
    {
        len = rt_device_write(printer_uart_device, 0, &c, 1);
        timeout++;
    }
    while (len != 1 && timeout < 500);
}

rt_uint8_t PrinterSendData(rt_uint8_t* str, rt_uint8_t len)
{
	rt_uint8_t* sendbuf = NULL;
	rt_uint8_t sendlen = 0;
		
	sendbuf = rt_malloc(len+9);
	if(RT_NULL == sendbuf)
		return RT_ERROR;
	
	
//	sendbuf[sendlen++] = 0x1b;    //ˮƽ�ñ�    
//	sendbuf[sendlen++] = 0x44;
//	sendbuf[sendlen++] = 0x03;    //�ڵ�4���ַ�����ʼ
//	sendbuf[sendlen++] = 0x00;    //�������������
//	sendbuf[sendlen++] = 0x09;    //ִ��ˮƽ�ñ�
	
	sendbuf[sendlen++] = 0x1b;
	sendbuf[sendlen++] = 0x66;	//��ӡ�ո�
	sendbuf[sendlen++] = 0x00;
	sendbuf[sendlen++] = 0x01;
	
	
	rt_memcpy(&sendbuf[sendlen++], str, len-1);
	sendlen += len-1;
	
	sendbuf[--sendlen] = 0x0d;
	for(rt_uint8_t i = 0;i < sendlen;i++)
	{
		printer_uart_putchar(sendbuf[i]);
    }

	if(sendbuf)
		rt_free(sendbuf);
	
	return RT_EOK;
}
/*
 * ��ʼ����ӡ������
 */
int printer_init(void)
{
	rt_uint8_t sendbuf[20] = {0};
	rt_uint8_t sendlen = 0;
		
	
	sendbuf[sendlen++] = 0x1b;
	sendbuf[sendlen++] = 0x40;	/* ��ʼ����ӡ�� */
	sendbuf[sendlen++] = 0x00;    //�������������
	
	sendbuf[sendlen++] = 0x1b;
	sendbuf[sendlen++] = 0x31;	//�����м��
	sendbuf[sendlen++] = 0x03;

	sendbuf[sendlen++] = 0x1b;
	sendbuf[sendlen++] = 0x70;	//�����ּ��
	sendbuf[sendlen++] = 0x00;
	
	sendbuf[sendlen++] = 0x1b;
	sendbuf[sendlen++] = 0x55;	//����Ŵ�
	sendbuf[sendlen++] = 0x01;
	
	sendbuf[sendlen++] = 0x1b;
	sendbuf[sendlen++] = 0x56;	//����Ŵ�	
	sendbuf[sendlen++] = 0x01;
	
	sendbuf[sendlen++] = 0x1b;
	sendbuf[sendlen++] = 0x57;	//��������Ŵ�	
	sendbuf[sendlen++] = 0x02;
	
	sendbuf[sendlen++] = 0x0d;
	for(rt_uint8_t i = 0;i < sendlen;i++)
	{
		printer_uart_putchar(sendbuf[i]);
    }
	
	return RT_EOK;
}
/*
 * ���ô�ӡ�������С
 */
int printer_set_font(rt_uint8_t font_size)
{
	rt_uint8_t sendbuf[5] = {0x1b, 0x57, font_size, 0};
	
	for(rt_uint8_t i = 0;i < 4;i++)
	{
		printer_uart_putchar(sendbuf[i]);
    }
	
	return RT_EOK;
}

/*
 * ���ô�ӡ�����
 */
int printer_set_gap(rt_uint8_t gap_size)
{
	rt_uint8_t sendbuf[5] = {0x1b, 0x57, gap_size, 0};
	
	for(rt_uint8_t i = 0;i < 4;i++)
	{
		printer_uart_putchar(sendbuf[i]);
    }
	
	return RT_EOK;
}
void TestPrinter(void)
{
	rt_uint8_t printer_cmd[] = {0x1b, 0x4a, 5, 0};
	rt_uint8_t num_char[] = "1234567890\n";
	rt_uint8_t en_char[] = "abcdefghijklmnopqrstuvwxyz\n";
	rt_uint8_t EN_char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	rt_uint8_t ch_char[] = "�������:\n";
	
	PrinterSendData(num_char, sizeof(num_char));
	PrinterSendData(en_char, sizeof(en_char));
	PrinterSendData(EN_char, sizeof(EN_char));
	PrinterSendData(ch_char, sizeof(ch_char));
	PrinterSendData(printer_cmd, sizeof(printer_cmd));
}


rt_err_t printer_uart_open(const char *name)
{
    rt_err_t res;

    /* ����ϵͳ�еĴ����豸 */
    printer_uart_device = rt_device_find(name);   
    /* ���ҵ��豸����� */
    if (printer_uart_device != RT_NULL)
    {          
        /* ���豸���Կɶ�д���жϷ�ʽ */
        res = rt_device_open(printer_uart_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX );       
        /* ��鷵��ֵ */
        if (res != RT_EOK)
        {
            rt_kprintf("open %s device error.%d\n",name,res);
            return -RT_ERROR;
        }
    }
    else
    {
        rt_kprintf("can't find %s device.\n",name);
        return -RT_ERROR;
    }

    return RT_EOK;
}


int hw_printer_init(void)
{
	struct serial_configure config = {                                          \
										BAUD_RATE_9600,   /* 9600 bits/s */  	\
										DATA_BITS_8,      /* 8 databits */     \
										STOP_BITS_1,      /* 1 stopbit */      \
										PARITY_NONE,      /* No parity  */     \
										BIT_ORDER_LSB,    /* LSB first sent */ \
										NRZ_NORMAL,       /* Normal mode */    \
										RT_SERIAL_RB_BUFSZ, /* Buffer size */  \
										0                                      \
									 };
/*
	1b  31  n     �����о�
	1b  55  n     ����Ŵ�	
	1b  56  n     ����Ŵ�	
	1b  57  n     ��������Ŵ�
	1b  70  n     �����ּ��
	1b  4a  n     ��ֽ n ����
*/
	
	printer_uart_open(RT_PRINTER_DEVICE_NAME);
	rt_device_control(printer_uart_device, RT_DEVICE_CTRL_CONFIG, &config);
	
	return RT_EOK;
}
INIT_COMPONENT_EXPORT(hw_printer_init);
