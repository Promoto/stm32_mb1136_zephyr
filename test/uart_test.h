#ifndef _UART_TEST_H_
#define _UART_TEST_H_

uint8_t custom_uart_log(const struct device *dev, const char *str);
uint8_t custom_uart_init(void);
uint8_t custom_uart_test(void);

#endif
