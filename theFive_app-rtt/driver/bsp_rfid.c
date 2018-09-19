#include "bsp_rfid.h"

#if 0
    #define RFIDDEBUG      rt_kprintf
#else
    #define RFIDDEBUG(...)
#endif

#define RFID_ADDR                0Xb0>>1
#define METHOD 1
#define RFID_I2CBUS_NAME  "i2c5"     /* I2C�豸����,�����drv_i2c.cע���I2C�豸����һ�� */

static struct rt_i2c_bus_device *rfid_i2c_bus;    /* I2C�豸��� */

rt_err_t rfid_write_reg(rt_uint8_t* data, rt_uint8_t len)
{
    rt_uint8_t *buf = RT_NULL;
	rt_uint8_t cCheckSum = 0;
	rt_err_t result = 0;

	buf = rt_malloc(len+1);
	if(RT_NULL == buf)
		return -RT_ERROR;
	for(rt_uint8_t i = 0;i<len;i++)
	{
		buf[i] = data[i];
		cCheckSum ^= data[i];
	}
    buf[len] = cCheckSum;

    if (rt_i2c_master_send(rfid_i2c_bus, RFID_ADDR, 0, buf, len+1) == len+1)
    {
		result = RT_EOK;
		goto __exit;
    }
    else
    {
		result = -RT_ERROR;
		goto __exit;
    }
	
__exit:
	if(buf)
		rt_free(buf);
	
    return result; 
}
rt_err_t rfid_read_reg(rt_uint8_t *buf, rt_uint8_t len)
{
    struct rt_i2c_msg msgs;

    msgs.addr  = RFID_ADDR;     /* �ӻ���ַ */
    msgs.flags = RT_I2C_RD;       /* ����־ */
    msgs.buf   = buf;             /* ��ȡ����ָ�� */
    msgs.len   = len;             /* ��ȡ�����ֽ��� */

    if (rt_i2c_transfer(rfid_i2c_bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}
rt_uint8_t SetCard(void)
{
	//���ô����ߺ͹ر��Զ�Ѱ��
	rt_uint8_t ComSet[] = {0x03, COMM_CONTROL_ANTENNA, 0x11};
	
//// ������
//rt_uint8_t ComHaltCard[]    
//	= {0x02, 
//	   COMM_CARD_HALT,
//	  };  
//rt_uint8_t ComHaltMCU[]    
//	= {0x02, 
//	   	  COMM_SET_MCU_IDLE,
//		};
	rt_uint8_t _rev[3] = {0};
	rt_uint8_t cStatus;
	
	rfid_write_reg(ComSet, 3);
	rt_thread_delay(18);
	rfid_read_reg(_rev, 3);

	if ((_rev[1] != ComSet[1]) || cStatus != 0)
	{
		return 1;
	}
	return 0;
}
/**
 * @brief  ������
 *  
 * @param  rev:���տ��ţ�4�ֽ�
 *
 * @return
 */	
rt_uint8_t SearchCard(rt_uint8_t *rev)
{
	rt_uint8_t ComSearchCard[]  = {0x03, COMM_MIFARE_SEARCH_CARD, 0x00};
	rt_uint8_t cStatus;	
	
	cStatus = rfid_write_reg(ComSearchCard, 3);
	if(0 != cStatus)
	{
		cStatus = 1;
		goto __exit;
	}
	rt_thread_delay(18);
	cStatus = rfid_read_reg(rev,7);	
	if(0 != cStatus)
	{
		cStatus = 2;
		goto __exit;
	}
	if ((rev[1] != ComSearchCard[1]))
	{
		cStatus = 3;
		goto __exit;
	}
__exit:
	return cStatus;
}
/**
 * @brief  ��ʼ��Ǯ��
 *  
 * @param  num�����
 *		   buf����ʼֵ��4�ֽ�
 * @return
 */
rt_uint8_t InitWallet(rt_uint8_t num, rt_uint8_t *buf)
{
	rt_uint8_t ComIntiPurse[] = {0x0E, 
								 COMM_INIT_PURSE, 
								 0x00, 0x05, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa , 0x78, 0x56, 0x34, 0x12};
	
	// Ǯ���� INC & DEC	��Ľ��
//	rt_uint8_t pursevalue[]     
//	= {0x79,0x56,0x34, 0x12};
	
	rt_uint8_t _rev[3] = {0};
	rt_uint8_t cStatus;
	
	ComIntiPurse[3] = num;
	rt_memcpy(&ComIntiPurse[10], buf, 4);
	
	cStatus = rfid_write_reg(ComIntiPurse, sizeof(ComIntiPurse));
	if(0 != cStatus)
	{
		cStatus = 1;
		goto __exit;
	}
	rt_thread_delay(18);
	cStatus = rfid_read_reg(_rev, 3);
	if(0 != cStatus)
	{
		cStatus = 2;
		goto __exit;
	}
	if ((_rev[1] != ComIntiPurse[1]))
	{
		cStatus = 3;
		goto __exit;
	}
__exit:
	return cStatus;
}
/**
 * @brief  ��Ǯ��ֵ
 *  
 * @param  num�����
 *		   rev:����Ǯ��ֵ��4�ֽ�
 * @return
 */
rt_uint8_t ReadWallet(rt_uint8_t num, rt_uint8_t *rev)
{
	rt_uint8_t ComReadPurse[] = {0x0A, 
								 COMM_READ_PURSE, 
								 0x00, 0x0a, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
	rt_uint8_t cStatus;

	ComReadPurse[3] = num;							 
	cStatus = rfid_write_reg(ComReadPurse, sizeof(ComReadPurse));
	if(0 != cStatus)
	{
		cStatus = 1;
		goto __exit;
	}
	rt_thread_delay(18);
	cStatus = rfid_read_reg(rev, 7);						  
	if(0 != cStatus)
	{
		cStatus = 2;
		goto __exit;
	}
	if ((rev[1] != ComReadPurse[1]))
	{
		cStatus = 3;
		goto __exit;
	}
__exit:
	return cStatus;
}
/**
 * @brief  ��ֵ���ۿ�
 *  
 * @param 	money�����
 *			cfg��1Ϊ��ֵ
 *				 0Ϊ�ۿ�
 *			num�����
 * @return
 */
rt_uint8_t INC_DEC_Wallet(rt_uint8_t money, rt_uint8_t cfg, rt_uint8_t num)
{
	rt_uint8_t ComWallet[] = {0x0E, 
							  COMM_INC_VALUE, 
							  0x00, 0x05, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x02, 0x00, 0x00, 0x00};
	rt_uint8_t _rev[3] = {0};
	rt_uint8_t cStatus = 0;
	
	if(cfg)
		ComWallet[1] = COMM_INC_VALUE;
	else
		ComWallet[1] = COMM_DEC_VALUE;
	ComWallet[3] = num;
	cStatus = rfid_write_reg(ComWallet, sizeof(ComWallet));
	if(0 != cStatus)
	{
		cStatus = 1;
		goto __exit;
	}
	rt_thread_delay(18);
	cStatus = rfid_read_reg(_rev, 3);
	if(0 != cStatus)
	{
		cStatus = 2;
		goto __exit;
	}
	if ((_rev[1] != ComWallet[1]))
	{
		cStatus = 3;
		goto __exit;
	}
__exit:
	return cStatus;
}
/**
 * @brief  ����Ǯ��
 *  
 * @param  now_num����ǰǮ��
 *		   backup_num������Ǯ��
 * @return
 */
rt_uint8_t backups_Wallet(rt_uint8_t now_num, rt_uint8_t backup_num)
{
	rt_uint8_t ComBakPurse[] = {0x0B, 
							    COMM_BAK_PURSE, 
							    0x00, 0x05, 0x06, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
	rt_uint8_t _rev[3] = {0};
	rt_uint8_t cStatus = 0;
	
	ComBakPurse[3] = now_num;
	ComBakPurse[4] = backup_num;
	cStatus = rfid_write_reg(ComBakPurse, sizeof(ComBakPurse));
	if(0 != cStatus)
	{
		cStatus = 1;
		goto __exit;
	}
	rt_thread_delay(18);
	cStatus = rfid_read_reg(_rev,3);
	if(0 != cStatus)
	{
		cStatus = 2;
		goto __exit;
	}
	if ((_rev[1] != ComBakPurse[1]))
	{
		cStatus = 3;
		goto __exit;
	}
__exit:
	return cStatus;
}
/**
 * @brief  ��������
 *  
 * @param  num�����
 *		   rev:����ֵ��16�ֽ�
 * @return
 */
rt_uint8_t ReadBlock(rt_uint8_t num, rt_uint8_t *rev)
{
	rt_uint8_t cReadBlock[] = {0x0A, 
							   COMM_READ_BLOCK, 
							   0x00, 0x00, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa};
	rt_uint8_t cStatus = 0;

	cReadBlock[3] = num;
	cStatus = rfid_write_reg(cReadBlock, sizeof(cReadBlock));
	if(0 != cStatus)
	{
		cStatus = 1;
		goto __exit;
	}
	rt_thread_delay(18);
	cStatus = rfid_read_reg(rev,17);
	if(0 != cStatus)
	{
		cStatus = 2;
		goto __exit;
	}
	if ((rev[1] != cReadBlock[1]))
	{
		cStatus = 3;
		goto __exit;
	}
__exit:
	return cStatus;
}
/**
 * @brief  д������
 *  
 * @param  num�����
 *		   buf: д��ֵ��16�ֽ�
 *
 * @return
 */
rt_uint8_t WriteBlock(rt_uint8_t num, rt_uint8_t *buf)
{
	rt_uint8_t ComWriteBlock[] = {0x1A, 
								  COMM_WRITE_BLOCK, 
								  0x00, 0x04, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
								  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
								  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
	rt_uint8_t _rev[3] = {0};
	rt_uint8_t cStatus;
	
	ComWriteBlock[3] = num;
	cStatus = rfid_write_reg(ComWriteBlock, sizeof(ComWriteBlock));
	if(0 != cStatus)
	{
		cStatus = 1;
		goto __exit;
	}
	rt_thread_delay(18);
	cStatus = rfid_read_reg(_rev,3);
	if(0 != cStatus)
	{
		cStatus = 2;
		goto __exit;
	}
	if ((_rev[1] != ComWriteBlock[1]))
	{
		cStatus = 3;
		goto __exit;
	}
__exit:
	return cStatus;
}

/*
���� RFID�� 
*/
void TestRFID(void)
{
	rt_uint8_t cReceBuf[20] = {0};	//���ջ�����	
	if(0 == SearchCard(cReceBuf))
	{
		for(rt_uint8_t i = 0;i<7;i++)
		{
			rt_kprintf("%x  ",cReceBuf[i]);
		}
		rt_kprintf("\r\n");
		rt_memset(cReceBuf, 0, 7);
	}
	
	if(ReadBlock(50, cReceBuf) == 0)
	{
		for(rt_uint8_t i = 0;i<12;i++)
		{
			rt_kprintf("%x  ",cReceBuf[i]);
		}
		rt_kprintf("\r\n");
		rt_memset(cReceBuf, 0, 7);
	}
	if(ReadWallet(6, cReceBuf) == 0)
	{
		for(rt_uint8_t i = 0;i<7;i++)
		{
			rt_kprintf("%x  ",cReceBuf[i]);
		}
		rt_kprintf("\r\n");
	}		
}

//rfidӲ����ʼ��
//����ֵ: 0,�ɹ� / -1,�������
int rfid_hw_init(void)
{
    rfid_i2c_bus = rt_i2c_bus_device_find(RFID_I2CBUS_NAME);  /*����I2C�豸*/

    if (rfid_i2c_bus == RT_NULL)
    {
        RFIDDEBUG("can't find ad1110 %s device\r\n", rfid_i2c_bus);
        return -RT_ERROR;
    }

    RFIDDEBUG("rfid set i2c bus to %s\r\n", rfid_i2c_bus);

    return RT_EOK;
}
INIT_APP_EXPORT(rfid_hw_init);











