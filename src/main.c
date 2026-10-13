/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>


#include <zephyr/kernel.h> // 確保有引入 kernel.h 來使用 k_msleep
#include <zephyr/drivers/gpio.h>


/* 1. 從 Device Tree 中取得 alias 名稱為 led0 的節點 */
/* 大多數 Zephyr 支援的 STM32 開發板 (如 Nucleo/Discovery) 預設都有定義 led0 */
#define LED0_NODE DT_ALIAS(led0)

/* 確保 Device Tree 中有定義這個節點 */
#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

/* 2. 獲取 GPIO 腳位的規格與屬性 (Port, Pin, Active High/Low 等) */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);


int main(void)
{
	// 強制開啟 STM32 在 Sleep, Stop, Standby 模式下的除錯時脈
    *(volatile uint32_t *)0xE0042004 |= 0x07;

	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);

	int ret;

    /* 3. 檢查對應的 GPIO 控制器是否已經初始化並就緒 */
    if (!gpio_is_ready_dt(&led)) {
        return 0;
    }

    /* 4. 將該引腳配置為輸出，並套用 Device Tree 定義的初始狀態 */
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return 0;
    }

	while(1)
	{
		/* 5. 翻轉 (Toggle) GPIO 引腳電位 */
        ret = gpio_pin_toggle_dt(&led);
        if (ret < 0) {
            return 0;
        }

		printf("JACK my_custom_app\r\n");
		k_msleep(1000);
	}

	return 0;
}
