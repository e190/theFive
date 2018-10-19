/*
 * File      : drv_sdcard.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2017, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author         Notes
 * 2018-01-13     Liu2guang      the first version.
 */
#include "drv_sdio_sd.h"

static SD_HandleTypeDef hsdcard;
static struct rt_semaphore sd_lock;
#ifdef SD_DMA_MODE
static DMA_HandleTypeDef SDTxDMAHandler,SDRxDMAHandler;
#endif

rt_err_t sdio_sd_init(void)
{
    hsdcard.Instance                 = SDIO;
    hsdcard.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
    hsdcard.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
    hsdcard.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
    hsdcard.Init.BusWide             = SDIO_BUS_WIDE_1B;
    hsdcard.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_ENABLE;
    hsdcard.Init.ClockDiv            = SDIO_CLK_DIV;
    HAL_SD_DeInit(&hsdcard);
    if (HAL_SD_Init(&hsdcard) != HAL_OK)
    {
        rt_kprintf("HAL_SD_Init error\n");
        return RT_EIO;
    }
    if (HAL_SD_ConfigWideBusOperation(&hsdcard, SDIO_BUS_WIDE_4B) != HAL_OK)
    {
        rt_kprintf("HAL_SD_ConfigWideBusOperation error\n");
        return RT_EIO;
    }
    return RT_EOK;
}
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    DMA_HandleTypeDef TxDMAHandler,RxDMAHandler;

    __HAL_RCC_SDIO_CLK_ENABLE();
#if (SD_DMA_MODE == 1)
    __HAL_RCC_DMA2_CLK_ENABLE();
#endif
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin   = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
                            GPIO_PIN_12;
    GPIO_InitStruct.Mode  = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_InitStruct.Pin   = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

#if defined(USING_SD_RX_DMA) || defined(USING_SD_TX_DMA)
    __HAL_RCC_DMA2_CLK_ENABLE();
    hdma.Instance                 = DMA2_Channel4;
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hdma.Init.Mode                = DMA_NORMAL;
    hdma.Init.Priority            = DMA_PRIORITY_HIGH;
#if defined(USING_SD_RX_DMA)
    hdma.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    hdma.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma.Init.MemInc              = DMA_MINC_ENABLE;
    __HAL_LINKDMA(&hsdcard, hdmarx, hdma);
#endif
#if defined(USING_SD_TX_DMA)
    hdma.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma.Init.PeriphInc           = DMA_MINC_ENABLE;
    hdma.Init.MemInc              = DMA_PINC_DISABLE;
    __HAL_LINKDMA(&hsdcard, hdmatx, hdma);
#endif
    HAL_DMA_DeInit(&hdma);
    if (HAL_DMA_Init(&hdma) != HAL_OK)
    {
        rt_kprintf("HAL_DMA_Init error\n");
        return RT_EIO;
    }
