#ifndef __BSP_RFID_H
#define __BSP_RFID_H
#include <rthw.h>

#define	    WRITE_M104X 	0xb0
#define	    READ_M104X		0xb1

#define	    COMM_MODULE_LOGIN_ON		0X00    //登记
					 
#define 	COMM_SET_MCU_IDLE			0X04	//设置模块空闲睡眠模式		
#define 	COMM_CONTROL_ANTENNA		0X05	//模块天线控制、自动寻卡设置
			
#define 	COMM_PA_OUT_SET				0X06	//PA?????			
#define 	COMM_PB_OUT_SET				0X07	//PB?????			
#define 	COMM_PA_OUT_CLR				0X08	//PA????0			
#define 	COMM_PB_OUT_CLR				0X09	//PB????0
#define 	COMM_PA_DRAIN_OPEN			0X0A	//PA???????
#define 	COMM_PB_DRAIN_OPEN			0X0B	//PB???????
#define 	COMM_PA_INPUT				0X0C	//PA???			
#define 	COMM_PB_INPUT				0X0D	//PB???			
#define 	COMM_RS485_MODE				0X0E	//RS485????
#define 	COMM_INSIDE_CARD_CODE		0X0F	//??1000???????
#define 	COMM_WRITE_CARD_INTO		0X10	//????????
#define 	COMM_READ_CARD_OUT			0X11	//????????

#define 	COMM_WRITE_MODULE_ID		0X13	//设置设备标识
#define 	COMM_READ_MODULE_ID			0X14	//读取设备标识
#define 	COMM_SET_COM_BPS			0X15	//???????
#define 	COMM_READ_HARDWARE_VERSION	0X16	//????????
#define 	COMM_READ_SEARIAL_NUMBER	0X17	//???????
#define 	COMM_WRITE_SEARIAL_NUMBER	0X18	//???????

#define 	COMM_SELF_SEARCH_CARD		0X19	//主动寻卡
#define 	COMM_CONTROL_RED_LED		0X1A	//控制
#define 	COMM_CONTROL_GREEN_LED		0X1B	//????
#define 	COMM_CONTROL_YELLOW_LED		0X1C	//????
#define 	COMM_CONTROL_BUZZER			0X1D	//?????
#define 	COMM_CONTROL_CLR_KEY		0X1E	//????
#define 	COMM_CONTROL_DAT_DISPLAY	0X1F	//??????????
								   

#define 	COMM_MIFARE_SEARCH_CARD		0X20	//TYPE-A寻卡
#define 	COMM_READ_BLOCK				0X21	//读块
#define 	COMM_READ_SECTOR			0X22	//读扇区
#define 	COMM_WRITE_BLOCK			0X23	//写块	
#define 	COMM_INIT_PURSE				0X24	//初始化钱包
#define 	COMM_READ_PURSE				0X25	//读钱包
#define 	COMM_INC_VALUE				0X26	//充值
#define 	COMM_DEC_VALUE				0X27	//扣款
#define 	COMM_BAK_PURSE				0X28	//备份钱包值
#define 	COMM_CARD_HALT				0X29	//卡休眠
#define 	COMM_LOAD_KEY_AB			0X2A	//下载密钥
#define 	COMM_READ_EEPROM			0X2B	//读RC500EEPROM
#define 	COMM_WRITE_EEPROM			0X2C	//读RC500EEPROM

#define 	COMM_HALT_ULTRA_LIGHT		0X32	//Halt Ultra Light?				
#define 	COMM_SEL_ULTRA_LIGHT		0X33	//????Ultra Light?	
#define 	COMM_READ_ULTRA_LIGHT		0X34	//Ultra Light??	
#define 	COMM_WRITE_ULTRA_LIGHT		0X35	//Ultra Light?? 
#define 	COMM_SET_SAM_BPS			0X36	//??SAM????	
#define 	COMM_RET_SAM				0X37	//SAM ???1 ???????,0:9600;1:38400

#define 	COMM_SAM_COS				0X38	//SAM ?COS ??
#define 	COMM_SAM_CONFIGE			0X39	//SAM ?????		
		
#define 	COMM_SET_TYPE_AB_OTHER		0X3A	//??????TYPE A/B

#define 	COMM_SEARCH_AT88RF020		0X3E	//??88RF020	
#define 	COMM_CHECK_AT88RF020_KEY	0X3F	//??88RF020??	
#define 	COMM_READ_AT88RF020_BLOCK	0X40	//88RF020??
#define 	COMM_WRITE_AT88RF020_BLOCK	0X41	//88RF020??
#define 	COMM_AT88RF020_LOCK			0X42	//88RF020??
#define 	COMM_AT88RF020_COUNT		0X43	//88RF020??
#define 	COMM_AT88RF020_HALT			0X44	//88RF020???	
#define 	COMM_MIFARE1_CONFIG_RESTORE	0X45	//mifare one ????	

