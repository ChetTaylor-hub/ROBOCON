# Ethan Lee
# 2022/4/29 21:58

import cv2 as cv
import numpy as np
import serial

width = 640  # 分辨率宽
height = 480 # 分辨率高

'''一些全局变量'''
center_retangle_red_x=0
center_retangle_red_y=0
center_retangle_blue_x=0
center_retangle_blue_y=0
center_retangle_white_x=0
center_retangle_white_y=0
center_circle_red_x=0
center_circle_red_y=0
center_circle_blue_x=0
center_circle_blue_y=0
center_circle_white_x=0
center_circle_white_y=0
bias_red_x=0
bias_red_y=0
bias_blue_x=0
bias_blue_y=0
overturn='3'
area_circle_red=0
area_circle_blue=0

"""可更改变量"""
red_lower = np.array([131, 64, 116])  # HSV阈值低
red_upper = np.array([255, 255, 255])  # HSV阈值高
blue_lower = np.array([64, 168, 147])  # HSV阈值低
blue_upper = np.array([255, 255, 255])  # HSV阈值高
white_lower = np.array([37, 30, 101])  # HSV阈值低
white_upper = np.array([118, 60, 255])  # HSV阈值高

mpu_com_name = "COM5"  # 单片机模块串口号，字符串格式/dev/ttyAMA0
mode = 'red'

NoneType = None

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
            # self.com.write("iFACM".encode())
            if self.com.in_waiting:
                data = self.com.read(10).decode('utf-8')
                # distance.append(data)
                distance = int(data[2]) * 1 + int(data[4]) * 0.1 + int(data[5]) * 0.01 + int(data[6]) * 0.001
                ac = int(data[6]) * 0.001
        return distance, ac

    def receive(self):
        self.com.write("waiting".encode())
        if self.com.in_waiting:
            data = self.com.read(1).decode('utf-8')
            return data





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



def detect_retangle(frame, lower, upper):
    # HSV
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式
    k = np.ones((10, 10), np.uint8)  # 开运算卷积核
    hsv1 = cv.dilate(cv.erode(hsv, k), k)  # 开运算，去除亮点
    mask = cv.inRange(hsv1, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）

    color_mask = cv.medianBlur(mask, 7)  # 中值滤波
    kernel = cv.getStructuringElement(cv.MORPH_CROSS, (5, 5))  # 十字形结构
    color_mask = cv.dilate(color_mask, kernel)  # 膨胀
    kernel = np.ones((10, 10), np.uint8)
    color_mask = cv.erode(color_mask, kernel)  # 腐蚀
    #cv.imshow("mask2", color_mask)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        area1 = cv.contourArea(contour)
        if 3<=bian<=6 and area1 > 15000:
            peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
            x, y, w, h = cv.boundingRect(approx)  # x，y是矩阵左上点的坐标，w，h是矩阵的宽和高
            center_x, center_y = x + w/2, y + h/2
            cv.circle(frame, (int(center_x), int(center_y)), 1, (0, 255, 0), 5)
            cv.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)  # 划矩形线把图像包起来
            return int(center_x), int(center_y)
        else:
            return 0, 0
    except ValueError:
        return 0, 0

