import numpy as np
import cv2  # opencv库
import serial  # python串口库
import serial.tools.list_ports
import wb


"""可更改变量"""

red_lower = np.array([0, 80, 100])  # HSV阈值低
red_upper = np.array([255, 255, 255])  # HSV阈值高

"""不可更改变量"""



"""主进程用以开启子进程"""
def seee_red(cap,mpu_ser):
        _, frame = cap.read()
        frame = cv2.GaussianBlur(frame, (5, 5), 0)  # 高斯滤波
        frame = wb.white_balance(frame, 5)  # 进行自适应白平衡滤波，效果很好，帧率会降低
        hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式
        k = np.ones((5, 5), np.uint8)  # 开运算卷积核
        hsv1 = cv2.dilate(cv2.erode(hsv, k), k)  # 开运算，去除亮点

        mask = cv2.inRange(hsv1, red_lower, red_upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）

        """寻找色块并绘制中心断十字，发送x，y与中心点的误差到单片机"""
        cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
        if len(cnts) > 0:  # 如果找到了颜色
            # cnt = max(cnts, key=cv2.contourArea)  # 找到边缘最长的一块



            circles = cv2.HoughCircles(mask, cv2.HOUGH_GRADIENT, 1, 50, param1=50, param2=8, minRadius=220,
                                       maxRadius=300)
            if circles is not None:
                circles = np.uint16(np.around(circles))
                for i in circles[0, :]:
                    # 绘制外圆
                    cv2.circle(frame, (i[0], i[1]), i[2], (0, 255, 0), 2)
                    # 绘制圆心
                    cv2.circle(frame, (i[0], i[1]), 2, (0, 0, 255), 3)
                    delx = i[0] - 320
                    mpu_ser.com.write("%".encode('utf-8') + str(delx).encode('utf-8') + "RC".encode('utf-8'))  # 向单片机串口发送x误差

            else:
                print('莫得圆')

        else:
            print("find no color\r\n")
            pass
        cv2.imshow('frame', frame)  # 实时显示



