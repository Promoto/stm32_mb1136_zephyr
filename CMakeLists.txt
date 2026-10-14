cmake_minimum_required(VERSION 3.20.0)

# 尋找 Zephyr 套件，這依賴於環境變數 ZEPHYR_BASE
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(stm32_mb1136_zephyr)

target_sources(app PRIVATE 
    src/main.c
    test/gpio_test.c
    test/uart_test.c
)

# 加入這行，讓編譯器認得 test 資料夾裡的 .h 檔
target_include_directories(app PRIVATE 
    src
    test
)