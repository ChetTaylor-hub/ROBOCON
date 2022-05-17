# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import threading
import time

import cv2 as cv
from threading import Thread
import numpy as np
import serial

'#################################################################'
"""可更改变量"""
'''色域更改'''
red_lower = np.array([138, 25, 168])  # HSV阈值低
red_upper = np.array([255, 255, 255])  # HSV阈值高
blue_lower = np.array([35, 195, 146])  # HSV阈值低
blue_upper = np.array([114, 255, 255])  # HSV阈值高
white_lower = np.array([101, 35, 201])  # HSV阈值低
white_upper = np.array([187, 255, 255])  # HSV阈值高
'''相机参数'''
video = 1  # 视频来源，usb摄像头一般为0
width = 640  # 分辨率宽
height = 480 # 分辨率高
'''串口号'''
mpu_com_name = "COM5"  # 单片机模块串口号，字符串格式/dev/ttyAMA0
'#################################################################'



'#################################################################'
"""不可更改变量"""
cap = cv.VideoCapture(video)  # 捕捉视频
cap.set(cv.CAP_PROP_FOURCC, cv.VideoWriter_fourcc('M', 'J', 'P', 'G'))
cap.set(3, width)  # 设置分辨率宽640
cap.set(4, height)  # 设置分辨率高480
'#################################################################'



'#################################################################'
'''一些全局变量'''
'''中心点坐标'''
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
circle_blue_area=0
'''偏差坐标'''
bias_red_x=0
bias_red_y=0
bias_blue_x=0
bias_blue_y=0
'''自动翻转'''
overturn=3
overturn_slow=3
count=0
'''检测宽度'''
check_width=100
check_height=100
'#################################################################'

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


