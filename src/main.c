/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h> // 確保有引入 kernel.h 來使用 k_msleep

#include "gpio_test.h"
#include "uart_test.h"

int main(void)
{
    static uint32_t sys_cnt = 0;

	// 強制開啟 STM32 在 Sleep, Stop, Standby 模式下的除錯時脈
    *(volatile uint32_t *)0xE0042004 |= 0x07;

	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);
	
    custom_gpio_init();

    custom_uart_init();

	while(1)
	{
        custom_gpio_toggle_handle();

        custom_uart_test();

		printf("JACK my_custom_app = %d\r\n", sys_cnt++);

		k_msleep(1000);
	}

	return 0;
}
