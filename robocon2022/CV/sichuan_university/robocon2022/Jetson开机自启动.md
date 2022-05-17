# jetson nano开机自启动python程序（NX应该是一样的）

---
## 使用shell脚本进行开机自启动
### shell脚本使用简述

1.解析器

shell脚本开头必须要指定解析器，即下列语句，该语句放在shell脚本的第一行。

	#！/bin/bash
	
除了指定解析器语句的行开头的#有意义，其余行的开头的#号表示注释

2.输出操作 echo

	echo '11111111'

echo后面是输出的内容

3.管道操作 |

	echo '11111111' | sudo chmod 777 /dev/ttyTHS1

在使用sudo命令时会提示输入密码，但是开机自启动时不能手动输入，因此在shell脚本中采用管道命令。上图命令表示在使用sudo chmod 777/dev/ttyTHS1弹出要求输入sudo密码时输入11111111.

4.其余基本操作（和linux普通操作基本一致）

5.举例

如：想要执行/home/rc/robocon2022 文件下下的find1.py文件，整体shell文件如下：

	#!/bin/bash
	echo '11111111' | sudo chmod 777/tty/THS1
	cd /home/rc/robocon2022/
	python3 find1.py

---
### jetson nano使用

1.设置开机自动登录

打开设置

![打开设置](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/ZQ1.png)

点击用户账户

![点击用户账户](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/ZQ2.png)

右上角先解锁，然后打开自动登录


2.创建shell文件

创建.sh文件

	cd /home/rc/robocon2022
	sudo gedit start.sh

添加下列内容到start.sh

	#!/bin/bash
	echo '11111111' | sudo chmod 777/tty/THS1
	cd /home/rc/robocon2022/
	python3 find1.py

保存退出

3.添加shell文件到开机自启动目录

终端输入命令 ,进入自启动管理界面

	gnome-session-properties

![自启动管理](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/ZQ4.png)

点击添加，输入如下图:

![自启动输入](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/ZQ5.png)

命令栏输入如下:(-x后面内容为刚刚创建的.sh文件绝对路径）名字随意，注释随意

	gnome-terminal -x /home/rc/robocon2022/start.sh

保存退出

4.给文件赋予权限（一定要！！不然开机会显示权限不够)

	cd /home/rc/robocon2022/
	sudo chmod a+x start.sh

5.重启应该可以使用了

	sudo reboot




