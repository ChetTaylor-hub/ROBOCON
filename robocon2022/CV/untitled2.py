# -*- coding: utf-8 -*-
"""
Created on Sun Mar 28 17:43:21 2021

@author: zhoujingtao
"""
import cv2 as cv
import numpy as np


def color_detetc(frame):
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)  # hsv 色彩空间 分割肤色
    lower_hsv1 = np.array([-10, 96, 56])
    upper_hsv1 = np.array([16, 255, 255])

    # lower_hsv1 = np.array([101,100,100])
    # upper_hsv1 = np.array([154,255,255])
    mask1 = cv.inRange(hsv, lowerb=lower_hsv1, upperb=upper_hsv1)  # hsv 掩码
    cv.imshow("frame2", mask1)
    lower_hsv2 = np.array([132, 199, 126])
    upper_hsv2 = np.array([179, 255, 255])
    # lower_hsv2 = np.array([171,125,171])
    # upper_hsv2 = np.array([182,255,255])
    mask2 = cv.inRange(hsv, lowerb=lower_hsv2, upperb=upper_hsv2)  # hsv 掩码
    cv.imshow("frame3", mask2)
    return mask1, mask2


def main():
    capture = cv.VideoCapture(1)  # 0打开电脑自带摄像头，如果参数是1会打开外接摄像头
    while True:
        ret, frame = capture.read()
        mask1, mask2 = color_detetc(frame)
        xred = []
        xblue = []
        bottle = [[180, 250], [280, 340], [370, 430]]
        mask1 = cv.erode(mask1, None, iterations=1)  # 腐蚀去毛刺（完善图片边角）
        mask1 = cv.dilate(mask1, None, iterations=4)  # 膨胀恢复图像（适当调整大小可以实现将图片中间的一些无用特征去掉)
        mask2 = cv.erode(mask2, None, iterations=1)  # 腐蚀去毛刺（完善图片边角）
        mask2 = cv.dilate(mask2, None, iterations=4)  # 膨胀恢复图像（适当调整大小可以实现将图片中间的一些无用特征去掉)

        contours1, heriachy1 = cv.findContours(mask1, cv.RETR_EXTERNAL,
                                               cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标) #mask1二值图， cv.RETR_EXTERNAL取外轮廓模式， CV_CHAIN_APPROX_SIMPLE：压缩水平的、垂直的和斜的部分，也就是，函数只保留他们的终点部分
        contours2, heriachy2 = cv.findContours(mask2, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)
        for i, contour in enumerate(contours1):
            peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集 #把光滑曲线折线化
            bian = len(approx)
            if (1):
                area1 = cv.contourArea(contour)  # 计算轮廓面积
                if area1 > 400:
                    peri = cv.arcLength(contours1[i], True)  # 计算得到弧长，形状是闭合的（True）
                    approx = cv.approxPolyDP(contours1[i], 0.015 * peri, True)  # 传入轮廓的点集
                    x, y, w, h = cv.boundingRect(approx)  # x，y是矩阵左上点的坐标，w，h是矩阵的宽和高
                    cv.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)  # 划矩形线把图像包起来
                    cv.putText(frame, str(i), (x, y), cv.FONT_HERSHEY_SIMPLEX, 5,
                               [255, 0, 0])  # 各参数依次是：图片，添加的文字，左上角坐标，字体，字体大小，颜色，字体粗细
                    xblue.append(int(x + (w / 2)))

        for k, contour in enumerate(contours2):
            peri = cv.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
            # approx = cv2.approxPolyDP(cnt,2.8,True)
            bian = len(approx)
            if (1):
                area2 = cv.contourArea(contour)
                if (area2 > 400):
                    peri = cv.arcLength(contours2[k], True)  # 计算得到弧长，形状是闭合的（True）
                    approx = cv.approxPolyDP(contours2[k], 0.015 * peri, True)  # 传入轮廓的点集
                    x, y, w, h = cv.boundingRect(approx)  # x，y是矩阵左上点的坐标，w，h是矩阵的宽和高
                    cv.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
                    cv.putText(frame, str(k), (x, y), cv.FONT_HERSHEY_SIMPLEX, 5, [0, 0, 255])
                    xred.append(int(x + (w / 2)))
        xred.sort()
        xblue.sort()
        if (len(xred) < 3):
            for i in range(0, (3 - len(xred))):
                xred.append(0)
        if (len(xblue) < 3):
            for i in range(0, (3 - len(xblue))):
                xblue.append(0)
        for r in range(0, 3):  # 如果一个壶被挡住，假如是第三个壶被挡住，没有识别出来，数组中会补零，排序后就变成了第一个位置，此代码实现正确排序
            for i in range(0, len(xred)):
                if (xred[i] > bottle[r][0] and xred[i] < bottle[r][1]):
                    if (i == r):
                        xred[r] = xred[i]
                    else:
                        xred[r] = xred[i]
                        xred[i] = 0

        for b in range(0, 3):
            for i in range(0, len(xblue)):
                if (xblue[i] > bottle[b][0] and xblue[i] < bottle[b][1]):
                    if (i == b):
                        pass
                    else:
                        xblue[b] = xblue[i]
                        xblue[i] = 0

        for j in range(0, 3):
            if (xred[j] > xblue[j]):
                if (xred[j] == 0):
                    print("红:" + str(2 * j + 1) + "蓝:" + str(2 * j + 2))
                elif (xblue[j] == 0):
                    print("红:" + str(2 * j + 2) + "蓝:" + str(2 * j + 1))
                else:
                    print("红:" + str(2 * j + 2) + "蓝:" + str(2 * j + 1))
            else:
                if (xred[j] == 0):
                    print("红:" + str(2 * j + 2) + "蓝:" + str(2 * j + 1))
                elif (xblue[j] == 0):
                    print("红:" + str(2 * j + 1) + "蓝:" + str(2 * j + 2))
                else:
                    print("红:" + str(2 * j + 1) + "蓝:" + str(2 * j + 2))
        cv.imshow("frame", frame)
        c = cv.waitKey(50)
        if c == 27:
            break


main()

cv.waitKey(0)
cv.destroyAllWindows()
