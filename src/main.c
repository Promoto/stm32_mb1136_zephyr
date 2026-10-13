/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include <zephyr/kernel.h> // 確保有引入 kernel.h 來使用 k_msleep
#include <zephyr/drivers/gpio.h>

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

/* 1. 從 Device Tree 取得 usart3 的硬體實例 */
#define USART3_NODE DT_NODELABEL(usart3)
static const struct device *uart3_dev = DEVICE_DT_GET(USART3_NODE);

/* 建立一個自訂的字串輸出函數 (Polling 模式阻塞輸出) */
void custom_uart_log(const struct device *dev, const char *str)
{
    if (!device_is_ready(dev)) {
        return;
    }
    
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        /* uart_poll_out 會將單一字元推入硬體 TX 暫存器 */
        uart_poll_out(dev, str[i]);
    }
}

uint8_t custom_uart_init(void)
{
    /* 2. 檢查 SYS_INIT 是否已經成功喚醒 USART3 */
    if (!device_is_ready(uart3_dev)) {
        printk("系統錯誤：USART3 硬體未就緒！\n");
        return 0;
    }

    /* 3. 開始使用 USART3 輸出你的額外 Log */
    custom_uart_log(uart3_dev, "[CUSTOM LOG] USART3 initialized successfully.\r\n");

    /* 4. (進階) 手動在執行期重新設定 UART 參數 (例如改變 Baudrate) */
    struct uart_config cfg;
    
    /* 先取得當前設定 */
    if (uart_config_get(uart3_dev, &cfg) == 0) {
        cfg.baudrate = 115200;
        
        /* 應用新設定 */
        if (uart_configure(uart3_dev, &cfg) == 0) {
            custom_uart_log(uart3_dev, "[CUSTOM LOG] Baudrate changed to 115200.\r\n");
        }
    }

    return 1;
}

/* 1. 從 Device Tree 中取得 alias 名稱為 led0 的節點 */
/* 大多數 Zephyr 支援的 STM32 開發板 (如 Nucleo/Discovery) 預設都有定義 led0 */
#define LED0_NODE DT_ALIAS(led0)

/* 確保 Device Tree 中有定義這個節點 */
#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

/* 2. 獲取 GPIO 腳位的規格與屬性 (Port, Pin, Active High/Low 等) */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

uint8_t custom_gpio_init(void)
{
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

    return 1;
}

uint8_t custom_gpio_toggle_handle(void)
{   
    int ret;

    /* 5. 翻轉 (Toggle) GPIO 引腳電位 */
    ret = gpio_pin_toggle_dt(&led);
    if (ret < 0) {
        return 0;
    }

    return 1;
}

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

        custom_uart_log(uart3_dev, "System is running...\r\n");

		printf("JACK my_custom_app = %d\r\n", sys_cnt++);

		k_msleep(1000);
	}

	return 0;
}