#define 	COMM_MIFARE1_PCD_REQUEST	0X46	//mifare one ??
#define 	COMM_MIFARE1_PCD_ANTICOLL	0X47	//mifare one ???
#define 	COMM_MIFARE1_PCD_SELECT		0X48	//mifare one ??
#define 	COMM_AUTHENTICATION1		0X49	//??1
#define 	COMM_AUTHENTICATION2		0X4A	//??2
#define 	COMM_PCD_READ_BLOCK			0X4B	//?????
#define 	COMM_PCD_WRITE_BLOCK		0X4C	//?????
#define 	COMM_PCD_INIT_PURSE			0X4D	//????????
#define 	COMM_PCD_READ_PURSE			0X4E	//??????
#define 	COMM_PCD_DEC_VALUE			0X4F	//?????
#define 	COMM_PCD_INC_VALUE			0X50	//?????
#define 	COMM_PCD_RESTORE			0X51	//????????????
#define 	COMM_PCD_TRANSFER			0X52	//??????????
#define 	COMM_PCD_TYPEA_HIGH_RST		0X53	//TYPEA CPU????
#define 	COMM_PCD_TYPEA_HIGH_COS		0X54	//TYPEA CPU?COS??
#define 	COMM_PCD_TYPEB_ATTRIB		0X55	//??TYPE?
#define 	COMM_RET_TYPE_B_CPU			0X3B	//TYPE-BCPU ???
#define 	COMM_TYPE_B_CPU_COS			0X3C	//TYPE-B  COS ??
#define 	COMM_TYPE_B_CPU_HALT		0X3D	//TYPE-B???

#define 	COMM_TYPE_B_ST_SELECT		0X60			//????ST?
#define 	COMM_TYPE_B_ST_COMPLETION	0X61			//??ST???DESACTIVED??
#define 	COMM_TYPE_B_READ_SR176		0X62			//?SR176?1???
#define 	COMM_TYPE_B_WRITE_SR176		0X63			//?SR176?1???
#define 	COMM_TYPE_B_PROTECT_SR176	0X64			//SR176???? 
#define 	COMM_TYPE_B_READ_SRIX4K		0X65			//?SRIX4K?1???
#define 	COMM_TYPE_B_WRITE_SRIX4K	0X66			//?SRIX4K?1???
#define 	COMM_TYPE_B_AUATHENTICATE_SRIX4K	0X67	//SRIX4K?????
#define 	COMM_TYPE_B_GET_UID_SRIX4K			0X68			//??SRIX4K?UID
#define 	COMM_TYPE_B_LOCK_SRIX4K				0X69			//SRIX4K???? 

#define 	COMM_CARD_LIGHT						0X6A 
#define 	COMM_CARD_CL_DESELECT				0X6B

#define 	COMM_ISO15693_Inventory				0X70			//
#define 	COMM_ISO15693_Stay_Quiet			0X71			//
#define 	COMM_ISO15693_Select				0X72			//
#define 	COMM_ISO15693_Reset_Ready			0X73			//
#define 	COMM_ISO15693_Read					0X74			//
#define 	COMM_ISO15693_Write					0X75			//
#define 	COMM_ISO15693_Lock_Block			0X76			//
#define 	COMM_ISO15693_Write_AFI				0X77			//
#define 	COMM_ISO15693_Lock_AFI				0X78			//
#define 	COMM_ISO15693_Write_DSFID			0X79			//
#define 	COMM_ISO15693_Lock_DSFID			0X7A			//
#define 	COMM_ISO15693_Get_System_Info		0X7B			//
#define 	COMM_ISO15693_Get_Block_Security	0X7C			//


#define 	COMM_125K_SEARCH_CARD	 			0x80
#define 	COMM_SET_3686KHZ			0XF1	//??????
#define 	COMM_PROGRAME_ISP			0xf2	//isp	
#define 	COMM_PRODUCT_NAME			0xf3	//????

rt_uint8_t rfid_SetCard(void);
rt_uint8_t rfid_SearchCard(rt_uint8_t *rev);
rt_uint8_t rfid_ReadWallet(rt_uint8_t num, rt_uint8_t *rev);
rt_uint8_t rfid_ReadBlock(rt_uint8_t num, rt_uint8_t *rev);
void TestRFID(void);

int rfid_hw_init(void);

#endif

