#include <zephyr/kernel.h> // 確保有引入 kernel.h 來使用 k_msleep
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <string.h>

#include "uart_test.h"

/* 1. 從 Device Tree 取得 usart3 的硬體實例 */
#define USART3_NODE DT_NODELABEL(usart3)
static const struct device *uart3_dev = DEVICE_DT_GET(USART3_NODE);

/* 建立一個自訂的字串輸出函數 (Polling 模式阻塞輸出) */
uint8_t custom_uart_log(const struct device *dev, const char *str)
{
    if (!device_is_ready(dev)) {
        return 0;
    }
    
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        /* uart_poll_out 會將單一字元推入硬體 TX 暫存器 */
        uart_poll_out(dev, str[i]);
    }

    return 1;
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

uint8_t custom_uart_test(void)
{
    custom_uart_log(uart3_dev, "System is running...\r\n");
    
    return 1;
}