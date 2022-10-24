# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import threading

import cv2 as cv

import numpy as np
from matplotlib import pyplot as plt

x=0
y=0
center_x=0
center_y=0
center_shu_x=[]
center_shu_y=[]
bias_x=0
bias_y=0
num=0
symbol=1


lower_hsv1 = np.array([54, 143, 181])
upper_hsv1 = np.array([206, 255, 255])

lower_hsv2 = np.array([143, 5, 80])
upper_hsv2 = np.array([206, 255, 255])

def Canny_detect(frame):
    gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    mask = cv.Canny(gray, 64, 128)
    return mask


def detect_area(frame, lower_hsv, upper_hsv):
    mask1 = Canny_detect(frame)

    mask1 = cv.erode(mask1, None, iterations=1)  # 腐蚀去毛刺（完善图片边角）
    mask1 = cv.dilate(mask1, None, iterations=4)  # 膨胀恢复图像（适当调整大小可以实现将图片中间的一些无用特征去掉)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask1, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    for i, contour in enumerate(contours1):
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        # if(bian > 4 and bian < 8):
        area1 = cv.contourArea(contour)
        return area1


def detect(frame):
    #mask1 = Canny_detect(frame)
    #cv.imshow("mask", mask1)
    gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    mask1 = cv.Canny(gray, 64, 128)

    #        img1,
    contours1, heriachy1 = cv.findContours(mask1, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    for i, contour in enumerate(contours1):
        peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        # if(bian > 4 and bian < 8):
        area1 = cv.contourArea(contour)
        print(area1)
        if area1 > 3 and bian >= 8 and bian <= 10:
            print(bian)
            peri = cv.arcLength(contours1[i], True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contours1[i], 0.015 * peri, True)  # 传入轮廓的点集
            (x, y), radius = cv.minEnclosingCircle(approx)
            center_blue = (int(x), int(y))
            radius = int(radius)
            # res = cv.circle(frame,center, radius, (0, 255, 0), 2)
            cv.circle(frame, center_blue, radius, (0, 255, 0), 5)
            return int(x), int(y)
        else:
            return 0, 0
    return 0, 0



def main():
    global num
    global center_x
    global center_y
    global bias_x
    global bias_y
    global x
    global y
    global symbol
    capture = cv.VideoCapture(1)  # 打开电脑自带摄像头，如果参数是1会打开外接摄像头

    while True:

        def on_EVENT_LBUTTONDOWN(event, x, y, flags, param):
            if event == cv.EVENT_LBUTTONDOWN:
                xy = "%d,%d" % (x, y)
                cv.circle(frame, (x, y), 1, (255, 0, 0), thickness=-1)
                cv.putText(frame, xy, (x, y), cv.FONT_HERSHEY_PLAIN,
                            1.0, (0, 0, 0), thickness=1)
                cv.imshow("image", frame)
        #ret, frame = capture.read()
        frame = cv.imread("pipei/B3AB862852FA3A305F4890D09F317517.jpg")
        frame = cv.resize(frame, [1024, 1024])
        #frame = cv.GaussianBlur(frame, (5, 5), 10)
        #frame = frame[350:700, 240:800]
        h, w = frame.shape[:2]
        gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
        mask1 = cv.Canny(gray, 256, 256)

        '''
        # 200 代表应该检测到的行的最小长度
        lines = cv.HoughLines(mask1, 1, np.pi / 180, 200)

        print(lines)
        for i in range(len(lines)):
            for rho, theta in lines[i]:
                a = np.cos(theta)
                b = np.sin(theta)
                x0 = a * rho
                y0 = b * rho
                x1 = int(x0 + w * (-b))
                y1 = int(y0 + w * (a))
                x2 = int(x0 - w * (-b))
                y2 = int(y0 - w * (a))

                cv.line(frame, (x1, y1), (x2, y2), (0, 0, 255), 2)
                plt.imshow(frame), plt.title('messi_plt')
                plt.xticks([]), plt.yticks([])  # 隐藏坐标轴
                plt.show()
        '''
        #        img1,
        #mask1 = cv.erode(mask1, None, iterations=1)  # 腐蚀去毛刺（完善图片边角）
        mask1 = cv.dilate(mask1, None, iterations=4)  # 膨胀恢复图像（适当调整大小可以实现将图片中间的一些无用特征去掉)
        cv.imshow("mask1", mask1)

        contours1, heriachy1 = cv.findContours(mask1, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

        for i, contour in enumerate(contours1):
            peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
            bian = len(approx)
            # if(bian > 4 and bian < 8):
            area1 = cv.contourArea(contour)
            print(area1)
            if area1 > 18000 :
                peri = cv.arcLength(contours1[i], True)  # 计算得到弧长，形状是闭合的（True）
                approx = cv.approxPolyDP(contours1[i], 0.015 * peri, True)  # 传入轮廓的点集
                x, y, w, h = cv.boundingRect(approx)  # x，y是矩阵左上点的坐标，w，h是矩阵的宽和高
                cv.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)  # 划矩形线把图像包起来
                cv.putText(frame, str(i), (x, y), cv.FONT_HERSHEY_SIMPLEX, 5,
                           [255, 0, 0])  # 各参数依次是：图片，添加的文字，左上角坐标，字体，字体大小，颜色，字体粗细

        cv.namedWindow("image")
        cv.setMouseCallback("image", on_EVENT_LBUTTONDOWN)
        cv.imshow("frame", frame)
        c = cv.waitKey(20)
        if c == 27:
            break
        '''
        if num <= 50:
            #x, y = detect(frame)
            print(x, y)
            if x != 0 or y != 0:#到时候可以卡定范围
                center_shu_x.append(x)
                center_shu_y.append(y)
                num = num + 1
        elif num <= 60:
            center_x = sum(center_shu_x) / len(center_shu_x)
            center_y = sum(center_shu_y) / len(center_shu_y)
            num = num + 1



        if num > 60:
            center = (center_x, center_y)
            print("red:{}".format(center))
            #print("blue:{}".format(center_blue))

            bias_x = int(center_x - x)
            bias_y = int(center_y - y)
            print("偏差x{}， y{}".format(bias_x, bias_y))
            num = num + 1
            if bias_x < 10 and bias_y < 10:
                symbol = 1
        
        
        cv.namedWindow("image")
        cv.setMouseCallback("image", on_EVENT_LBUTTONDOWN)
        cv.imshow("image", frame)

        cv.imshow("frame", frame)
        c = cv.waitKey(20)
        if c == 27:
            break

        '''





main()

cv.waitKey(0)
cv.destroyAllWindows()