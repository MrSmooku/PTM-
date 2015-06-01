#ifndef _CUSTMOCHAR_H_
#define _CUSTOMCHAR_H_

#include "stm32f4xx.h"

uint8_t charTermo[] = { 0x4,0xa,0xe,0xe,0x17,0x1f,0xe};
uint8_t charCelc[] = {0x18,0x18,0x3,0x4,0x4,0x4,0x3};
uint8_t carLeft0Open[] = {	0x3,0x4,0x4,0x5,0x6,0x8,0x10 };
uint8_t carLeft1Open[] = {	0x0,0x7,0x4,0x4,0x3,0x0,0x0 };
uint8_t carR0Open[] = {	0x18,0x4,0x4,0x14,0xc,0x2,0x1 };
uint8_t carR1Open[] = {	0x0,0x1c,0x4,0x4,0x18,0x0,0x0 };
uint8_t carR0[] = {		0x18,0x4,0x4,0x14,0xc,0x4,0x4 };
uint8_t carR1[] = {	0x4,0x1c,0x4,0x4,0x18,0x0,0x0 };
uint8_t carL0[] = {	0x3,0x4,0x4,0x5,0x6,0x4,0x4 };
uint8_t carL1[] = {	0x4,0x7,0x4,0x4,0x3,0x0,0x0 };
#endif
