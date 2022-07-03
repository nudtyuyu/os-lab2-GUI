# os-lab2-GUI
Add GUI features to Linux-0.11
在os-lab1-syscall的基础上，给Linux0.11添加了鼠标驱动和图形界面功能，并设计了简单的马里奥小游戏用于测试功能实现效果\
Linux-0.11在lab1中添加syscall之后的源码项目链接：<https://github.com/nudtyuyu/os-lab1-syscall>

### 本项目实现的功能
####  (1) 添加鼠标设备驱动
`kernel/chr_dev/mouse.2.s`
`kernel/chr_dev/readmouse.c`
####  (2) 显示模式的设置
`kernel/graphics.c`
####  (3) 消息驱动
`kernel/message.c`
`include/message.h`
####  (4) 应用程序-mygame:Easy Mario
`mygame/`

