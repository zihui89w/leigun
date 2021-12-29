//===-- boards/nuc970.c -----------------------------------------*- C -*-===//
//
//              The Leigun Embedded System Simulator Platform : modules
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//===----------------------------------------------------------------------===//
///
/// @file
/// Compose a Nuvoton NUC970 Board
///
//===----------------------------------------------------------------------===//

// clang-format off
/*
 *************************************************************************************************
 * Compose a Nuvoton NUC970 Board
 *
 * Copyright 2006 Jochen Karrer. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY Jochen Karrer ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Jochen Karrer.
 *
 *************************************************************************************************
 */
// clang-format on

//==============================================================================
//= Dependencies
//==============================================================================
// Main Module Header

// Local/Private Headers
#include "nuc970_aic.h"
#include "amdflash.h"
#include "arm/mmu_arm9.h"
#include "controllers/nuc970/driver/nuc970.h"
#include "controllers/nuc970/nuc970_clk.h"
#include "controllers/nuc970/nuc970_sys.h"
#include "controllers/imx21/imx21_cspi.h"
#include "controllers/imx21/imx21_dmac.h"
#include "controllers/imx21/imx21_eim.h"
#include "controllers/nuc970/nuc970_gpio.h"
#include "controllers/imx21/imx21_i2c.h"
#include "controllers/imx21/imx21_lcdc.h"
#include "controllers/imx21/imx21_max.h"
#include "controllers/imx21/imx21_otg.h"
#include "controllers/imx21/imx21_pwm.h"
#include "controllers/imx21/imx21_rtc.h"
#include "controllers/imx21/imx21_sdhc.h"
#include "controllers/nuc970/nuc970_sdic.h"
#include "controllers/nuc970/nuc970_uart.h"
#include "controllers/imx21/imx21_wdog.h"
#include "cs8900.h"
#include "devices/sdcard/mmcard.h"
#include "djet460.h"
#include "nuc970_timer.h"

// Leigun Core Headers
#include "bus.h"
#include "dram.h"
#include "fbdisplay.h"
#include "mmcdev.h"
#include "rfbserver.h"
#include "signode.h"
#include "sram.h"
#include "leigun/leigun.h"
#include "leigun/device.h"

// External headers

// System headers
#include <stdio.h>


//==============================================================================
//= Constants(also Enumerations)
//==============================================================================
#define BOARD_NAME "NUC970"
#define BOARD_DESCRIPTION "NUC970"
#define BOARD_DEFAULTCONFIG                                                    \
    "[global]\n"                                                               \
    "start_address: 0xe00000\n"                                              \
    "cpu_clock: 300000000\n"                                                   \
    "\n"                                                                       \
    "[dram0]\n"                                                                \
    "size: 64M\n"                                                              \
    "\n"                                                                       \
    "[flash0]\n"                                                               \
    "type: AM29BDS128H\n"                                                      \
    "chips: 1\n"                                                               \
    "\n"                                                                       \
    "[vram]\n"                                                                 \
    "size: 56k\n"


//==============================================================================
//= Types
//==============================================================================
typedef struct board_s {
    Device_Board_t base;
    Device_MPU_t *mpu;
} board_t;


//==============================================================================
//= Variables
//==============================================================================


//==============================================================================
//= Function declarations(static)
//==============================================================================
static void create_clock_links(void);
static void create_signal_links(void);
static Device_Board_t *create(void);


//==============================================================================
//= Function definitions(static)
//==============================================================================

static void create_clock_links(void) {
    Clock_Link("pwm.clk", "crm.perclk1");
    Clock_Link("pwm.clk32", "crm.clk32");

    Clock_Link("lcdc.clk", "crm.perclk3");

    Clock_Link("sdhc1.inclk", "crm.perclk2");
    Clock_Link("sdhc2.inclk", "crm.perclk2");

    Clock_Link("cspi1.clk", "crm.perclk2");
    Clock_Link("cspi2.clk", "crm.perclk2");
    Clock_Link("cspi3.clk", "crm.perclk2");

    Clock_Link("uart1.clk", "crm.perclk1");
    Clock_Link("uart2.clk", "crm.perclk1");
    Clock_Link("uart3.clk", "crm.perclk1");
    Clock_Link("uart4.clk", "crm.perclk1");
}

