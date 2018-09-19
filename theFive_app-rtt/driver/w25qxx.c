#include "w25qxx.h"
#include "board.h"

uint16_t W25QXX_TYPE = W25Q128;	//Ĭ����W25Q128

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  hspi1.Instance = SPI3;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    rt_kprintf("123");
  }
SPI1_ReadWriteByte(0Xff); 
}
 

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q256
//����Ϊ32M�ֽ�,����512��Block,8192��Sector 
													 
//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{ 
    uint8_t temp;
    
    //SPI1_Init();		   	    //��ʼ��SPI
    //SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_2); //����Ϊ42Mʱ��,����ģʽ
    W25QXX_TYPE = W25QXX_ReadID();	        //��ȡFLASH ID.
    if(W25QXX_TYPE == W25Q128)                //SPI FLASHΪW25Q128
    {
        temp=W25QXX_ReadSR(3);              //��ȡ״̬�Ĵ���3���жϵ�ַģʽ
        if((temp&0X01)==0)	            //�������4�ֽڵ�ַģʽ,�����4�ֽڵ�ַģʽ
        {
            SPI1_ReadWriteByte(W25X_Enable4ByteAddr);//���ͽ���4�ֽڵ�ַģʽָ��    
        }
    }
    if(W25QXX_ReadID() != W25Q128)	
    {
        rt_kprintf("789");
    }
}  

//��ȡW25QXX��״̬�Ĵ�����W25QXXһ����3��״̬�Ĵ���
//״̬�Ĵ���1��
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
//״̬�Ĵ���2��
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//״̬�Ĵ���3��
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:״̬�Ĵ����ţ���:1~3
//����ֵ:״̬�Ĵ���ֵ
uint8_t W25QXX_ReadSR(uint8_t regno)   
{  
    uint8_t byte=0,command=0; 
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //��״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //��״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //��״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_ReadStatusReg1;    
            break;
    }                               //ʹ������   
    SPI1_ReadWriteByte(command);            //���Ͷ�ȡ״̬�Ĵ�������    
    byte=SPI1_ReadWriteByte(0Xff);          //��ȡһ���ֽ�    
    return byte;   
} 
//дW25QXX״̬�Ĵ���
void W25QXX_Write_SR(uint8_t regno,uint8_t sr)   
{   
    uint8_t command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //д״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //д״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //д״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_WriteStatusReg1;    
            break;
    }    
    SPI1_ReadWriteByte(command);            //����дȡ״̬�Ĵ�������    
    SPI1_ReadWriteByte(sr);                 //д��һ���ֽ�      	      
}   
//W25QXXдʹ��	
//��WEL��λ   
void W25QXX_Write_Enable(void)   
{                      
    SPI1_ReadWriteByte(W25X_WriteEnable);   //����дʹ��      	      
} 
//W25QXXд��ֹ	
//��WEL����  
void W25QXX_Write_Disable(void)   
{  
    SPI1_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��        	      
} 

