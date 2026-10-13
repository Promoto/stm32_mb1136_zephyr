#########################################################
# Environment Setup
#########################################################

$ https://docs.zephyrproject.org/latest/develop/getting_started/index.html

#########################################################
# Project Build
#########################################################

$ west build -p always -b stm32f103_mini samples/hello_world
$ west build -p always -b nucleo_f103rb samples/hello_world
$ west build -p always -b nucleo_f103rb samples/basic/blinky

=> Build Command
$ west build -b nucleo_f103rb
$ west build -p always -b nucleo_f103rb

###########################################################
# System Power Mode
###########################################################

(1). Sleep Mode

SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk; // 清除深度睡眠位元
__WFI();                            // 執行 WFI 組合語言指令

(2). Stop Mode 

SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;  // 開啟深度睡眠
PWR->CR &= ~PWR_CR_PDDS;            // 清除 PDDS，選擇進入 Stop 而非 Standby
PWR->CR |= PWR_CR_LPDS;             // (可選) 讓內部穩壓器進入低功耗
__WFI();

(3). Standby Mode

SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;  // 開啟深度睡眠
PWR->CR |= PWR_CR_PDDS;             // 設定 PDDS，選擇進入 Standby
PWR->CR |= PWR_CR_CWUF;             // 清除喚醒旗標 (Clear Wakeup Flag)
__WFI();

###########################################################
# Project Structure
###########################################################

(0). zephyrproject (zephyr sdk project)

(1). my_custom_app (application project)

* prj.config: marco control boot_seq software	
* Kconfig Search: https://docs.zephyrproject.org/latest/kconfig.html
* Config Result: .config, .config-trace.json
* CMakeLists.txt: project make rule & file include

(2). output file

* output folder : zephyrproject\my_custom_app\build\zephyr
* output bin 	: zephyr.bin
* output dts 	: zephyr.dts
* output map 	: zephyr_final.map

(3). Boot Sequence


(4). Github Push
$ git config --global user.email "you@example.com"
$ git config --global user.name "Your Name"





