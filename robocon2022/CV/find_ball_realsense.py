# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import numpy as np
import cv2  # opencv库
import pyrealsense2 as rs
import serial  # python串口库
import serial.tools.list_ports
import threading
from threading import Thread
import time
import multiprocessing
import random

from matplotlib import pyplot as plt

"""可更改变量"""
color_lower = np.array([60, 141, 25])  # HSV阈值低
color_upper = np.array([174, 255, 255])  # HSV阈值高
light_com_name = "COM7"  # 激光模块串口号,字符串格式
mpu_com_name = "COM6"  # 单片机模块串口号，字符串格式
mode = "fast"  # 设置激光测距模式，标准->"standard"，慢速->"slow"，快速->"fast"
# Configure depth and color streams
frame_width=640
frame_height=480
fps=30
"""卡尔曼迭代次数"""
num=10


"""不可更改变量"""
light_on_flag = False  # 激光打开标注
light_off_flag = False  # 激光关闭标志
dis = 0  # 距离
accuracy = 0  # 精准度

"""一些全局变量"""
bais_x = 0  # 横坐标偏差
dis = 0
x_last = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
p_last = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
Q = [0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1]  # 系统噪声
R = [0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5]  # 测量噪声
y = [[], [], [], [], [], [], [], [], [], []]
Distance = []
color_dist = {
    'forward_yellow_blind_path': {'Lower': np.array([57, 166, 59]), 'Upper': np.array([119, 255, 255])},
    'chin_yellow_blind_path': {'Lower': np.array([10, 40, 120]), 'Upper': np.array([40, 255, 255])},
}


def kalman_2(z_measure, i):
    global x_last, p_last, Q, R
    x_mid = x_last[i]
    p_mid = p_last[i] + Q[i]
    kg = p_mid / (p_mid + R[i])
    x_now = x_mid + kg * (z_measure - x_mid)
    p_now = (1 - kg) * p_mid
    p_last[i] = p_now
    x_last[i] = x_now
    return x_now, p_last[i], x_last[i]


