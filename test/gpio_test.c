#include <zephyr/kernel.h> // 確保有引入 kernel.h 來使用 k_msleep
#include <zephyr/drivers/gpio.h>

#include "gpio_test.h"

/* 1. 從 Device Tree 取得按鍵設定 (依賴 app.overlay 中的 sw0 別名) */
#define SW0_NODE DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});

/* 定義中斷 Callback 所需的資料結構 */
static struct gpio_callback button_cb_data;

/* 1. 從 Device Tree 中取得 alias 名稱為 led0 的節點 */
/* 大多數 Zephyr 支援的 STM32 開發板 (如 Nucleo/Discovery) 預設都有定義 led0 */
#define LED0_NODE DT_ALIAS(led0)

/* 確保 Device Tree 中有定義這個節點 */
#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Unsupported board: led0 devicetree alias is not defined"
#endif

/* 2. 獲取 GPIO 腳位的規格與屬性 (Port, Pin, Active High/Low 等) */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/* 2. 中斷處理函式 (ISR Callback) */
void button_pressed_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    /* 注意：ISR 內必須保持簡短，絕不可呼叫 k_sleep() 等會阻塞的函式 */
    printk("Button Triggered! (Pin mask: 0x%x)\n", pins);
    
    /* 若要控制 LED，可在此處呼叫 led_test_toggle() */
}

/* 3. 初始化按鍵硬體與註冊中斷 */
void button_interrupt_init(void)
{
    int ret;

    /* 檢查硬體裝置是否就緒 */
    if (!gpio_is_ready_dt(&button)) {
        printk("Error: button device %s is not ready\n", button.port->name);
        return;
    }

    /* 設定為輸入模式 (如果硬體沒有外部電阻，可改為 GPIO_INPUT | GPIO_PULL_UP) */
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n", ret, button.port->name, button.pin);
        return;
    }

    /* 設定中斷觸發條件：當電平變化至 Active 狀態時觸發 (Edge to Active) */
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error %d: failed to configure interrupt on %s pin %d\n", ret, button.port->name, button.pin);
        return;
    }

    /* 將 Callback 函式與指定的 GPIO Pin 綁定並加入系統 */
    gpio_init_callback(&button_cb_data, button_pressed_isr, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    printk("Button interrupt initialized at %s pin %d\n", button.port->name, button.pin);
}

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

    button_interrupt_init(); // 初始化按鍵中斷

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