#endif
#if (SD_DMA_MODE==1)                        //使用DMA模式
    HAL_NVIC_SetPriority(SDMMC1_IRQn,2,0);  //配置SDMMC1中断，抢占优先级2，子优先级0
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);        //使能SDMMC1中断

    //配置发送DMA
    SDRxDMAHandler.Instance = DMA2_Stream3;
    SDRxDMAHandler.Init.Channel = DMA_CHANNEL_4;
    SDRxDMAHandler.Init.Direction = DMA_PERIPH_TO_MEMORY;
    SDRxDMAHandler.Init.PeriphInc = DMA_PINC_DISABLE;
    SDRxDMAHandler.Init.MemInc = DMA_MINC_ENABLE;
    SDRxDMAHandler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    SDRxDMAHandler.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    SDRxDMAHandler.Init.Mode = DMA_PFCTRL;
    SDRxDMAHandler.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    SDRxDMAHandler.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    SDRxDMAHandler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    SDRxDMAHandler.Init.MemBurst = DMA_MBURST_INC4;
    SDRxDMAHandler.Init.PeriphBurst = DMA_PBURST_INC4;

    __HAL_LINKDMA(hsd, hdmarx, SDRxDMAHandler); //将接收DMA和SD卡的发送DMA连接起来
    HAL_DMA_DeInit(&SDRxDMAHandler);
    HAL_DMA_Init(&SDRxDMAHandler);              //初始化接收DMA

    //配置接收DMA
    SDTxDMAHandler.Instance = DMA2_Stream6;
    SDTxDMAHandler.Init.Channel = DMA_CHANNEL_4;
    SDTxDMAHandler.Init.Direction = DMA_MEMORY_TO_PERIPH;
    SDTxDMAHandler.Init.PeriphInc = DMA_PINC_DISABLE;
    SDTxDMAHandler.Init.MemInc = DMA_MINC_ENABLE;
    SDTxDMAHandler.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    SDTxDMAHandler.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    SDTxDMAHandler.Init.Mode = DMA_PFCTRL;
    SDTxDMAHandler.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    SDTxDMAHandler.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    SDTxDMAHandler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    SDTxDMAHandler.Init.MemBurst = DMA_MBURST_INC4;
    SDTxDMAHandler.Init.PeriphBurst = DMA_PBURST_INC4;

    __HAL_LINKDMA(hsd, hdmatx, SDTxDMAHandler);//将发送DMA和SD卡的发送DMA连接起来
    HAL_DMA_DeInit(&SDTxDMAHandler);
    HAL_DMA_Init(&SDTxDMAHandler);              //初始化发送DMA


    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 3, 0);  //接收DMA中断优先级
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 3, 0);  //发送DMA中断优先级
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
#endif

    HAL_NVIC_SetPriority(SDIO_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
}
void SDIO_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_SD_IRQHandler(&hsdcard);
    rt_interrupt_leave();
}

#if defined(USING_SD_RX_DMA) || defined(USING_SD_TX_DMA)
void DMA2_Channel4_5_IRQHandler(void)
{
    rt_interrupt_enter();
    HAL_DMA_IRQHandler(&hdma);
    rt_interrupt_leave();
}
#endif

rt_err_t stm32_read_blocks(uint32_t *data, uint32_t addr, uint32_t num)
{
    uint32_t timeout = 0;
    HAL_SD_StateTypeDef state_return;
    HAL_SD_CardStateTypeDef sd_card_state_return;
#if defined(USING_SD_RX_DMA) && defined(USING_SD_TX_DMA)
    hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma.Init.MemInc    = DMA_MINC_ENABLE;
    HAL_DMA_DeInit(&hdma);
    HAL_DMA_Init(&hdma);
#endif
#if defined(USING_SD_RX_DMA)
    if (HAL_SD_ReadBlocks_DMA(&hsdcard, (uint8_t *)data, addr, num) != HAL_OK)
#else
    if (HAL_SD_ReadBlocks(&hsdcard, (uint8_t *)data, addr, num, SDIO_TIMEOUT) != HAL_OK)
#endif
    {
        return RT_EIO;
    }
    do
    {
        state_return = HAL_SD_GetState(&hsdcard);
        timeout++;
    }
    while ((HAL_SD_STATE_BUSY == state_return) && (SDIO_TIMEOUT > timeout));
    if (HAL_SD_STATE_READY != state_return)
    {
        return RT_ERROR;
    }
    do
    {
        sd_card_state_return = HAL_SD_GetCardState(&hsdcard);
        timeout++;
    }
    while ((HAL_SD_CARD_TRANSFER != sd_card_state_return) && (SDIO_TIMEOUT > timeout));
    if (SDIO_TIMEOUT <= timeout)
    {
        return RT_ETIMEOUT;
    }
    return RT_EOK;
}

