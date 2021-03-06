# F1C100s_with_Keil_RTX4_emWin5
F1C100s with Keil RTX4 + emWin5

# 项目简介
全志F1C100s/F1C200s是极具性价比的ARM9芯片，采用ARM926EJ-S内核，片内自带32MB/64MB SIP-DDR1，DDR频率可稳定运行在156Mhz，CPU主频可达720Mhz无故障运行。这些芯片被广泛用于消费/娱乐电子设备中，系统采用定制化的嵌入式Linux。如今这款芯片市场价大约10元RMB，极具性价比！高性价比吸引了大量单片机开发者，但是单片机开发者通常缺乏Linux开发经验，而且对具备MMU+Cache的ARM9也不够熟悉，因而无法快速上手这几款SoC。于是，这个项目的诞生就是为广大单片机开发者降低门槛，让你打开熟悉的Keil就有可用的RTOS【RTX4】和GUI【emWin】！（Keil环境为ARM公司版权所有）项目直接支持的开发板有：

* widora的tiny200 v1
* widora的tiny200 v2
* sipeed的lichee-pi nano

# 分支
* fb-write-through

该分支的帧缓存安排在SDRAM末尾的“非缓存写通”区域，预留了4MB。这样就不用在帧中断里清理cache了！从而避免在屏幕上出现一些小横条。

* spl-separated

该分支把SPL代码分离出来，编译为f1c100s-spl_uart0.bin和f1c100s-spl_uart1.bin；在生成spl-separated.bin时自动合并SPL。SPL分离出来之后项目代码更灵活，驱动程序更随心所欲，不再受SPL特点的限制。
spl-separated支持“sunxi-fel.exe uboot spl-separated.bin”命令！

# 推荐
推荐使用widora的tiny200开发板，板子为单片机开发者设计了boot和reset按钮，因而进入FEL模式非常方便！板子还自带USB转串口芯片，可作为终端串口，方便调试信息输出。

# 图赏
![三款PCB](doc/img/index.jpg)
![F1C100s](doc/img/1.jpg)
![Keil MDK](doc/img/2.jpg)
![图1](doc/img/a.jpg)
![图2](doc/img/b.jpg)
![图3](doc/img/c.jpg)
![图4](doc/img/d.jpg)
![图5](doc/img/e.jpg)
![图6](doc/img/f.jpg)
![图7](doc/img/g.jpg)
![图8](doc/img/h.jpg)

