cmake_minimum_required(VERSION 3.20.0)

# 尋找 Zephyr 套件，這依賴於環境變數 ZEPHYR_BASE
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(my_custom_app)

target_sources(app PRIVATE src/main.c)