rt_err_t stm32_write_blocks(uint32_t *data, uint32_t addr, uint32_t num)
{
    uint32_t timeout = 0;
    HAL_SD_StateTypeDef state_return;
    HAL_SD_CardStateTypeDef sd_card_state_return;
#if defined(USING_SD_RX_DMA) && defined(USING_SD_TX_DMA)
    hdma.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma.Init.PeriphInc = DMA_MINC_ENABLE;
    hdma.Init.MemInc    = DMA_PINC_DISABLE;
    HAL_DMA_DeInit(&hdma);
    HAL_DMA_Init(&hdma);
#endif
#if defined(USING_SD_TX_DMA)
    if (HAL_SD_WriteBlocks_DMA(&hsdcard, (uint8_t *)data, addr, num) != HAL_OK)
#else
    if (HAL_SD_WriteBlocks(&hsdcard, (uint8_t *)data, addr, num, SDIO_TIMEOUT) != HAL_OK)
#endif
    {
        return RT_ERROR;
    }
    do
    {
        state_return = HAL_SD_GetState(&hsdcard);
        timeout++;
    }
    while ((HAL_SD_STATE_BUSY == state_return) && (SDIO_TIMEOUT > timeout));
    if (HAL_SD_STATE_READY != state_return)
    {
        return RT_ERROR;
    }
    do
    {
        sd_card_state_return = HAL_SD_GetCardState(&hsdcard);
        timeout++;
    }
    while ((HAL_SD_CARD_TRANSFER != sd_card_state_return) && (SDIO_TIMEOUT > timeout));
    if (SDIO_TIMEOUT <= timeout)
    {
        return RT_ETIMEOUT;
    }
    return RT_EOK;
}

static rt_err_t stm32_sdcard_init(rt_device_t dev)
{
    if (rt_sem_init(&sd_lock, "sdlock", 1, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
    	DEBUG_PRINTF("init sd lock semaphore failed\n");
        return RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t stm32_sdcard_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t stm32_sdcard_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_size_t stm32_sdcard_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    int ret = RT_EOK;
    rt_sem_take(&sd_lock, RT_WAITING_FOREVER);
    ret = stm32_read_blocks((uint32_t *)buffer, pos, size);
    rt_sem_release(&sd_lock);
    if (ret != RT_EOK)
    {
        return 0;
    }
    return size;
}

static rt_size_t stm32_sdcard_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    int ret = RT_EOK;
    rt_sem_take(&sd_lock, RT_WAITING_FOREVER);
    ret = stm32_write_blocks((uint32_t *)buffer, pos, size);
    rt_sem_release(&sd_lock);
    if (ret != RT_EOK)
    {
        return 0;
    }
    return size;
}

static rt_err_t stm32_sdcard_control(rt_device_t dev, int cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);
    // RT_DEVICE_CTRL_BLK_GETGEOME
    if (cmd == RT_DEVICE_CTRL_BLK_GETGEOME)
    {
        HAL_SD_CardInfoTypeDef sdcard_info;
        struct rt_device_blk_geometry *geometry;
        HAL_SD_GetCardInfo(&hsdcard, &sdcard_info);
        geometry = (struct rt_device_blk_geometry *)args;
        geometry->bytes_per_sector = sdcard_info.BlockSize;
        geometry->block_size       = sdcard_info.BlockSize;
        geometry->sector_count     = sdcard_info.BlockNbr;
    }
    return RT_EOK;
}

static struct rt_device sdcard_device;

int rt_hw_sdcard_init(void)
{
    rt_err_t ret = RT_EOK;

	if (sdio_sd_init() != RT_EOK)
	{
        DEBUG_PRINTF("sdcard init failed\n");
        return RT_ERROR;
    }

	sdcard_device.type    = RT_Device_Class_Block;
	sdcard_device.init    = stm32_sdcard_init;
	sdcard_device.open    = stm32_sdcard_open;
	sdcard_device.read    = stm32_sdcard_read;
	sdcard_device.write   = stm32_sdcard_write;
	sdcard_device.control = stm32_sdcard_control;
	sdcard_device.close   = stm32_sdcard_close;
    ret = rt_device_register(&sdcard_device, "sd0",
                             RT_DEVICE_FLAG_REMOVABLE |
                             RT_DEVICE_FLAG_RDWR      |
                             RT_DEVICE_FLAG_STANDALONE);
    if (ret != RT_EOK)
    {
        return ret;
    }
    return RT_EOK;
}
INIT_DEVICE_EXPORT(rt_hw_sdcard_init);
