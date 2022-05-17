import numpy as np
import cv2  # opencv库
import serial  # python串口库
import serial.tools.list_ports
import wb
import multiprocessing

"""可更改变量"""

red_lower = np.array([0, 112, 103])  # HSV阈值低
red_upper = np.array([78, 248, 255])  # HSV阈值高

#blue_lower = np.array([lh, ls, lv])  # HSV阈值低
#blue_upper = np.array([hh, hs, hv])  # HSV阈值高

video = 0  # 视频来源，usb摄像头一般为0

width = 640  # 分辨率宽
height = 480 # 分辨率高

mpu_com_name = "COM1"  # 单片机模块串口号，字符串格式/dev/ttyTHS0
mode = 'red'

"""不可更改变量"""
cap = cv2.VideoCapture(video)  # 捕捉视频
cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
cap.set(3, width)  # 设置分辨率宽640
cap.set(4, height)  # 设置分辨率高480
light_on_flag = False  # 激光打开标注
light_off_flag = False  # 激光关闭标志
dis = 0  # 距离
accuracy = 0  # 精准度


class Comcontrol(serial.Serial):  # 定义串口控制类,继承串口类
    def __init__(self, port, baudrate, bytesize, stopbits, timeout, parity):  # 实例初始化
        super(Comcontrol, self).__init__()
        self.port = port
        self.baudrate = baudrate
        self.bytesize = bytesize
        self.stopbits = stopbits
        self.timeout = timeout
        self.parity = parity
        """建立串口连接"""
        """设置串口参数：串口号，波特率，数据长度，停止位,延迟时间，校验位"""
        self.com = serial.Serial(port=self.port,
                                 baudrate=self.baudrate,
                                 bytesize=self.bytesize,
                                 stopbits=self.stopbits,
                                 timeout=self.timeout,
                                 parity=self.parity)

    def light_on(self):
        """开启激光"""
        """由产品手册得知，往模块发送大写字母O即可开启激光"""
        self.com.write("O".encode())
        data = self.com.read(10).decode('utf-8')
        print(data)
        if data == "O,OK!\r\n":  # 开启成功返回True
            return True
        else:
            return False

    def light_off(self):
        """关闭激光"""
        """由产品手册得知，往模块发送大写字母C即可开启激光"""
        self.com.write("C".encode())
        data = self.com.read(10).decode('utf-8')
        print(data)
        if data == "C,OK!\r\n":  # 关闭成功返回True
            return True
        else:
            return False

    def measure(self, mode):
        """测量距离，入口参数：串口实例，测量模式"""
        """由产品手册得知，测量模式为：D->标准模式，M->慢速模式，F->快速模式"""
        """由产品手册得知，读取从串口返回数据为：'12.345m,0079'字符串类型"""
        distance = 0  # 距离定义
        ac = 0  # 精准度定义
        if mode == "standard":
            self.com.write("D".encode())
            if self.com.in_waiting:
                data = self.com.read(100).decode('utf-8')  # 得到返回字符串
                distance = int(data[3] + data[5:8])  # 处理得到距离，单位mm
                ac = int(data[10:-1])  # 处理得到准确度，数值越小越准确
        elif mode == "slow":
            self.com.write("M".encode())
            if self.com.in_waiting:
                data = self.com.read(100).decode('utf-8')
                distance = int(data[3] + data[5:8])
                ac = int(data[10:-1])
        elif mode == "fast":
            self.com.write("F".encode())
            if self.com.in_waiting:
                data = self.com.read(100).decode('utf-8')
                distance = int(data[3] + data[5:8])
                ac = int(data[10:-1])
        return distance, ac






def draw_cross(x, y):
    """画图函数"""
    cv2.circle(frame, (x, y), 1, (255, 0, 255), 2)  # 半径足够小的圆就是画点
    cv2.line(frame, (int(x + 5), int(y)), (int(x + 10), int(y)), (255, 0, 255), 2)  # 画出断十字
    cv2.line(frame, (int(x - 5), int(y)), (int(x - 10), int(y)), (255, 0, 255), 2)
    cv2.line(frame, (int(x), int(y + 5)), (int(x), int(y + 10)), (255, 0, 255), 2)
    cv2.line(frame, (int(x), int(y - 5)), (int(x), int(y - 10)), (255, 0, 255), 2)
    # 打印出物体的坐标，坐标可以与单片机通信
    print(x, y)




def mpu_com_connect():
    flag2 = False
    while flag2 is False:
        mpu_ser = Comcontrol(port=mpu_com_name,  # 单片机串口实例化
                             baudrate=115200,
                             bytesize=8,
                             stopbits=1,
                             timeout=0.8,
                             parity='N')
        flag2 = mpu_ser.com.is_open
        if flag2:
            print("mpu connection success\r\n")
            return mpu_ser


"""主进程用以开启子进程"""
if __name__ == '__main__':

    """连接单片机串口"""
    mpu_ser = mpu_com_connect()

    while True:

        _, frame = cap.read()
        frame = cv2.GaussianBlur(frame, (5, 5), 0)  # 高斯滤波
        frame = wb.white_balance(frame, 5)  # 进行自适应白平衡滤波，效果很好，帧率会降低
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式

        k = np.ones((15, 15), np.uint8)  # 开运算卷积核
        #hsv1 = cv2.dilate(cv2.erode(hsv, k), k)  # 开运算，去除亮点


        if mode == 'red':
            mask = cv2.inRange(hsv, red_lower, red_upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
            ker = cv2.getStructuringElement(cv2.MORPH_RECT, (15, 15))
            # mask = cv2.dilate(mask, ker)
            mask = cv2.morphologyEx(mask,cv2.MORPH_CLOSE,ker)
        elif mode == "blue":
            mask = cv2.inRange(hsv, 50, 50)
        # cv2.imshow('f', mask)
        """寻找色块并绘制中心断十字，发送x，y与中心点的误差到单片机"""
        cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
        mask1 = np.ones_like(mask)
        if len(cnts) > 0:  # 如果找到了颜色
            cnt = max(cnts, key=cv2.contourArea)
            (x1, y1) = cv2.minEnclosingCircle(cnt)[0]
            circles = cv2.HoughCircles(mask, cv2.HOUGH_GRADIENT, 2, 50, param1=20, param2=7, minRadius=210,
                                       maxRadius=222)
            cv2.fillConvexPoly(mask1,cnt,255)
            cv2.imshow('f',mask1)
            if circles is not None:
                circles = np.uint16(np.around(circles))
                for i in circles[0, :]:
                    # 绘制外圆
                    cv2.circle(frame, (i[0], i[1]), i[2], (0, 255, 0), 2)
                    # 绘制圆心
                    cv2.circle(frame, (i[0], i[1]), 2, (0, 0, 255), 3)
                    delx = i[0] - 320
                    #mpu_ser.com.write("%".encode('utf-8') + str(delx).encode('utf-8') + "RC".encode('utf-8'))  # 向单片机串口发送x误差

            else:
                print('莫得圆')

        else:
            print("find no color\r\n")
            pass
        cv2.imshow('frame', frame)  # 实时显示

        if cv2.waitKey(1) & 0xFF == 27:  # 等待1ms，检测按下esc退出
            break

cap.release()
cv2.destroyAllWindows()