def detect_circle_and_area(frame, lower, upper):
    # HSV
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式
    k = np.ones((10, 10), np.uint8)  # 开运算卷积核
    hsv = cv.dilate(cv.erode(hsv, k), k)  # 开运算，去除亮点
    mask1 = cv.inRange(hsv, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    cv.imshow("mask1", mask1)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask1, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        #print("圆{}".format(bian))
        area = cv.contourArea(contour)
        if 7 < bian < 14:
            peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
            area = cv.contourArea(contour)
            (x, y), radius = cv.minEnclosingCircle(approx)
            if width/2 - check_width/2 < x < width/2 + check_width/2 and height/2 - check_height/2 < y < height/2 + check_height/2:
                center_blue = (int(x), int(y))
                radius = int(radius)
                # res = cv.circle(frame,center, radius, (0, 255, 0), 2)
                cv.circle(frame, center_blue, 1, (0, 0, 255), 5)
                cv.circle(frame, center_blue, radius, (0, 255, 0), 5)
                return int(x), int(y), int(area)
            else:
                return 0, 0, 0
        else:
            return 0, 0, 0
    except ValueError:
        return 0, 0, 0



def detect_retangle_and_area(frame, lower, upper):
    # HSV
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式
    k = np.ones((10, 10), np.uint8)  # 开运算卷积核
    hsv1 = cv.dilate(cv.erode(hsv, k), k)  # 开运算，去除亮点
    mask = cv.inRange(hsv1, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    cv.imshow("mask2", mask)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        #print("矩形{}".format(bian))
        area1 = cv.contourArea(contour)
        if 3<=bian<=6 and area1 > 10000:
            peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
            area = cv.contourArea(contour)
            x, y, w, h = cv.boundingRect(approx)  # x，y是矩阵左上点的坐标，w，h是矩阵的宽和高
            center_x, center_y = x + w/2, y + h/2
            if width / 2 - check_width / 2 < center_x < width / 2 + check_width / 2 and height / 2 - check_height / 2 < center_y < height / 2 + check_height / 2:
                cv.circle(frame, (int(center_x), int(center_y)), 1, (0, 255, 0), 5)
                cv.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)  # 划矩形线把图像包起来
                return int(center_x), int(center_y), int(area)
            else:
                return 0, 0, 0
        else:
            return 0, 0, 0
    except ValueError:
        return 0, 0, 0



def detect_circle(frame, lower, upper):
    # HSV
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式
    k = np.ones((10, 10), np.uint8)  # 开运算卷积核
    hsv = cv.dilate(cv.erode(hsv, k), k)  # 开运算，去除亮点
    mask1 = cv.inRange(hsv, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    #mask1 = cv.dilate(mask1, None, iterations=2)
    cv.imshow("mask1", mask1)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask1, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        #print("圆{}".format(bian))
        area = cv.contourArea(contour)
        if 7 < bian < 14:
            peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
            (x, y), radius = cv.minEnclosingCircle(approx)
            if width/2 - check_width/2 < x < width/2 + check_width/2 and height/2 - check_height/2 < y < height/2 + check_height/2:
                center_blue = (int(x), int(y))
                radius = int(radius)
                # res = cv.circle(frame,center, radius, (0, 255, 0), 2)
                cv.circle(frame, center_blue, 1, (0, 0, 255), 5)
                cv.circle(frame, center_blue, radius, (0, 255, 0), 5)
                return int(x), int(y)
            else:
                return 0, 0
        else:
            return 0, 0
    except ValueError:
        return 0, 0


def AutoTurn(frame):
    gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    circles = cv.HoughCircles(gray, cv.HOUGH_GRADIENT, 1, 1000, param1=60, param2=60, minRadius=50, maxRadius=500)
    # 白天数据 1，1000，60，60，50，500
    # 晚上数据 1, 1000, 30, 90, 50, 500

    center_retangle_red_x, center_retangle_red_y = detect_retangle(frame, red_lower, red_upper)
    center_retangle_blue_x, center_retangle_blue_y = detect_retangle(frame, blue_lower, blue_upper)
    cv.rectangle(frame, (220, 170), (340, 290), (255, 0, 0), 1, 1)
    if (abs(center_retangle_red_x - 280) < 60 or abs(center_retangle_blue_x - 280) < 60) and (
            abs(center_retangle_red_y - 230) < 60 or abs(center_retangle_blue_y - 230) < 60):
        print("倾斜")
    # 如果画幅面积变了，可能要校准这两个数值，这是把矩形框在架子范围内的

    elif type(circles) != type(NoneType):
        center_circle_red_x, center_circle_red_y, area_circle_red = detect_circle(frame, red_lower, red_upper)
        center_circle_blue_x, center_circle_blue_y, area_circle_blue = detect_circle(frame, blue_lower, blue_upper)
        for i in circles[0, :]:
            if abs(int(center_circle_blue_x) - int(i[0])) < 10 or abs(int(center_circle_red_x) - int(i[0])) < 10:
                if 70000 < area_circle_blue < 72000:
                    print('反')
                # print(1,int (center_circle_red_x) - int(i[0]))
                # print(1,int(center_circle_blue_x) - int(i[0]))
                print('正')
            else:
                # if 12000<area_circle_blue<14000:
                #    print('正')
                # print(2, int(center_circle_red_x) - int(i[0]))
                # print(2, int(center_circle_blue_x) - int(i[0]))
                print('反')
            cv.circle(frame, (int(i[0]), int(i[1])), int(i[2]), (0, 255, 0), 5)
            cv.circle(frame, (int(i[0]), int(i[1])), 2, (0, 0, 255), 3)
    # 前提是操作手把目标放在摄像头可以检测到的区域里了
    # 期望：做到全自动 在按下升起按钮后停在指定停止高度，自动反转后，继续上升到指定高度  暂时不能做到行进中翻转和边升高边反转
    else:
        print('倾斜')

def detect_retangle(frame, lower, upper):
    # HSV
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式
    k = np.ones((10, 10), np.uint8)  # 开运算卷积核
    hsv1 = cv.dilate(cv.erode(hsv, k), k)  # 开运算，去除亮点
    mask = cv.inRange(hsv1, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    cv.imshow("mask2", mask)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        #print("矩形{}".format(bian))
        area1 = cv.contourArea(contour)
        if 3<=bian<=6 and area1 > 1000:
            peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
            x, y, w, h = cv.boundingRect(approx)  # x，y是矩阵左上点的坐标，w，h是矩阵的宽和高
            center_x, center_y = x + w/2, y + h/2
            if width / 2 - check_width / 2 < center_x < width / 2 + check_width / 2 and height / 2 - check_height / 2 < center_y < height / 2 + check_height / 2:
                cv.circle(frame, (int(center_x), int(center_y)), 1, (0, 255, 0), 5)
                cv.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)  # 划矩形线把图像包起来
                return int(center_x), int(center_y)
            else:
                return 0, 0
        else:
            return 0, 0
    except ValueError:
        return 0, 0


def white_balance(img, mode=5):
    """白平衡处理（默认为5, 1均值、2完美反射、3灰度世界、4基于图像分析的偏色检测及颜色校正、5动态阈值）"""
    # 读取图像
    b, g, r = cv.split(img)
    # 均值变为三通道
    h, w, c = img.shape
    if mode == 1:
        # 默认均值  ---- 简单的求均值白平衡法
        b_avg, g_avg, r_avg = cv.mean(b)[0], cv.mean(g)[0], cv.mean(r)[0]
        # 求各个通道所占增益
        k = (b_avg + g_avg + r_avg) / 3
        kb, kg, kr = k / b_avg, k / g_avg, k / r_avg
        b = cv.addWeighted(src1=b, alpha=kb, src2=0, beta=0, gamma=0)
        g = cv.addWeighted(src1=g, alpha=kg, src2=0, beta=0, gamma=0)
        r = cv.addWeighted(src1=r, alpha=kr, src2=0, beta=0, gamma=0)
        output_img = cv.merge([b, g, r])
    elif mode == 2:
        # 完美反射白平衡 ---- 依赖ratio值选取而且对亮度最大区域不是白色的图像效果不佳。
        output_img = img.copy()
        sum_ = np.double() + b + g + r
        hists, bins = np.histogram(sum_.flatten(), 766, [0, 766])
        Y = 765
        num, key = 0, 0
        ratio = 0.01
        while Y >= 0:
            num += hists[Y]
            if num > h * w * ratio / 100:
                key = Y
                break
            Y = Y - 1

        sumkey = np.where(sum_ >= key)
        sum_b, sum_g, sum_r = np.sum(b[sumkey]), np.sum(g[sumkey]), np.sum(r[sumkey])
        times = len(sumkey[0])
        avg_b, avg_g, avg_r = sum_b / times, sum_g / times, sum_r / times

        maxvalue = float(np.max(output_img))
        output_img[:, :, 0] = output_img[:, :, 0] * maxvalue / int(avg_b)
        output_img[:, :, 1] = output_img[:, :, 1] * maxvalue / int(avg_g)
        output_img[:, :, 2] = output_img[:, :, 2] * maxvalue / int(avg_r)
    elif mode == 3:
        # 灰度世界假设
        b_avg, g_avg, r_avg = cv.mean(b)[0], cv.mean(g)[0], cv.mean(r)[0]
        # 需要调整的RGB分量的增益
        k = (b_avg + g_avg + r_avg) / 3
        kb, kg, kr = k / b_avg, k / g_avg, k / r_avg
        ba, ga, ra = b * kb, g * kg, r * kr

        output_img = cv.merge([ba, ga, ra])
    elif mode == 4:
        # 基于图像分析的偏色检测及颜色校正
        I_b_2, I_r_2 = np.double(b) ** 2, np.double(r) ** 2
        sum_I_b_2, sum_I_r_2 = np.sum(I_b_2), np.sum(I_r_2)
        sum_I_b, sum_I_g, sum_I_r = np.sum(b), np.sum(g), np.sum(r)
        max_I_b, max_I_g, max_I_r = np.max(b), np.max(g), np.max(r)
        max_I_b_2, max_I_r_2 = np.max(I_b_2), np.max(I_r_2)
        [u_b, v_b] = np.matmul(np.linalg.inv([[sum_I_b_2, sum_I_b], [max_I_b_2, max_I_b]]), [sum_I_g, max_I_g])
        [u_r, v_r] = np.matmul(np.linalg.inv([[sum_I_r_2, sum_I_r], [max_I_r_2, max_I_r]]), [sum_I_g, max_I_g])
        b0 = np.uint8(u_b * (np.double(b) ** 2) + v_b * b)
        r0 = np.uint8(u_r * (np.double(r) ** 2) + v_r * r)
        output_img = cv.merge([b0, g, r0])
    elif mode == 5:
        # 动态阈值算法 ---- 白点检测和白点调整
        # 只是白点检测不是与完美反射算法相同的认为最亮的点为白点，而是通过另外的规则确定
        def con_num(x):
            if x > 0:
                return 1
            if x < 0:
                return -1
            if x == 0:
                return 0

        yuv_img = cv.cvtColor(img, cv.COLOR_BGR2YCrCb)
        # YUV空间
        (y, u, v) = cv.split(yuv_img)
        max_y = np.max(y.flatten())
        sum_u, sum_v = np.sum(u), np.sum(v)
        avl_u, avl_v = sum_u / (h * w), sum_v / (h * w)
        du, dv = np.sum(np.abs(u - avl_u)), np.sum(np.abs(v - avl_v))
        avl_du, avl_dv = du / (h * w), dv / (h * w)
        radio = 0.5  # 如果该值过大过小，色温向两极端发展

        valuekey = np.where((np.abs(u - (avl_u + avl_du * con_num(avl_u))) < radio * avl_du)
                            | (np.abs(v - (avl_v + avl_dv * con_num(avl_v))) < radio * avl_dv))
        num_y, yhistogram = np.zeros((h, w)), np.zeros(256)
        num_y[valuekey] = np.uint8(y[valuekey])
        yhistogram = np.bincount(np.uint8(num_y[valuekey].flatten()), minlength=256)
        ysum = len(valuekey[0])
        Y = 255
        num, key = 0, 0
        while Y >= 0:
            num += yhistogram[Y]
            if num > 0.1 * ysum:  # 取前10%的亮点为计算值，如果该值过大易过曝光，该值过小调整幅度小
                key = Y
                break
            Y = Y - 1

        sumkey = np.where(num_y > key)
        sum_b, sum_g, sum_r = np.sum(b[sumkey]), np.sum(g[sumkey]), np.sum(r[sumkey])
        num_rgb = len(sumkey[0])

        b0 = np.double(b) * int(max_y) / (sum_b / num_rgb)
        g0 = np.double(g) * int(max_y) / (sum_g / num_rgb)
        r0 = np.double(r) * int(max_y) / (sum_r / num_rgb)

        output_img = cv.merge([b0, g0, r0])
    else:
        raise TypeError('mode should be in [1,2,3,4,5]. Got {}'.format(mode))
    output_img = np.uint8(np.clip(output_img, 0, 255))
    return output_img

def send_UART():
    while True:
        global bias_x, bias_y
        print(bias_x, bias_y)  # 打印出距离和准确度
        mpu_ser.com.write("q".encode() + str(bias_x).encode() + "e".encode())  # 向单片机串口发送激光测出距离，帧头@

if __name__ == '__main__':

    """连接单片机串口"""
    #mpu_ser = mpu_com_connect()

    while cap.isOpened():
        start = time.time()
        ret, frame = cap.read()
        frame = cv.GaussianBlur(frame, (5, 5), 0)  # 高斯滤波
        #frame = white_balance(frame, 5)  # 进行自适应白平衡滤波，效果很好，帧率会降低

        src = frame
        '''电机翻转模式'''
        #center_retangle_white_x, center_retangle_white_y = detect_retangle(frame, white_lower, white_upper)
        center_retangle_blue_x, center_retangle_blue_y = detect_retangle(src, blue_lower, blue_upper)
        center_retangle_red_x, center_retangle_red_y = detect_retangle(src, red_lower, red_upper)
        center_circle_white_x, center_circle_white_y = detect_circle(src, white_lower, white_upper)
        center_circle_red_x, center_circle_red_y = detect_circle(src, red_lower, red_upper)
        center_circle_blue_x, center_circle_blue_y, circle_blue_area = detect_circle_and_area(src, blue_lower, blue_upper)
        if center_retangle_red_x != 0 and center_retangle_red_y != 0:
            overturn = 1
        if center_retangle_blue_x !=0 and center_retangle_blue_y != 0:
            overturn = 1
        if center_circle_red_x and center_circle_red_y and center_circle_white_x and center_circle_white_y != 0:
            overturn = 0
        if center_circle_white_x and center_circle_white_y and center_circle_blue_x and center_circle_blue_y != 0:
            overturn = 0
        if circle_blue_area < 6000 and center_circle_blue_x != 0 and center_circle_blue_y != 0 and center_circle_white_x == 0 and center_circle_white_y == 0:
            overturn = 0
        if circle_blue_area > 12000 and center_circle_blue_x != 0 and center_circle_blue_y != 0 and center_circle_white_x == 0 and center_circle_white_y == 0:
            overturn = 2
        if center_circle_white_x and center_circle_white_y != 0 and center_circle_red_x == 0 and center_circle_red_y == 0 and center_circle_blue_x == 0 and center_circle_blue_y == 0:
            overturn = 2
        #print(center_retangle_red_x, center_retangle_red_y, center_retangle_blue_x, center_retangle_blue_y)
        #print(circle_blue_area)
        #center_circle_blue_x, center_circle_blue_y = detect_circle(frame, blue_lower, blue_upper)
        #if center_circle_blue_x and center_circle_blue_y != 0:
            #overturn = '1'
        '''
        中心点校准模式
        '''
        '''
        center_circle_blue_x, center_circle_blue_y = detect_circle(frame, blue_lower, blue_upper)
        if center_circle_blue_x and center_circle_blue_y != 0:
            bias_blue_x = center_circle_blue_x - width/2
            bias_blue_y = center_circle_blue_y - (height/2-80)
        center_circle_red_x, center_circle_red_y = detect_circle(frame, red_lower, red_upper)
        if center_circle_red_x and center_circle_red_y != 0:
            bias_red_x = center_circle_red_x - width / 2
            bias_red_y = center_circle_red_y - (height / 2 - 80)
        message = 'q' + str(bias_red_x).zfill(6) + str(bias_blue_x).zfill(6) + str(overturn) + 'e'
        #mpu_ser.com.write(message.encode())  # 向单片机串口发送激光测出距离，帧头@
        '''
        end = time.time()
        print('FPS=%d'%(int(1/(end-start))))
        #print(message)

        cv.rectangle(frame, (int(width/2 - check_width/2), int(height/2 - check_height/2)), (int(width/2 + check_width/2), int(height/2 + check_height/2)), (0, 255, 0), 2)  # 划矩形线把图像包起来
        cv.imshow("frame", frame)
        cv.imshow('src', src)
        c = cv.waitKey(1)
        if c == 27:
            break

    cv.destroyAllWindows()