static void create_signal_links(void) {
    int i;
    /* Connect the interrupt controller to the CPU */
    SigName_Link("arm.irq", "aitc.irq");
    SigName_Link("arm.fiq", "aitc.fiq");

    /* Connect the devices to the Interrupt controller */
    SigName_Link("aitc.nIntSrc6", "cspi3.irq");
    // SigName_Link("aitc.nIntSrc7","rnga");
    SigName_Link("aitc.nIntSrc8", "gpio.irq");
    // SigName_Link("aitc.nIntSrc9","firi");
    SigName_Link("aitc.nIntSrc10", "sdhc2.irq");
    SigName_Link("aitc.nIntSrc11", "sdhc1.irq");
    SigName_Link("aitc.nIntSrc12", "i2c.irq");
    // SigName_Link("aitc.nIntSrc13","ssi2");
    // SigName_Link("aitc.nIntSrc14","ssi1");
    SigName_Link("aitc.nIntSrc15", "cspi2.irq");
    SigName_Link("aitc.nIntSrc16", "cspi1.irq");
    SigName_Link("aitc.nIntSrc17", "uart4.irq");
    SigName_Link("aitc.nIntSrc18", "uart3.irq");
    SigName_Link("aitc.nIntSrc19", "uart2.irq");
    SigName_Link("aitc.nIntSrc20", "uart1.irq");
    // SigName_Link("aitc.nIntSrc21","kpp");
    SigName_Link("aitc.nIntSrc22", "rtc.irq");
    SigName_Link("aitc.nIntSrc23", "pwm.irq");
    SigName_Link("aitc.nIntSrc24", "gpt3.irq");
    SigName_Link("aitc.nIntSrc25", "gpt2.irq");
    SigName_Link("aitc.nIntSrc26", "gpt1.irq");
    // SigName_Link("aitc.nIntSrc27","wdog");
    // SigName_Link("aitc.nIntSrc28","pcmcia");
    // SigName_Link("aitc.nIntSrc29","nfc");
    // SigName_Link("aitc.nIntSrc30","bmi");
    // SigName_Link("aitc.nIntSrc31","csi");
    for (i = 0; i < 16; i++) {
        char name1[100], name2[100];
        sprintf(name1, "aitc.nIntSrc%d", i + 32);
        sprintf(name2, "dmac.irq%d", i);
        SigName_Link(name1, name2);
    }
    // SigName_Link("aitc.nIntSrc49","emmaenc");
    // SigName_Link("aitc.nIntSrc50","emmadec");
    // SigName_Link("aitc.nIntSrc51","emmaprp");
    // SigName_Link("aitc.nIntSrc52","emmapp");
    SigName_Link("aitc.nIntSrc53", "otg.intWkup");
    SigName_Link("aitc.nIntSrc54", "otg.intDma");
    SigName_Link("aitc.nIntSrc55", "otg.intHost");
    SigName_Link("aitc.nIntSrc56", "otg.intFunc");
    SigName_Link("aitc.nIntSrc57", "otg.intMnp");
    SigName_Link("aitc.nIntSrc58", "otg.intCtrl");
    // SigName_Link("aitc.nIntSrc53","usbwkup");
    // SigName_Link("aitc.nIntSrc54","usbdma");
    // SigName_Link("aitc.nIntSrc55","usbhost");
    // SigName_Link("aitc.nIntSrc56","usbfunc");
    // SigName_Link("aitc.nIntSrc57","usbmnp");
    // SigName_Link("aitc.nIntSrc58","usbctrl");
    // SigName_Link("aitc.nIntSrc60","slcdc");
    SigName_Link("aitc.nIntSrc61", "lcdc.irq");

    /* DMA requests */
    SigName_Link("dmac.dma_req1", "cspi3.rx_dmareq");
    SigName_Link("dmac.dma_req2", "cspi3.tx_dmareq");
    // SigName_Link("dmac.dma_req3","external dmareq");
    // SigName_Link("dmac.dma_req4","firi.rx_dmareq");
    // SigName_Link("dmac.dma_req5","firi.tx_dmareq");
    SigName_Link("dmac.dma_req6", "sdhc2.dma_req");
    SigName_Link("dmac.dma_req7", "sdhc1.dma_req");
    // SigName_Link("dmac.dma_req8","ssi2.rx0_dmareq");
    // SigName_Link("dmac.dma_req9","ssi2.tx0_dmareq");
    // SigName_Link("dmac.dma_req10","ssi2.rx1_dmareq");
    // SigName_Link("dmac.dma_req11","ssi2.tx1_dmareq");
    // SigName_Link("dmac.dma_req12","ssi1.rx0_dmareq");
    // SigName_Link("dmac.dma_req13","ssi1.tx0_dmareq");
    // SigName_Link("dmac.dma_req14","ssi1.rx1_dmareq");
    // SigName_Link("dmac.dma_req15","ssi1.tx1_dmareq");
    SigName_Link("dmac.dma_req16", "cspi2.rx_dmareq");
    SigName_Link("dmac.dma_req17", "cspi2.tx_dmareq");
    SigName_Link("dmac.dma_req18", "cspi1.rx_dmareq");
    SigName_Link("dmac.dma_req19", "cspi1.tx_dmareq");
    SigName_Link("dmac.dma_req20", "uart4.rx_dmareq");
    SigName_Link("dmac.dma_req21", "uart4.tx_dmareq");
    SigName_Link("dmac.dma_req22", "uart3.rx_dmareq");
    SigName_Link("dmac.dma_req23", "uart3.tx_dmareq");
    SigName_Link("dmac.dma_req24", "uart2.rx_dmareq");
    SigName_Link("dmac.dma_req25", "uart2.tx_dmareq");
    SigName_Link("dmac.dma_req26", "uart1.rx_dmareq");
    SigName_Link("dmac.dma_req27", "uart1.tx_dmareq");
    // SigName_Link("dmac.dma_req28","bmi.tx_dmareq");
    // SigName_Link("dmac.dma_req29","bmi.rx_dmareq");
    // SigName_Link("dmac.dma_req30","csi.stat_dmareq");
    // SigName_Link("dmac.dma_req31","csi.rx_dmareq");

    /* External components */
    SigName_Link("imx21.l17", "cs8900.intrq0");

    SigName_Link("imx21.h11", "GND"); /* SD Card 1 detect */
}

