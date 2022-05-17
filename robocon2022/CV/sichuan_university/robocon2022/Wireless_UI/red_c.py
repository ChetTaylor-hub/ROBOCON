import cv2
import numpy as np
import wb



def draw_cross(x, y):
    """画图函数"""
    cv2.circle(frame, (x, y), 1, (255, 0, 255), 2)  # 半径足够小的圆就是画点
    cv2.line(frame, (int(x + 5), int(y)), (int(x + 10), int(y)), (255, 0, 255), 2)  # 画出断十字
    cv2.line(frame, (int(x - 5), int(y)), (int(x - 10), int(y)), (255, 0, 255), 2)
    cv2.line(frame, (int(x), int(y + 5)), (int(x), int(y + 10)), (255, 0, 255), 2)
    cv2.line(frame, (int(x), int(y - 5)), (int(x), int(y - 10)), (255, 0, 255), 2)
    # 打印出物体的坐标，坐标可以与单片机通信
    print(x, y)


cv2.namedWindow('image')
cv2.namedWindow('circle')
cv2.createTrackbar('hh', 'image', 0, 255, lambda x: None)
cv2.createTrackbar('hs', 'image', 0, 255, lambda x: None)
cv2.createTrackbar('hv', 'image', 0, 255, lambda x: None)
cv2.createTrackbar('lh', 'image', 0, 255, lambda x: None)
cv2.createTrackbar('ls', 'image', 0, 255, lambda x: None)
cv2.createTrackbar('lv', 'image', 0, 255, lambda x: None)
cv2.createTrackbar('minR', 'circle', 90, 255, lambda x: None)
cv2.createTrackbar('maxR', 'circle', 130, 255, lambda x: None)
# lh = 0
# ls = 85
# lv = 252
# hh = 255
# hs = 255
# hv = 255
# minR = 90
# maxR = 130

cap = cv2.VideoCapture(0,cv2.CAP_DSHOW)
while(1):
    _, frame = cap.read()
    frame = cv2.GaussianBlur(frame, (5, 5), 0)  # 高斯滤波
    frame = wb.white_balance(frame, 5)  # 进行自适应白平衡滤波，效果很好，帧率会降低
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)  # BGR格式转换成HSV颜色格式
    k = np.ones((10, 10), np.uint8)  # 开运算卷积核
    hsv1 = cv2.dilate(cv2.erode(hsv, k), k)  # 开运算，去除亮点

    lh = cv2.getTrackbarPos('lh', 'image')
    ls = cv2.getTrackbarPos('ls', 'image')
    lv = cv2.getTrackbarPos('lv', 'image')
    hh = cv2.getTrackbarPos('hh', 'image')
    hs = cv2.getTrackbarPos('hs', 'image')
    hv = cv2.getTrackbarPos('hv', 'image')

    color_lower = np.array([lh, ls, lv])  # HSV阈值低
    color_upper = np.array([hh, hs, hv])  # HSV阈值高
    mask = cv2.inRange(hsv1, color_lower, color_upper)  # 得到mask，入口参数（图像，HSV颜色低值，HSV颜色高值）
    cv2.imshow('f',mask)
    """寻找色块并绘制中心断十字，发送x，y与中心点的误差到单片机"""
    cnts = cv2.findContours(mask.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
    if len(cnts) > 0:  # 如果找到了颜色
        # cnt = max(cnts, key=cv2.contourArea)  # 找到边缘最长的一块

        minR = cv2.getTrackbarPos('minR', 'circle')
        maxR = cv2.getTrackbarPos('maxR', 'circle') #因为进行了颜色识别，加上半径大概就可以不需要考虑其他的霍夫参数了，要再加。
        minR1 = np.uint8(minR)
        maxR1 = np.uint8(maxR)
        circles = cv2.HoughCircles(mask, cv2.HOUGH_GRADIENT, 1, 50, param1=50, param2=30, minRadius=minR1, maxRadius=maxR1)
        if circles is not None:
            circles = np.uint16(np.around(circles))
            for i in circles[0, :]:
                # 绘制外圆
                cv2.circle(frame, (i[0], i[1]), i[2], (0, 255, 0), 2)
                # 绘制圆心
                cv2.circle(frame, (i[0], i[1]), 2, (0, 0, 255), 3)
                delx = i[0] - 320
                x = abs(delx)

                # if delx < 0:
                #     mpu_ser.com.write("-".encode('utf-8') + str(x).encode('utf-8') + "e".encode('utf-8'))  # 向单片机串口发送x误差
                # elif delx >= 0:
                #     mpu_ser.com.write(str(x).encode('utf-8') + "e".encode('utf-8'))
        else:
            print('莫得圆')
        # area = cv2.contourArea(cnt)  # 得到面积
        # if area > Area:  # 计算该区域的面积,在要求范围内则认为正对基座
        #     (x1, y1) = cv2.minEnclosingCircle(cnt)[0]  # 得到物体的中心坐标
        #     # -------------------------------------------->x
        #     # -(0,0)
        #     # -
        #     # -
        #     # -
        #     # y                            (640,480)
        #     centrex = width / 2  # 摄像头的正中心坐标
        #     centrey = height / 2
        #     m = int(x1)  # 记录色块坐标
        #     n = int(y1)
        #     x_bias = int(m - centrex)  # 计算X与中心的偏差
        #     y_bias = int(n - centrey)  # 计算Y与中心的偏差
        #     mpu_ser.com.write("#".encode() + str(x_bias).encode() + "e".encode())  # 向单片机串口发送x误差，帧头#
        #     mpu_ser.com.write("%".encode() + str(y_bias).encode() + "e".encode())  # 向单片机串口发送y误差，帧头%
        #     draw_cross(m, n)  # 画出断十字
    else:
        print("find no color\r\n")
        pass
    cv2.imshow('frame', frame)  # 实时显示

    if cv2.waitKey(1) & 0xFF == 27:  # 等待1ms，检测按下esc退出
        break
cap.release()
cv2.destroyAllWindows()