class Realsense(object):
    def __init__(
            self,
            colorizer,
            depth_to_disparity,
            disparity_to_depth,
            decimation,
            spatial,
            temporal,
            hole_filling
    ):
        self.colorizer = colorizer
        self.depth_to_disparity = depth_to_disparity
        self.disparity_to_depth = disparity_to_depth
        self.decimation = decimation
        self.spatial = spatial
        self.temporal = temporal
        self.hole_filling = hole_filling

    def object_color_detect(self, object_color):
        '''
        detect the color boject
        '''
        global depth_frame, color_image, color_dist, frame_width, frame_height, bais_x

        color_image = cv2.GaussianBlur(color_image, (5, 5), 0)  # 高斯滤波
        color_image = white_balance(color_image, 5)  # 进行自适应白平衡滤波，效果很好，帧率会降低
        hsvFrame = cv2.cvtColor(color_image, cv2.COLOR_BGR2HSV)
        # HSV
        color_lower = np.array(color_dist[object_color]["Lower"], np.uint8)
        color_upper = np.array(color_dist[object_color]["Upper"], np.uint8)
        color_mask = cv2.inRange(hsvFrame, color_lower, color_upper)
        cv2.imshow("color_mask", color_mask)

        color_mask = cv2.medianBlur(color_mask, 7)  # 中值滤波

        # kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))  # 矩形结构
        # kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))  # 椭圆结构
        kernel = cv2.getStructuringElement(cv2.MORPH_CROSS, (5, 5))  # 十字形结构

        color_mask = cv2.dilate(color_mask, kernel)  # 膨胀
        kernel = np.ones((10, 10), np.uint8)
        # color_mask = cv2.erode(color_mask, kernel)  # 腐蚀

        res = cv2.bitwise_and(color_image, color_image, mask=color_mask)
        cv2.imshow("Color Detection res in Real-Time", res)

        # Creating contour to track red color
        contours, hierarchy = cv2.findContours(color_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        try:
            cnt = max(contours, key=cv2.contourArea)
            peri = cv2.arcLength(cnt, True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv2.approxPolyDP(cnt, 0.015 * peri, True)  # 传入轮廓的点集
            bian = len(approx)
            if (bian > 4 and bian < 15):
                peri = cv2.arcLength(cnt, True)  # 计算得到弧长，形状是闭合的（True）
                approx = cv2.approxPolyDP(cnt, 0.015 * peri, True)  # 传入轮廓的点集
                (x, y), radius = cv2.minEnclosingCircle(approx)
                center_blue = (int(x), int(y))
                radius = int(radius)
                # res = cv.circle(frame,center, radius, (0, 255, 0), 2)
                cv2.circle(color_image, center_blue, radius, (0, 255, 0), 5)
                cv2.circle(color_image, center_blue, 1, (0, 0, 255), 5)
                bais_x = center_blue[0] - frame_width / 2
                cir_list = [x, y, radius]
                # mpu_ser.com.write("%".encode('utf-8') + str(bais_x).encode('utf-8') + "RC".encode('utf-8'))  # 向单片机串口发送x误差
            else:
                cir_list = []
        except ValueError:
            cir_list = []

        return cir_list

    def object_depth_derect(self, frame):
        frame = self.decimation.process(frame)
        frame = self.depth_to_disparity.process(frame)
        frame = self.spatial.process(frame)
        frame = self.temporal.process(frame)
        # frame = self.disparity_to_depth.process(frame)
        frame = self.hole_filling.process(frame)
        colorized_depth = np.asanyarray(self.colorizer.colorize(frame).get_data())
        cv2.imshow('more depth filter', colorized_depth)
        return frame

    def object_distance_measure(self, cir_list):
        global depth_frame, color_image, p_last, x_last, num, dis
        if cir_list != []:
            centre_x = int(cir_list[0])
            centre_y = int(cir_list[1])
            centre = (centre_x, centre_y)
            radius = int(cir_list[2])

            # 测距的区域
            roi_radius = int(radius/4)
            # print(roi_lx, roi_rx, roi_ty, roi_by)
            color_image = cv2.circle(color_image, centre, roi_radius, (0, 255, 0), 5)

            depth_points = []
            depth_means = []
            # 获取目标框内的物体距离，并进行均值滤波.
            if 280 < centre_x < 400:
                for j in range(20):
                    rand_x = random.randint(centre_x - roi_radius, centre_x + roi_radius)  # 生成roi_lx到roi_rx之间的随机整数
                    rand_y = random.randint(centre_y - roi_radius, centre_y + roi_radius)
                    depth_point = round(depth_frame.get_distance(rand_x, rand_y) * 100, 2)  # 返回浮点数的四舍五入值
                    if depth_point != 0:
                        depth_points.append(depth_point)
                depth_object = np.mean(depth_points)
                Distance.append(depth_object)
                for i in range(num):
                    depth_object, p_last[i], x_last[i] = kalman_2(depth_object, i)
                    y[i].append(depth_object)
                if depth_object >= 30:
                    #print("The camera is facing an object mean ", int(depth_object), " cm away.")
                    dis = round(depth_object, 3)
                else:
                    #print("The camera is facing an object mean <30 cm away.")
                    dis = round(depth_object, 3)


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





def white_balance(img, mode=5):
    """白平衡处理（默认为5, 1均值、2完美反射、3灰度世界、4基于图像分析的偏色检测及颜色校正、5动态阈值）"""
    # 读取图像
    b, g, r = cv2.split(img)
    # 均值变为三通道
    h, w, c = img.shape
    if mode == 1:
        # 默认均值  ---- 简单的求均值白平衡法
        b_avg, g_avg, r_avg = cv2.mean(b)[0], cv2.mean(g)[0], cv2.mean(r)[0]
        # 求各个通道所占增益
        k = (b_avg + g_avg + r_avg) / 3
        kb, kg, kr = k / b_avg, k / g_avg, k / r_avg
        b = cv2.addWeighted(src1=b, alpha=kb, src2=0, beta=0, gamma=0)
        g = cv2.addWeighted(src1=g, alpha=kg, src2=0, beta=0, gamma=0)
        r = cv2.addWeighted(src1=r, alpha=kr, src2=0, beta=0, gamma=0)
        output_img = cv2.merge([b, g, r])
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
        b_avg, g_avg, r_avg = cv2.mean(b)[0], cv2.mean(g)[0], cv2.mean(r)[0]
        # 需要调整的RGB分量的增益
        k = (b_avg + g_avg + r_avg) / 3
        kb, kg, kr = k / b_avg, k / g_avg, k / r_avg
        ba, ga, ra = b * kb, g * kg, r * kr

        output_img = cv2.merge([ba, ga, ra])
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
        output_img = cv2.merge([b0, g, r0])
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

        yuv_img = cv2.cvtColor(img, cv2.COLOR_BGR2YCrCb)
        # YUV空间
        (y, u, v) = cv2.split(yuv_img)
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

        output_img = cv2.merge([b0, g0, r0])
    else:
        raise TypeError('mode should be in [1,2,3,4,5]. Got {}'.format(mode))
    output_img = np.uint8(np.clip(output_img, 0, 255))
    return output_img


def draw_cross(x, y):
    """画图函数"""
    cv2.circle(color_image, (x, y), 1, (255, 0, 255), 2)  # 半径足够小的圆就是画点
    cv2.line(color_image, (int(x + 5), int(y)), (int(x + 10), int(y)), (255, 0, 255), 2)  # 画出断十字
    cv2.line(color_image, (int(x - 5), int(y)), (int(x - 10), int(y)), (255, 0, 255), 2)
    cv2.line(color_image, (int(x), int(y + 5)), (int(x), int(y + 10)), (255, 0, 255), 2)
    cv2.line(color_image, (int(x), int(y - 5)), (int(x), int(y - 10)), (255, 0, 255), 2)
    # 打印出物体的坐标，坐标可以与单片机通信
    print(x, y)


def dis_ac_process(light_ser, mpu_ser):
    while True:
        global bais_x
        dis, accuracy = light_ser.measure(mode=mode)  # 选择模式测距

        print(dis, accuracy, "\r\n")  # 打印出距离和准确度
        mpu_ser.com.write("q".encode() + str(bais_x).encode() + "e".encode())  # 向单片机串口发送激光测出距离，帧头@
        mpu_ser.com.write("r".encode() + str(dis).encode() + "e".encode())  # 向单片机串口发送激光测距准确度，帧头$


def send_UART():
    while True:
        global bais_x, dis
        print(dis, bais_x, "\r\n")  # 打印出距离和准确度
        mpu_ser.com.write("q".encode() + str(bais_x).encode() + "e".encode())  # 向单片机串口发送激光测出距离，帧头@
        mpu_ser.com.write("r".encode() + str(dis).encode() + "e".encode())  # 向单片机串口发送激光测距准确度，帧头$


def realsense_init():

    # 六、复合多个滤波器
    colorizer = rs.colorizer()
    depth_to_disparity = rs.disparity_transform(True)
    disparity_to_depth = rs.disparity_transform(False)
    decimation = rs.decimation_filter()
    spatial = rs.spatial_filter()
    temporal = rs.temporal_filter()
    hole_filling = rs.hole_filling_filter()

    realsense = Realsense(
        colorizer,
        depth_to_disparity,
        disparity_to_depth,
        decimation,
        spatial,
        temporal,
        hole_filling
    )

    return realsense

def light_com_connect():  # 连接激光串口
    flag1 = False
    while flag1 is False:  # 没连接到激光模块串口则一直连接
        port_list = list(serial.tools.list_ports.comports())  # 搜索可用串口
        if len(port_list) == 0:  # 没有可用串口
            print("no useful COM\r\n")
        else:
            print("find com successful\r\n")
            for i in range(0, len(port_list)):
                port = port_list[i]  # 提取使用串口字符串
                port = str(port)
                print("you use: ", port[0:4], "\r\n")
                if port[0:4] == light_com_name:
                    light_ser = Comcontrol(port=light_com_name,  # 激光串口实例化
                                           baudrate=115200,
                                           bytesize=8,
                                           stopbits=1,
                                           timeout=0.6,
                                           parity='N')
                    flag1 = light_ser.com.is_open  # 激光模块是否打开标志
                    if flag1:
                        print("light connection success\r\n")
                        return light_ser
                else:
                    print("connection failed\r\n")


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

    """连接激光串口"""
    #light_ser = light_com_connect()

    """连接单片机串口"""
    mpu_ser = mpu_com_connect()

    """开启激光测距进程"""
    # light_process = multiprocessing.Process(target=dis_ac_process, args=(light_ser, mpu_ser))
    # light_process = Thread(target=dis_ac_process, args=(light_ser, mpu_ser))
    # light_process.start()
    # dis_process = Thread(target=send_UART(), args=())

    """D435初始化"""
    realsense = realsense_init()


    pipeline = rs.pipeline()
    config = rs.config()
    config.enable_stream(rs.stream.depth, frame_width, frame_height, rs.format.z16, fps)
    config.enable_stream(rs.stream.color, frame_width, frame_height, rs.format.bgr8, fps)
    # Start streaming
    pipeline.start(config)


    try:
        while True:

            start = time.time()
            # Wait for a coherent pair of frames: depth and color
            frames = pipeline.wait_for_frames()

            depth_frame = frames.get_depth_frame()
            color_frame = frames.get_color_frame()
            if not depth_frame or not color_frame:
                continue

            # Convert images to numpy arrays
            color_image = np.asanyarray(color_frame.get_data())

            # task program
            realsense.object_depth_derect(depth_frame)
            bbox_list = realsense.object_color_detect("forward_yellow_blind_path")
            realsense.object_distance_measure(bbox_list)

            print(bais_x, dis)
            mpu_ser.com.write("q".encode() + str(bais_x).encode() + "e".encode())  # 向单片机串口发送激光测出距离，帧头@
            #mpu_ser.com.write("r".encode() + str(dis).encode() + "e".encode())  # 向单片机串口发送激光测距准确度，帧头$
            # show image
            cv2.imshow("color_image", color_image)

            #print("FPS:", 1 / (time.time() - start), "/s")

            # Press esc or 'q' to close the image window
            key = cv2.waitKey(1)
            if key & 0xFF == ord('q') or key == 27:
                cv2.destroyAllWindows()
                break
    finally:
        # Stop streaming
        plt.show()
        pipeline.stop()
    plt.plot(Distance, color="b")
    plt.plot(y[0], color="g")
    plt.plot(y[num-1], color="r")
    plt.show()


serial.close()
