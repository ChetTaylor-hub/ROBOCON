# -*- coding: utf-8 -*-
import cv2
import numpy as np
import wb

def shot(img, x, y, color):
    cv2.circle(img, (x, y), 3, color, -1)
    cv2.line(img, (x - 10, y), (x - 25, y), color, 3)
    cv2.line(img, (x + 10, y), (x + 25, y), color, 3)
    cv2.line(img, (x, y - 10), (x, y - 25), color, 3)
    cv2.line(img, (x, y + 10), (x, y + 25), color, 3)


def find_target(img, color):
    cnt, hierarchy = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    if len(cnt):
        areas = []
        for c in range(len(cnt)):
            areas.append(cv2.contourArea(cnt[c]))
            m_id = areas.index(max(areas))
        M = cv2.moments(cnt[m_id])
        cx = int(M['m10'] / M['m00'])
        cy = int(M['m01'] / M['m00'])
        if cv2.contourArea(cnt[m_id]) > 200:
            print("Location:%f, %f" % (cx, cy))
            shot(img, cx, cy, color)
            return cx, cy
    return 0, 0


def smooth(capture, core):
    erosion = cv2.erode(capture, core, iterations=1)
    dilation = cv2.dilate(erosion, core, iterations=3)
    smooth_out = cv2.medianBlur(dilation, 1)
    return smooth_out

