# 基座对准代码总结

##导入库说明
![导入库图片](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/pack.png)

1.numpy库作为科学计算库在某些图像处理运算中会使用到

2.cv2库即opencv库，注意版本

3.serial库即串口库，windows上将usb插上usb连接线后会出现COM口（一般情况下必须插上usb连接线才会有串口），同时可以在设备管理器中进行定义

4.threading库即线程库，经过测试与查资料，发现python有全局线程索，导致无法进行真正意义上的多线程操作，本库无用

5.time库，用作系统硬延时，注意使用场合

6.multiprocessing进程库,可以在python环境中实现操作系统操作，类似于时间片轮转，具体方法看下面

## 变量说明
![可更改变量](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/para1.png)

1.追踪颜色阈值调节，上下HSV阈值

2.视频来源，USB摄像头一般为0

3.根据地盘距离基座的距离可以固定在摄像头视野中色块的大小，以此可以作为判断是否正对基座的标志

4.预定义摄像头的分辨率，太大帧率太低，太小看不清楚

5.预定义两个串口所对应的字符串格式，若是使用USB串口则名字为"COMX",X为串口号，若使用jetson nano则为"/dev/ttyTHS1"

6.mode设置激光测距模式，有三种模式，具体见代码

--------------------------------------------------
![不可更改变量](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/para2.png)

1.捕捉视频流，在jetson nano上使用时，不能使用参数中的cv2.CAP_DSHOW,虽然我也不知道为什么

    DSHOW是微软开发的用于视频播放的操作系统I/O接口，就类似于linux系统下的v2l，
    所以linux系统下当然用不了DSHOW。

## 代码说明
![自定义串口类继承](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/class_init.png)

1.python面向对象操作，初始化串口必须的参数，同时调用Serial库的实例化操作即self.com = serial.Serial(...),这样在使用的使用可以直接使用自己定义的类对象直接操作

----------------------------------------------------
![测距代码](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/measure.png)

1.根据PLS-K60编写的激光测距模块代码，使用python的串口收发命令进行操作

2.注意python使用串口收发时必须使用encode()方法和decode()方法，否则无法正常进行数据收发，或者说不符合日常习惯。

3.同时注意到，激光模块的数据返回需要有一定的时间，在这个地方，可以使用self.com.in_waiting来判断串口是否接收到数据，当真正接收到时候再开始处理数据

    in_waiting本身就代表缓冲区现在有的字节数。在pyserial 3.0之后改为了inwaiting（）函数

4.读取数据时，这里使用read(100)即一次读取100个单位，实际测试中，一次读取较少会导致数据读取不完整，100在测试中还没有出错



-------------------------------------------------------
![激光实例化](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/shilijiguang.png)

1.在激光的实例化代码中，会有判断操作，即会判断搜索到的串口是否与预先定义的串口相符合，相符合的话才会进行连接

------------------------------------------------------
![mpu实例化](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/mpu_com.png)

    我觉得这里应该是MCU吧

1.在实际的jetson nano上测试时，发现COM口与jetson nano上引出的串口存在一定的区别，即引出串口在使用serial库进行串口搜索时搜索不到，在这种情况下，直接使用Serial类方法进行实例化

------------------------------------------------------
![opencv](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/final.png)

1.使用了白平衡算法5，即自适应的白平衡滤波，效果很好，但是帧率会略微降低

    我们的USB相机是自带白平衡的。

2.注意在python中出现两个整数相减的情况，减完得到的变量会变成带.0的值，比如：5 - 3 = 2.0 , 这种情况会导致串口发送数据的错误

3.注意在使用opencv视频流方法时，必须在末尾加上cv2.waitKey(1)方法，否则会导致处理卡死

## jetson nano接线说明

![串口接线](https://gitee.com/giannis-wan/robocon2022/raw/wanheng/picture/WPS%E5%9B%BE%E7%89%87-%E4%BF%AE%E6%94%B9%E5%B0%BA%E5%AF%B8.png)

1.如图在jetson nano上存在接线为GND,10,8的三个引脚（从上方看）10为RX，8为TX