//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
//0XEF18,��ʾоƬ�ͺ�ΪW25Q256
uint16_t W25QXX_ReadID(void)
{
    uint16_t Temp = 0;	  				    
    SPI1_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
    SPI1_ReadWriteByte(0x00); 	    
    SPI1_ReadWriteByte(0x00); 	    
    SPI1_ReadWriteByte(0x00); 	 			   
    Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
    Temp|=SPI1_ReadWriteByte(0xFF);	 				    
    return Temp;
}   		    
//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)   
{ 
    uint16_t i;   										      
    SPI1_ReadWriteByte(W25X_ReadData);      //���Ͷ�ȡ����  
    if(W25QXX_TYPE==W25Q128)                //�����W25Q128�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI1_ReadWriteByte((uint8_t)((ReadAddr)>>24));    
    }
    SPI1_ReadWriteByte((uint8_t)((ReadAddr)>>16));   //����24bit��ַ    
    SPI1_ReadWriteByte((uint8_t)((ReadAddr)>>8));   
    SPI1_ReadWriteByte((uint8_t)ReadAddr);   
    for(i=0; i<NumByteToRead; i++)
    { 
        pBuffer[i]=SPI1_ReadWriteByte(0XFF);    //ѭ������  
    } 				    	      
}  
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QXX_Write_Page(char* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint16_t i;  
    W25QXX_Write_Enable();                  //SET WEL 
    SPI1_ReadWriteByte(W25X_PageProgram);   //����дҳ����   
    if(W25QXX_TYPE==W25Q128)                //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>24)); 
    }
    SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ    
    SPI1_ReadWriteByte((uint8_t)((WriteAddr)>>8));   
    SPI1_ReadWriteByte((uint8_t)WriteAddr);   
    for(i=0;i<NumByteToWrite;i++)SPI1_ReadWriteByte(pBuffer[i]);//ѭ��д��  
    W25QXX_Wait_Busy();					   //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(char* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 			 		 
    uint16_t pageremain;	   
    pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
    if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
    while(1)
    {	   
        W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
        if(NumByteToWrite==pageremain)break;//д�������
        else //NumByteToWrite>pageremain
        {
            pBuffer+=pageremain;
            WriteAddr+=pageremain;	

            NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
            if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
            else pageremain=NumByteToWrite; 	  //����256���ֽ���
        }
    };	    
} 
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   
char W25QXX_BUFFER[4096];		 
void W25QXX_Write(char* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)   
{ 
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;	   
    uint16_t i;    
    char * W25QXX_BUF;	  
    W25QXX_BUF=W25QXX_BUFFER;	     
    secpos=WriteAddr/4096;//������ַ  
    secoff=WriteAddr%4096;//�������ڵ�ƫ��
    secremain=4096-secoff;//����ʣ��ռ��С   
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
    if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
    while(1) 
    {	
        W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//������������������
        for(i=0;i<secremain;i++)//У������
        {
            if(W25QXX_BUF[secoff+i] != 0XFF)break;//��Ҫ����  	  
        }
        if(i<secremain)//��Ҫ����
        {
            W25QXX_Erase_Sector(secpos);//�����������
            for(i=0;i<secremain;i++)	   //����
            {
                W25QXX_BUF[i+secoff]=pBuffer[i];	  
            }
            W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  

        }else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
        if(NumByteToWrite==secremain)break;//д�������
        else//д��δ����
        {
            secpos++;//������ַ��1
            secoff=0;//ƫ��λ��Ϊ0 	 

            pBuffer+=secremain;  //ָ��ƫ��
            WriteAddr+=secremain;//д��ַƫ��	   
            NumByteToWrite-=secremain;				//�ֽ����ݼ�
            if(NumByteToWrite>4096)secremain=4096;	        //��һ����������д����
            else secremain=NumByteToWrite;			//��һ����������д����
        }	 
    };	 
}
//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)   
{                                   
    W25QXX_Write_Enable();                  //SET WEL 
    W25QXX_Wait_Busy();     
    SPI1_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������       	      
    W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms
void W25QXX_Erase_Sector(uint32_t Dst_Addr)   
{  
    //����falsh�������,������   
    //printf("fe:%x\r\n",Dst_Addr);	  
    Dst_Addr*=4096;
    W25QXX_Write_Enable();                  //SET WEL 	 
    W25QXX_Wait_Busy();     
    SPI1_ReadWriteByte(W25X_SectorErase);   //������������ָ�� 
    if(W25QXX_TYPE==W25Q128)                //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI1_ReadWriteByte((uint8_t)((Dst_Addr)>>24)); 
    }
    SPI1_ReadWriteByte((uint8_t)((Dst_Addr)>>16));  //����24bit��ַ    
    SPI1_ReadWriteByte((uint8_t)((Dst_Addr)>>8));   
    SPI1_ReadWriteByte((uint8_t)Dst_Addr);      	      
    W25QXX_Wait_Busy();   				    //�ȴ��������
}  
//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
    while((W25QXX_ReadSR(1)&0x01)==0x01);   // �ȴ�BUSYλ���
}  
//�������ģʽ
void W25QXX_PowerDown(void)   
{  
    SPI1_ReadWriteByte(W25X_PowerDown);     //���͵�������    	      
}   
//����
void W25QXX_WAKEUP(void)   
{   
    SPI1_ReadWriteByte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB       	      
    //delay_us(3);                                //�ȴ�TRES1
}   

void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    __HAL_SPI_DISABLE(&hspi1);          
    hspi1.Instance->CR1&=0XFFC7;         
    hspi1.Instance->CR1|=SPI_BaudRatePrescaler;
    __HAL_SPI_ENABLE(&hspi1);          
    
}

uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&hspi1,&TxData,&Rxdata,1, 1000);       
    return Rxdata;       	
}

