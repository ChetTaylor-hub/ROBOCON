# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import threading

import cv2 as cv

import numpy as np


center_blue_x=0
center_blue_y=0
center_red_x=0
center_red_y=0
center_blue_shu_x=[]
center_blue_shu_y=[]
center_red_shu_x=[]
center_red_shu_y=[]
bias_x=0
bias_y=0
num_red=0
num_blue=0
flag=0
brand=0
symbol=1


lower_hsv1 = np.array([92, 47, 123])
upper_hsv1 = np.array([255, 255, 255])

lower_hsv2 = np.array([92, 47, 123])
upper_hsv2 = np.array([255, 255, 255])



def color_detetc(frame, lower_hsv, upper_hsv):
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)  # hsv 色彩空间 分割肤色
    mask1 = cv.inRange(hsv, lowerb=lower_hsv, upperb=upper_hsv)  # hsv 掩码
    return mask1

def detect_area(frame, lower_hsv, upper_hsv):
    mask1 = color_detetc(frame, lower_hsv, upper_hsv)

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


def detect(frame, lower_hsv, upper_hsv):
    mask1 = color_detetc(frame, lower_hsv, upper_hsv)

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
        if area1 > 1000:
            peri = cv.arcLength(contours1[i], True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contours1[i], 0.015 * peri, True)  # 传入轮廓的点集
            (x, y), radius = cv.minEnclosingCircle(approx)
            center_blue = (int(x), int(y))
            radius = int(radius)
            # res = cv.circle(frame,center, radius, (0, 255, 0), 2)
            cv.circle(frame, center_blue, radius, (0, 255, 0), 5)
            # print(center_blue)
            return int(x), int(y)
        else:
            return 0, 0



def main():
    global num_red
    global num_blue
    global center_red_x
    global center_red_y
    global center_blue_x
    global center_blue_y
    global symbol
    global brand
    global flag
    global bias_x
    global bias_y
    capture = cv.VideoCapture(1)  # 打开电脑自带摄像头，如果参数是1会打开外接摄像头
    capture.set(10, 5)

    while True:
        ret, frame = capture.read()

        def on_EVENT_LBUTTONDOWN(event, x, y, flags, param):
            if event == cv.EVENT_LBUTTONDOWN:
                xy = "%d,%d" % (x, y)
                cv.circle(frame, (x, y), 1, (255, 0, 0), thickness=-1)
                cv.putText(frame, xy, (x, y), cv.FONT_HERSHEY_PLAIN,
                            1.0, (0, 0, 0), thickness=1)
                cv.imshow("image", frame)
        if flag == 0 : #获取红色地基中心点坐标
            if num_red <= 50:
                x, y = detect(frame, lower_hsv2, upper_hsv2)
                if x != 0 or y != 0:
                    center_red_shu_x.append(x)
                    center_red_shu_y.append(y)
                print(num_red)
            else:
                center_red_x = sum(center_red_shu_x) / len(center_red_shu_x)
                center_red_y = sum(center_red_shu_y) / len(center_red_shu_y)

            num_red = num_red + 1
        elif flag == 1:
            if num_blue <= 50:
                x, y = detect(frame, lower_hsv1, upper_hsv1)
                if x != 0 or y != 0:
                    center_blue_shu_x.append(x)
                    center_blue_shu_y.append(y)
            else:
                center_blue_x = sum(center_blue_shu_x) / len(center_blue_shu_x)
                center_blue_y = sum(center_blue_shu_y) / len(center_blue_shu_y)
            num_blue = num_blue + 1

        if num_red > 60 or num_blue > 60:
            center_red = (center_red_x, center_red_y)
            center_blue = (center_blue_x, center_blue_y)
            #print("red:{}".format(center_red))
            #print("blue:{}".format(center_blue))

            if brand == 0: #以红塔为地基
                red_base_x, red_base_y = detect(frame, lower_hsv2, upper_hsv2)
                blue_base_x, blue_base_y = detect(frame, lower_hsv1, upper_hsv1)
                if symbol==0: #搭建红积木
                    x, y = detect(frame, lower_hsv2, upper_hsv2)
                    bias_x = red_base_x - x
                    bias_y =  - y
                    print("搭建红积木{}， {}".format(bias_x, bias_y))
                    if bias_x < 10 and bias_y < 10:
                        symbol = 1
                else:
                    x, y = detect(frame, lower_hsv1, upper_hsv1)
                    bias_x = center_red_x - x
                    bias_y = center_red_y - y
                    print("搭建蓝积木{}， {}".format(bias_x, bias_y))
                    if bias_x < 10 and bias_y < 10:
                        symbol = 0
            else: #以蓝塔为地基
                if symbol == 0:
                    x, y = detect(frame, lower_hsv2, upper_hsv2)
                    bias_x = center_red_x - x
                    bias_y = center_red_y - y
                else:
                    x, y = detect(frame, lower_hsv1, upper_hsv1)
                    bias_x = center_red_x - x
                    bias_y = center_red_y - y


        cv.imshow("frame", frame)
        c = cv.waitKey(50)
        if c == 27:
            break
        cv.namedWindow("image")
        cv.setMouseCallback("image", on_EVENT_LBUTTONDOWN)
        cv.imshow("image", frame)






main()

cv.waitKey(0)
cv.destroyAllWindows()