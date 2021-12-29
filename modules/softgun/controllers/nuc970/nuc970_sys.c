/*
 *************************************************************************************************
 *
 * Emulation of NUC970 System Global Control Register
 *
 * state: working
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

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include "bus.h"
#include "signode.h"
#include "nuc970_sys.h"
#include "configfile.h"
#include "clock.h"
#include "cycletimer.h"
#include "sgstring.h"

#include "driver/nuc970.h"

typedef struct NUC970_Sys {
	BusDevice bdev;
	char *name;

	/* sysctl */
	uint32_t SYS_PDID;		//     (SYS_BA+0x000)  /*!< Product Identifier Register */
	uint32_t SYS_PWRON;		//     (SYS_BA+0x004)  /*!< Power-On Setting Register */
	uint32_t SYS_ARBCON;	//     (SYS_BA+0x008)  /*!< Arbitration Control Register */
	uint32_t SYS_LVRDCR;	//     (SYS_BA+0x020)  /*!< Low Voltage Reset & Detect Control Register */
	uint32_t SYS_MISCFCR;	//     (SYS_BA+0x030)  /*!< Miscellaneous Function Control Register */
	uint32_t SYS_MISCIER;	//     (SYS_BA+0x040)  /*!< Miscellaneous Interrupt Enable Register */
	uint32_t SYS_MISCISR;	//     (SYS_BA+0x044)  /*!< Miscellaneous Interrupt Status Register */
	uint32_t SYS_WKUPSER;	//     (SYS_BA+0x058)  /*!< System Wakeup Source Enable Register */
	uint32_t SYS_WKUPSSR;	//     (SYS_BA+0x05C)  /*!< System Wakeup Source Status Register */
	uint32_t SYS_AHBIPRST;	//     (SYS_BA+0x060)  /*!< AHB IP Reset Control Register */
	uint32_t SYS_APBIPRST0;	//     (SYS_BA+0x064)  /*!< APB IP Reset Control Register 0 */
	uint32_t SYS_APBIPRST1;	//     (SYS_BA+0x068)  /*!< APB IP Reset Control Register 1 */
	uint32_t SYS_RSTSTS;	//     (SYS_BA+0x06C)  /*!< Reset Source Active Status Register */
	uint32_t SYS_GPA_MFPL;	//     (SYS_BA+0x070)  /*!< GPIOA Low Byte Multiple Function Control Register */
	uint32_t SYS_GPA_MFPH;	//     (SYS_BA+0x074)  /*!< GPIOA High Byte Multiple Function Control Register */
	uint32_t SYS_GPB_MFPL;	//     (SYS_BA+0x078)  /*!< GPIOB Low Byte Multiple Function Control Register */
	uint32_t SYS_GPB_MFPH;	//     (SYS_BA+0x07C)  /*!< GPIOB High Byte Multiple Function Control Register */
	uint32_t SYS_GPC_MFPL;	//     (SYS_BA+0x080)  /*!< GPIOC Low Byte Multiple Function Control Register */
	uint32_t SYS_GPC_MFPH;	//     (SYS_BA+0x084)  /*!< GPIOC High Byte Multiple Function Control Register */
	uint32_t SYS_GPD_MFPL;	//     (SYS_BA+0x088)  /*!< GPIOD Low Byte Multiple Function Control Register */
	uint32_t SYS_GPD_MFPH;	//     (SYS_BA+0x08C)  /*!< GPIOD High Byte Multiple Function Control Register */
	uint32_t SYS_GPE_MFPL;	//     (SYS_BA+0x090)  /*!< GPIOE Low Byte Multiple Function Control Register */
	uint32_t SYS_GPE_MFPH;	//     (SYS_BA+0x094)  /*!< GPIOE High Byte Multiple Function Control Register */
	uint32_t SYS_GPF_MFPL;	//     (SYS_BA+0x098)  /*!< GPIOF Low Byte Multiple Function Control Register */
	uint32_t SYS_GPF_MFPH;	//     (SYS_BA+0x09C)  /*!< GPIOF High Byte Multiple Function Control Register */
	uint32_t SYS_GPG_MFPL;	//     (SYS_BA+0x0A0)  /*!< GPIOG Low Byte Multiple Function Control Register */
	uint32_t SYS_GPG_MFPH;	//     (SYS_BA+0x0A4)  /*!< GPIOG High Byte Multiple Function Control Register */
	uint32_t SYS_GPH_MFPL;	//     (SYS_BA+0x0A8)  /*!< GPIOH Low Byte Multiple Function Control Register */
	uint32_t SYS_GPH_MFPH;	//     (SYS_BA+0x0AC)  /*!< GPIOH High Byte Multiple Function Control Register */
	uint32_t SYS_GPI_MFPL;	//     (SYS_BA+0x0B0)  /*!< GPIOI Low Byte Multiple Function Control Register */
	uint32_t SYS_GPI_MFPH;	//     (SYS_BA+0x0B4)  /*!< GPIOI High Byte Multiple Function Control Register */
	uint32_t SYS_GPJ_MFPL;	//     (SYS_BA+0x0B8)  /*!< GPIOJ Low Byte Multiple Function Control Register */
	uint32_t SYS_DDR_DSCTL;	//     (SYS_BA+0x0F0)  /*!< DDR I/O Driving Strength Control Register */
	uint32_t SYS_PORDISCR;	//     (SYS_BA+0x100)  /*!< Power-On-Reset Disable Control Register */
	uint32_t SYS_ICEDBGCR;	//     (SYS_BA+0x104)  /*!< ICE Debug Interface Control Register */
	uint32_t SYS_ERRADDCR;	//     (SYS_BA+0x108)  /*!< Error Response Address Control Regsiter */
	uint32_t SYS_REGWPCTL;	//     (SYS_BA+0x1FC)

} NUC970_Sys;

