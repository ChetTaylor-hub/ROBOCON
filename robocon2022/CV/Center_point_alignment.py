# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
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
#广角120
red_lower = np.array([148, 84, 125])  # HSV阈值低
red_upper = np.array([255, 255, 255])  # HSV阈值高
blue_lower = np.array([46, 64, 150])  # HSV阈值低
blue_upper = np.array([255, 255, 255])  # HSV阈值高
white_lower = np.array([95, 66, 183])  # HSV阈值低
white_upper = np.array([251, 90, 255])  # HSV阈值高
'''相机参数'''
video = 1  # 视频来源，usb摄像头一般为0
width = 320  # 分辨率宽
height = 240 # 分辨率高
'''串口号'''
mpu_com_name = "COM7"  # 单片机模块串口号，字符串格式/dev/ttyAMA0
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
'''检测宽度高度'''
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
    mask = cv.inRange(hsv, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    mask = cv.erode(mask, None, iterations=1)
    mask = cv.dilate(mask, None, iterations=4)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        if 10 < bian < 14:
            area = cv.contourArea(contour)
            (x, y), radius = cv.minEnclosingCircle(approx)
            if width/2 - check_width/2 < x < width/2 + check_width/2 and height/2 - check_height/2 < y < height/2 + check_height/2:
                cv.circle(frame, (int(x), int(y)), 1, (0, 0, 255), 5)
                cv.circle(frame, (int(x), int(y)), int(radius), (0, 255, 0), 5)
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
    mask = cv.inRange(hsv, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    mask = cv.erode(mask, None, iterations=1)
    mask = cv.dilate(mask, None, iterations=4)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        if 3<=bian<=6:
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
    mask = cv.inRange(hsv, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    cv.imshow('src', mask)
    mask = cv.erode(mask, None, iterations=1)
    mask = cv.dilate(mask, None, iterations=3)
    cv.imshow('mask', mask)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        print(bian)
        if 7 < bian < 14:
            (x, y), radius = cv.minEnclosingCircle(approx)
            cv.circle(frame, (int(x), int(y)), 1, (0, 0, 255), 5)
            cv.circle(frame, (int(x), int(y)), int(radius), (0, 255, 0), 5)
            return int(x), int(y)
        else:
            return 0, 0
    except ValueError:
        return 0, 0


def detect_retangle(frame, lower, upper):
    # HSV
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式
    mask = cv.inRange(hsv, lower, upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    mask = cv.erode(mask, None, iterations=1)
    mask = cv.dilate(mask, None, iterations=4)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    try:
        contour = max(contours1, key=cv.contourArea)
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        if 3<=bian<=6:
            x, y, w, h = cv.boundingRect(approx)  # x，y是矩阵左上点的坐标，w，h是矩阵的宽和高
            center_x, center_y = x + w/2, y + h/2
            cv.circle(frame, (int(center_x), int(center_y)), 1, (0, 255, 0), 5)
            cv.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)  # 划矩形线把图像包起来
            return int(center_x), int(center_y)

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
        #frame = cv.GaussianBlur(frame, (5, 5), 0)  # 高斯滤波
        #frame = white_balance(frame, 5)  # 进行自适应白平衡滤波，效果很好，帧率会降低
        '''中心点校准模式'''
        center_circle_blue_x, center_circle_blue_y = detect_circle(frame, blue_lower, blue_upper)
        if center_circle_blue_x and center_circle_blue_y != 0:
            bias_blue_x = center_circle_blue_x - width/2
            bias_blue_y = center_circle_blue_y - (height/2 + 55)
        center_circle_red_x, center_circle_red_y = detect_circle(frame, red_lower, red_upper)
        if center_circle_red_x and center_circle_red_y != 0:
            bias_red_x = center_circle_red_x - width / 2
            bias_red_y = center_circle_red_y - (height/2 + 55)
        message = 'q' + str(bias_red_y).zfill(6) + str(bias_blue_y).zfill(6) + 'e'
        cv.circle(frame, (int(width/2), int(height/2+55)), 5, (0, 0, 255))
        #mpu_ser.com.write(message.encode())  # 向单片机串口发送激光测出距离，帧头@
        end = time.time()
        print(message)
        print('FPS=%d'%(int(1/(end-start))))
        cv.imshow('frame', frame)
        c = cv.waitKey(1)
        if c == 27:
            break

    cv.destroyAllWindows()