class Auto_color():
    def __init__(self, frame, mpu_ser):
        self.frame = frame
        self.mpu_ser = mpu_ser

    def tower_red(self):
        lower_red_1 = np.array([0, 209, 99])
        upper_red_1 = np.array([10, 255, 255])
        lower_red_2 = np.array([122, 105, 151])
        upper_red_2 = np.array([179, 255, 255])
        color_blue = (255, 245, 0)  # 准心蓝色

        dst = cv2.medianBlur(self.frame, 3)
        hsv = cv2.cvtColor(dst, cv2.COLOR_BGR2HSV)
        mask_1 = cv2.inRange(hsv, lower_red_1, upper_red_1)
        mask_2 = cv2.inRange(hsv, lower_red_2, upper_red_2)
        mask = cv2.bitwise_or(mask_1, mask_2)
        kernel = np.ones((3, 3), np.uint8)
        mask_blur = smooth(mask, kernel)
        now_x, now_y = find_target(mask_blur, color_blue)

        centrex = 640 / 2  # 摄像头的正中心坐标
        centrey = 480 / 2
        if now_x != 0:
            m = int(now_x)  # 记录色块坐标
            n = int(now_y)
            x_bias = int(m - centrex)  # 计算X与中心的偏差
            y_bias = int(n - centrey)  # 计算Y与中心的偏差
            shot(self.frame, m, n, color_blue)  # 画中心点

            if abs(x_bias)>3:
                self.mpu_ser.com.write("#".encode() + str(x_bias).encode() + "RC".encode())  # 向单片机串口发送x误差，帧头#
                print("中心坐标偏移量为:%d" % x_bias)
        else:
            print("the aera is too small that can't regard it as target")
        # self.frame = cv2.cvtColor(self.frame, cv2.COLOR_HSV2BGR)
        return self.frame


    def tower_blue(self):
        lower_blue = np.array([47, 100, 84])
        upper_blue = np.array([144, 255, 255])
        color_blue = (255, 245, 0)  # 准心蓝色

        dst = cv2.medianBlur(self.frame, 3)
        hsv = cv2.cvtColor(dst, cv2.COLOR_BGR2HSV)
        mask = cv2.inRange(hsv, lower_blue, upper_blue)
        kernel = np.ones((3, 3), np.uint8)
        mask_blur = smooth(mask, kernel)
        now_x, now_y = find_target(mask_blur, color_blue)

        centrex = 640 / 2  # 摄像头的正中心坐标
        centrey = 480 / 2
        if now_x != 0:
            m = int(now_x)  # 记录色块坐标
            n = int(now_y)
            x_bias = int(m - centrex)  # 计算X与中心的偏差
            y_bias = int(n - centrey)  # 计算Y与中心的偏差
            shot(self.frame, m, n, color_blue)  # 画中心点

            if abs(x_bias) > 3:
                self.mpu_ser.com.write("#".encode() + str(x_bias).encode() + "RC".encode())  # 向单片机串口发送x误差，帧头#
                print("中心坐标偏移量为:%d" % x_bias)
        else:
            print("the aera is too small that can't regard it as target")
        # self.frame = cv2.cvtColor(self.frame, cv2.COLOR_HSV2BGR)
        return self.frame


    def ball_red(self):
        red_lower = np.array([0, 112, 103])  # HSV阈值低
        red_upper = np.array([255, 248, 255])  # HSV阈值高

        frame = self.frame
        frame = cv2.GaussianBlur(frame, (5, 5), 0)  # 高斯滤波
        #frame = wb.white_balance(frame, 5)  # 进行自适应白平衡滤波，效果很好，帧率会降低
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式


        mask = cv2.inRange(hsv, red_lower, red_upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
        ker = cv2.getStructuringElement(cv2.MORPH_RECT, (13, 13))
        mask = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, ker)


        """寻找色块并绘制中心断十字，发送x，y与中心点的误差到单片机"""
        cnts = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
        mask1 = np.ones_like(frame)
        if len(cnts) > 0:  # 如果找到了颜色
            cnt = max(cnts, key=cv2.contourArea)
            x, y, w, h = cv2.boundingRect(cnt)
            reg = np.array(
                [[x, y], [x, y + h + 100], [x + w, y + h + 100], [x + w, y]])  # jia 100 jiang bai se chu nongchu
            cv2.fillConvexPoly(mask1, reg, 255)

            img1 = cv2.bitwise_and(mask1, frame)
            gray = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
            canny = cv2.Canny(gray, 20, 50)
            circles = cv2.HoughCircles(canny, cv2.HOUGH_GRADIENT, 1, 5000, param1=30, param2=7, minRadius=210,
                                       maxRadius=225)


            if circles is not None:
                circles = np.uint16(np.around(circles))
                for i in circles[0, :]:
                    if 120 < i[1] < 180:
                        # 绘制外圆
                        cv2.circle(frame, (i[0], i[1]), i[2], (0, 255, 0), 2)
                        print(i[1])
                        # 绘制圆心
                        cv2.circle(frame, (i[0], i[1]), 2, (0, 0, 255), 3)
                        delx = i[0] - 340
                        if abs(delx)>3:
                            self.mpu_ser.com.write("%".encode('utf-8') + str(delx).encode('utf-8') + "RC".encode('utf-8'))  # 向单片机串口发送x误差

            else:
                print('莫得圆')

        else:
            print("find no color\r\n")
            pass
        # frame = cv2.cvtColor(frame, cv2.COLOR_HSV2BGR)
        return frame

    def ball_blue(self):
        blue_lower = np.array([45, 55, 64])  # HSV阈值低
        blue_upper = np.array([255, 255, 255])  # HSV阈值高

        frame = self.frame
        frame = cv2.GaussianBlur(frame, (5, 5), 0)  # 高斯滤波
        #frame = wb.white_balance(frame, 5)  # 进行自适应白平衡滤波，效果很好，帧率会降低
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式

        mask = cv2.inRange(hsv, blue_lower, blue_upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
        ker = cv2.getStructuringElement(cv2.MORPH_RECT, (13, 13))
        mask = cv2.morphologyEx(mask,cv2.MORPH_CLOSE,ker)

        """寻找色块并绘制中心断十字，发送x，y与中心点的误差到单片机"""
        cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
        mask1 = np.ones_like(frame)
        if len(cnts) > 0:  # 如果找到了颜色
            cnt = max(cnts, key=cv2.contourArea)
            x, y, w, h = cv2.boundingRect(cnt)
            reg = np.array(
                [[x, y], [x, y + h + 100], [x + w, y + h + 100], [x + w, y]])  # jia 100 jiang bai se chu nongchu
            cv2.fillConvexPoly(mask1, reg, 255)

            img1 = cv2.bitwise_and(mask1, frame)
            gray = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
            canny = cv2.Canny(gray, 20, 50)
            circles = cv2.HoughCircles(canny, cv2.HOUGH_GRADIENT, 1, 5000, param1=30, param2=7, minRadius=210,
                                       maxRadius=225)


            if circles is not None:
                circles = np.uint16(np.around(circles))
                for i in circles[0, :]:
                    if 120 < i[1] < 180:
                        # 绘制外圆
                        cv2.circle(frame, (i[0], i[1]), i[2], (0, 255, 0), 2)
                        print(i[1])
                        # 绘制圆心
                        cv2.circle(frame, (i[0], i[1]), 2, (0, 0, 255), 3)
                        delx = i[0] - 320
                        if abs(delx) > 3:
                            self.mpu_ser.com.write("%".encode('utf-8') + str(delx).encode('utf-8') + "RC".encode('utf-8'))  # 向单片机串口发送x误差

            else:
                print('莫得圆')

        else:
            print("find no color\r\n")
            pass
        # frame = cv2.cvtColor(frame, cv2.COLOR_HSV2BGR)
        return frame