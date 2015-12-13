//*****************************************************************************
// gamelogic.c
// Author:
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "TM4C123.h"
#include "eeprom.h"
#include "board_util.h"
#include "uart_irqs.h"
#include "uart.h"
#include "gpio_port.h"
#include "adc.h"
#include "wireless.h"
#include "timers.h"
#include "interrupts.h"
#include "lcd.h"

#define BTN_UP 0x1
#define BTN_DOWN 0x2
#define BTN_LEFT 0x4
#define BTN_RIGHT 0x8
#define BTN_PS2 0x10
#define PS2_UP 0x20
#define PS2_DOWN 0x40
#define PS2_LEFT 0x80
#define PS2_RIGHT 0x100

extern volatile bool initState;

void read_buttons(uint16_t *pressed);
int random_generate(void);
