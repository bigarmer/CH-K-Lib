/* * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "i2c.h"
#include "i2s.h"
#include "wm8960.h"

static uint8_t buf[4096];

int main(void)
{
    int i;
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PA01_TX_PA02, 115200);
    
     /** print message before mode change*/
    printf("uart will be send on interrupt mode...\r\n");
    I2C_QuickInit(I2C0_SCL_PE24_SDA_PE25, 100*1000);
   // I2C_Scan(I2C0_SCL_PE24_SDA_PE25);
    wm8960_init(0);
   // wm8960_set_volume(kWolfsonModuleHP, 0x2FFF);
    
    for(i=0; i<sizeof(buf); i++)
    {
        buf[i] = i & 0xFF;
    }
    
    I2S_InitTypeDef Init;
    Init.instance = 0;
    Init.isStereo = true;
    Init.isMaster = true;
    Init.protocol = kSaiBusI2SLeft;
    Init.sampleBit = 16;
    Init.sampleRate = 11025;
    Init.chl = 0;
    I2S_Init(&Init);
    
    while(1)
    {

        I2S_SendData(0, 16, 0, buf, sizeof(buf));
        printf("HelloWorld\r\n");
      //  DelayMs(500);
    }
}