static uint32_t
SYS_PDID_read(void *clientData, uint32_t address, int rqlen)
{
	NUC970_Sys *sys = (NUC970_Sys *) clientData;
	return sys->SYS_PDID;
}

static void
SYS_PDID_write(void *clientData, uint32_t value, uint32_t address, int rqlen)
{
	NUC970_Sys *sys = (NUC970_Sys *) clientData;
	sys->SYS_PDID = value;
	return;
}

static uint32_t SYS_GPE_MFPL_read(void *clientData, uint32_t address, int reqlen)
{
	NUC970_Sys *sys = (NUC970_Sys*)clientData;
	return sys->SYS_GPE_MFPL;
}

static void SYS_GPE_MFPL_write(void *clientData, uint32_t value, uint32_t address, int reqlen)
{
	NUC970_Sys *sys = (NUC970_Sys*)clientData;
	sys->SYS_GPE_MFPL = value;
	return;
}

static uint32_t
SYS_REGWPCTL_read(void *clientData, uint32_t address, int rqlen)
{
	NUC970_Sys *sys = (NUC970_Sys *) clientData;
	return sys->SYS_REGWPCTL;
}

static void
SYS_REGWPCTL_write(void *clientData, uint32_t value, uint32_t address, int rqlen)
{
	NUC970_Sys *sys = (NUC970_Sys *) clientData;
	sys->SYS_REGWPCTL = value;
	return;
}

#define DEFINE_IOH(sys, reg) IOH_New32(REG_ ## reg, reg ## _read, reg ## _write, sys)

static void
NUC970Sys_Map(void *owner, uint32_t base, uint32_t mask, uint32_t flags)
{
	NUC970_Sys *sys = (NUC970_Sys *) owner;
	
	DEFINE_IOH(sys, SYS_PDID);

	DEFINE_IOH(sys, SYS_GPE_MFPL);

	DEFINE_IOH(sys, SYS_REGWPCTL);
	
}

static void
NUC970Sys_UnMap(void *owner, uint32_t base, uint32_t mask)
{
	IOH_Delete32(REG_SYS_PDID);
	IOH_Delete32(REG_SYS_PWRON);
	IOH_Delete32(REG_SYS_ARBCON);
	IOH_Delete32(REG_SYS_LVRDCR);
	IOH_Delete32(REG_SYS_MISCFCR);
	IOH_Delete32(REG_SYS_MISCIER);
	IOH_Delete32(REG_SYS_MISCISR);
	IOH_Delete32(REG_SYS_WKUPSER);
	IOH_Delete32(REG_SYS_WKUPSSR);
	IOH_Delete32(REG_SYS_AHBIPRST);
	IOH_Delete32(REG_SYS_APBIPRST0);
	IOH_Delete32(REG_SYS_APBIPRST1);
	IOH_Delete32(REG_SYS_RSTSTS);
	IOH_Delete32(REG_SYS_GPA_MFPL);
	IOH_Delete32(REG_SYS_GPA_MFPH);
	IOH_Delete32(REG_SYS_GPB_MFPL);
	IOH_Delete32(REG_SYS_GPB_MFPH);
	IOH_Delete32(REG_SYS_GPC_MFPL);
	IOH_Delete32(REG_SYS_GPC_MFPH);
	IOH_Delete32(REG_SYS_GPD_MFPL);
	IOH_Delete32(REG_SYS_GPD_MFPH);
	IOH_Delete32(REG_SYS_GPE_MFPL);
	IOH_Delete32(REG_SYS_GPE_MFPH);
	IOH_Delete32(REG_SYS_GPF_MFPL);
	IOH_Delete32(REG_SYS_GPF_MFPH);
	IOH_Delete32(REG_SYS_GPG_MFPL);
	IOH_Delete32(REG_SYS_GPG_MFPH);
	IOH_Delete32(REG_SYS_GPH_MFPL);
	IOH_Delete32(REG_SYS_GPH_MFPH);
	IOH_Delete32(REG_SYS_GPI_MFPL);
	IOH_Delete32(REG_SYS_GPI_MFPH);
	IOH_Delete32(REG_SYS_GPJ_MFPL);
	IOH_Delete32(REG_SYS_DDR_DSCTL);
	IOH_Delete32(REG_SYS_PORDISCR);
	IOH_Delete32(REG_SYS_ICEDBGCR);
	IOH_Delete32(REG_SYS_ERRADDCR);
	IOH_Delete32(REG_SYS_REGWPCTL);
}

BusDevice *
NUC970Sys_New(const char *name)
{
	NUC970_Sys *sys = sg_new(NUC970_Sys);

	sys->name = sg_strdup(name);
	if (!sys->name) {
		fprintf(stderr, "Out of memory for SYS name\n");
		exit(1);
	}
	
	
	sys->bdev.first_mapping = NULL;
	sys->bdev.Map = NUC970Sys_Map;
	sys->bdev.UnMap = NUC970Sys_UnMap;
	sys->bdev.owner = sys;
	sys->bdev.hw_flags = MEM_FLAG_WRITABLE | MEM_FLAG_READABLE;

	fprintf(stderr, "NUC970 System Global Control module \"%s\" created\n", name);

	return &sys->bdev;
}