static Device_Board_t *create(void) {
    ArmCoprocessor *copro;
    BusDevice *dev;
    BusDevice *dram0 = NULL;
    MMCDev *mmcard;
    FbDisplay *display;
    Keyboard *keyboard;
    UsbDevice *usbdev;
    board_t *board = LEIGUN_NEW(board);
    board->base.self = board;

    Bus_Init(MMU_InvalidateTlb, 1 * 1024);
    board->mpu = Device_CreateMPU("ARM9");
    copro = MMU9_Create("mmu", BYTE_ORDER_LITTLE, MMU_ARM926EJS);
    ARM9_RegisterCoprocessor(copro, 15);

    /* Currently I have no dram controller */
    dram0 = dev = DRam_New("dram0");
    if (dev) {
        Mem_AreaAddMapping(dev, 0x0000000, 0x04000000,
                           MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    }

    dev = AMDFlashBank_New("flash0");
    if (dev) {
        Mem_AreaAddMapping(dev, 0xC8000000, 0x04000000,
                           MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    }
    dev = IMX21_DmacNew("dmac");
    Mem_AreaAddMapping(dev, 0x10001000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = IMX21_WdogNew("wdog");
    Mem_AreaAddMapping(dev, 0x10002000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = NUC970Uart_New("uart1");
    Mem_AreaAddMapping(dev, UART0_BA, 0x0000100,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = NUC970Uart_New("uart2");
    Mem_AreaAddMapping(dev, UART1_BA, 0x0000100,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = NUC970Uart_New("uart3");
    Mem_AreaAddMapping(dev, UART2_BA, 0x0000100,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = NUC970Uart_New("uart4");
    Mem_AreaAddMapping(dev, UART3_BA, 0x0000100,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    dev = NUC970Timer_New("gpt1");
    Mem_AreaAddMapping(dev, TMR0_BA, 0x0000100,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = NUC970Timer_New("gpt2");
    Mem_AreaAddMapping(dev, 0x10004000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = NUC970Timer_New("gpt3");
    Mem_AreaAddMapping(dev, 0x10005000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = IMX21_PwmNew("pwm");
    Mem_AreaAddMapping(dev, 0x10006000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = IMX21_RtcNew("rtc");
    Mem_AreaAddMapping(dev, 0x10007000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    dev = IMX21_CspiNew("cspi1");
    Mem_AreaAddMapping(dev, 0x1000e000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = IMX21_CspiNew("cspi2");
    Mem_AreaAddMapping(dev, 0x1000f000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    dev = IMX21_I2cNew("i2c");
    Mem_AreaAddMapping(dev, 0x10012000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    dev = IMX21_SdhcNew("sdhc1");
    Mem_AreaAddMapping(dev, 0x10013000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    mmcard = MMCard_New("sdcard0");
    if (mmcard) {
        IMX21Sdhc_InsertCard(dev, mmcard);
    }

    dev = IMX21_SdhcNew("sdhc2");
    Mem_AreaAddMapping(dev, 0x10014000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    dev = NUC970_GpioNew("gpio");
    Mem_AreaAddMapping(dev, GPIO_BA, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    dev = IMX21_CspiNew("cspi3");
    Mem_AreaAddMapping(dev, 0x10017000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    RfbServer_New("display0", &display, &keyboard, NULL);
    dev = IMX21_LcdcNew("lcdc", display);
    Mem_AreaAddMapping(dev, 0x10021000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    dev = IMX21Otg_New("otg");
    DJet460_New("dj460", &usbdev);
    IMX21Otg_Plug(dev, usbdev, 0);

    /* IMX21Otg_Plug(dev,usbdevice,0); */
    Mem_AreaAddMapping(dev, 0x10024000, 0x00002000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = NUC970Clk_New("crm");
    Mem_AreaAddMapping(dev, CLK_BA, 0x0000100,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = NUC970Sys_New("sys");
    Mem_AreaAddMapping(dev, SYS_BA, 0x200, MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    dev = IMX21_MaxNew("max");
    Mem_AreaAddMapping(dev, 0x1003f000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = NUC970Aic_New("aitc");
    Mem_AreaAddMapping(dev, AIC_BA, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    dev = SRam_New("vram"); // Internal SRAM (56 KB)
    Mem_AreaAddMapping(dev, 0xbc000000, 0x0000e000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    /* Chips on base board */
    dev = CS8900_New("cs8900");
    Mem_AreaAddMapping(dev, 0xcc000000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    /* SDRAM interface controller module (SDIC) */
    dev = NUC970_SdicNew("sdic", dram0);
    Mem_AreaAddMapping(dev, SDIC_BA, 0x00000800, /* 0xB000_1800 - 0xB000_1FFF */
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);
    /* External memory interface module (EIM) */
    dev = IMX21_EimNew("eim");
    Mem_AreaAddMapping(dev, 0xDF001000, 0x00001000,
                       MEM_FLAG_WRITABLE | MEM_FLAG_READABLE);

    create_signal_links();
    create_clock_links();
    return &board->base;
}


//==============================================================================
//= Function definitions(global)
//==============================================================================
DEVICE_REGISTER_BOARD(BOARD_NAME, BOARD_DESCRIPTION, NULL, NULL, NULL, NULL,
                      NULL, &create, BOARD_DEFAULTCONFIG);
