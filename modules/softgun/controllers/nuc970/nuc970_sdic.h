/*
 **********************************************************************************
 * Emulation of NUC970 SDRAM interface controller module
 *
 * (C) 2021 Steven Deng
 *   Author: Steven Deng
 **********************************************************************************
 */

#include <bus.h>
BusDevice *NUC970_SdicNew(const char *name, BusDevice * dram);