def detect_circle(frame, lower, upper):
    # HSV
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式
    k = np.ones((10, 10), np.uint8)  # 开运算卷积核
    hsv = cv.dilate(cv.erode(hsv, k), k)  # 开运算，去除亮点
    mask1 = cv.inRange(hsv, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    mask1 = cv.erode(mask1, None, iterations=1)
    mask1 = cv.medianBlur(mask1, 7)  # 中值滤波
    mask1 = cv.dilate(mask1, None, iterations=4)
    #cv.imshow("mask1", mask1)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask1, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        #print("圆{}".format(bian))
        area = cv.contourArea(contour)
        if 7 < bian < 14 and area > 12000:
            peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
            (x, y), radius = cv.minEnclosingCircle(approx)
            center_blue = (int(x), int(y))
            radius = int(radius)
            # res = cv.circle(frame,center, radius, (0, 255, 0), 2)
            #cv.circle(frame, center_blue, 1, (0, 0, 255), 5)
            #cv.circle(frame, center_blue, radius, (0, 255, 0), 5)
            return int(x), int(y), area
        else:
            return 0, 0, 0
    except ValueError:
        return 0, 0, 0

'''def ROI_Circle(frame):
    mask3=np.zeros(frame)
    mask3=cv.circle(mask3,(280,230),250,(255,255,255),-1)
    cv.imshow("maske3",mask3)
   # frame_circle=cv.add(frame, mask3, mask=mask3)
   # cv.imshow("frame_circle",frame_circle)
   # return frame_circle'''

def main():
    mpu_ser = mpu_com_connect()
    capture = cv.VideoCapture(1, cv.CAP_DSHOW)  # 打开电脑自带摄像头，如果参数是1会打开外接摄像头
    capture.set(3, width)  # 设置分辨率宽640
    capture.set(4, height)  # 设置分辨率高480
    while True:
        ret, frame = capture.read()

        frame = frame[0:500, 25:525]  # frame [h,w]
        #后面的图像要全部基于圆形图像操作
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)  # convert frame to gray
        gray = cv.GaussianBlur(gray, (5, 5), 0)  # gaussianblur denoise
        #cv.imshow("gray", gray)
        #ROI_Circle(gray)

        circles = cv.HoughCircles(gray, cv.HOUGH_GRADIENT, 1, 1000, param1=60, param2=60, minRadius=50, maxRadius=500)
        #白天数据 1，1000，60，60，50，500
        #晚上数据 1, 1000, 30, 90, 50, 500

        center_retangle_red_x, center_retangle_red_y = detect_retangle(frame, red_lower, red_upper)
        center_retangle_blue_x, center_retangle_blue_y = detect_retangle(frame, blue_lower, blue_upper)
        cv.rectangle(frame,(220,170),(340,290),(255,0,0),1,1)
        if (abs(center_retangle_red_x-280) < 60 or abs(center_retangle_blue_x-280) < 60) and (abs(center_retangle_red_y - 230) < 60 or abs(center_retangle_blue_y-230) < 60):
            print("倾斜")
        #如果画幅面积变了，可能要校准这两个数值，这是把矩形框在架子范围内的

        elif type(circles) != type(NoneType):
            center_circle_red_x, center_circle_red_y, area_circle_red = detect_circle(frame, red_lower, red_upper)
            center_circle_blue_x, center_circle_blue_y, area_circle_blue = detect_circle(frame, blue_lower, blue_upper)
            for i in circles[0,:]:
                if abs(int(center_circle_blue_x) - int(i[0])) < 10 or abs(int(center_circle_red_x) - int(i[0])) < 10 :
                    if 70000 < area_circle_blue < 72000:
                        print('反')
                   # print(1,int (center_circle_red_x) - int(i[0]))
                   # print(1,int(center_circle_blue_x) - int(i[0]))
                    print('正')
                else:
                    #if 12000<area_circle_blue<14000:
                    #    print('正')
                    #print(2, int(center_circle_red_x) - int(i[0]))
                    #print(2, int(center_circle_blue_x) - int(i[0]))
                    print('反')
                cv.circle(frame, (int(i[0]), int(i[1])), int(i[2]), (0, 255, 0), 5)
                cv.circle(frame, (int(i[0]), int(i[1])), 2, (0, 0, 255), 3)
        #前提是操作手把目标放在摄像头可以检测到的区域里了
        #期望：做到全自动 在按下升起按钮后停在指定停止高度，自动反转后，继续上升到指定高度  暂时不能做到行进中翻转和边升高边反转
        else:
            print('倾斜')

        message = 'q' + str(bias_red_x).zfill(6) + str(bias_blue_x).zfill(6) + str(overturn) + 'e'

        cv.imshow("frame", frame)
        c = cv.waitKey(50)
        if c == 27:
            break


main()
cv.destroyAllWindows()
