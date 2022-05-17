from ball_3 import Ui_Camshow
import sys
import numpy as np
from PyQt5 import QtCore
from PyQt5.QtWidgets import QApplication, QMainWindow, QMessageBox
from PyQt5.QtCore import QTimer
import cv2
import time
import serial
from Auto_Color import Auto_color


class CamShow(QMainWindow, Ui_Camshow):
    def __init__(self, parent=None):
        super(CamShow, self).__init__(parent)
        self.cap = cv2.VideoCapture()
        self.Image_num = 0    # 用于计算帧率
        self.time_now = 0     # 用于计算帧率
        self.image = np.ndarray((640, 480, 3), dtype='uint8')
        self.x = 100
        self.y = 100
        self.timer_camera = QTimer()    # 时钟定义
        self.mode = 0  # 自动模式
        self.id = 0
        self.setupUi(self)          # 初始化界面
        self.call_back_functions()    # 界面运行逻辑

    def start_camera(self):
        self.connectButton.setText("正在连接")       # 显示正在连接
        if not self.timer_camera.isActive():       # 判断时钟是否开始运行,若此时状态是没有运行相机，则进入开启

            flag = self.cap.open(1)     # 打开相机
            self.cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
            self.cap.set(3, 640)  # 设置分辨率宽640
            self.cap.set(4, 480)
            self.timer_camera.start(30)
            if not flag:
                self.connectButton.setText('开启失败')
            else:
                self.time_now = time.perf_counter()
                self.connectButton.setText('切换射头')
        else:                                      # 若此时是已经运行相机，则进入重启
            self.timer_camera.stop()
            self.cap.release()
            self.connectButton.setText('重新连接')

    def start1_camera(self):
        self.connectButton.setText("正在连接")       # 显示正在连接
        if not self.timer_camera.isActive():
            self.cap = cv2.VideoCapture()  # 判断时钟是否开始运行,若此时状态是没有运行相机，则进入开启
            flag1 = self.cap.open(0)     # 打开相机
            self.cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter_fourcc('M', 'J', 'P', 'G'))
            self.cap.set(cv2.CAP_PROP_BRIGHTNESS, 135)  # 亮度设置
            # # cap1.set(cv2.CAP_PROP_CONTRAST, 45)  # 对比度
            # # # cap1.set(cv2.CAP_PROP_EXPOSURE, -2)  # 快门
            self.cap.set(cv2.CAP_PROP_SATURATION, 100)  # 饱和度
            self.cap.set(3, 640)  # 设置分辨率宽640
            self.cap.set(4, 480)
            self.timer_camera.start(30)
            if not flag1:
                self.connectButton.setText('开启失败')
            else:
                self.time_now = time.perf_counter()
                self.connectButton.setText('切换射头')
        else:                                      # 若此时是已经运行相机，则进入重启
            self.timer_camera.stop()
            self.cap.release()
            self.connectButton.setText('重新连接')

    def show_camera(self):
        mode = self.mode

        if (mode == 3) or (mode == 1):
            flag, self.image = self.cap.read()
        else:
            flag, self.image = self.cap.read()
        self.Image_num += 1
        if self.Image_num % 10 == 9:
            frame_rate = 10 / (time.perf_counter() - self.time_now)
            self.Fps_lcd.display(frame_rate)
            self.time_now = time.perf_counter()
        ac = Auto_color(self.image, mpu_ser)
        '''试试放init里'''
        if mode == 0:
            show = cv2.resize(self.image, (1280, 960))
            show = cv2.cvtColor(show, cv2.COLOR_BGR2RGB)
            show = self.draw_shot(show)
        elif mode == 4:
            show = ac.tower_blue()
            show = cv2.resize(show, (1280, 960))
            show = self.draw_shot(show)
        elif mode == 2:
            show = ac.tower_red()
            show = cv2.resize(show, (1280, 960))
            show = self.draw_shot(show)
        elif mode == 3:
            show = ac.ball_red()
            show = cv2.resize(show, (1280, 960))
            show = self.draw_shot(show)
        elif mode == 1:
            show = ac.ball_blue()
            show = cv2.resize(show, (1280, 960))
            show = self.draw_shot(show)

        cv2.imshow('win', show)


    def draw_shot(self, show):
        cv2.line(show, (640, 400), (640, 700), (255, 0, 255), 2)
        self.draw_dis(show, 780, 480, '0')
        self.draw_dis(show, 740, 550, '5')
        self.draw_dis(show, 700, 620, '10')
        self.draw_small(show)
        return show

    def call_back_functions(self):
        self.connectButton.clicked.connect(self.clickButton_cont)
        self.connectButton_2.clicked.connect(self.clickButton)
        self.connectButton_3.clicked.connect(self.clickButton)
        self.connectButton_4.clicked.connect(self.clickButton)
        self.connectButton_5.clicked.connect(self.clickButton)
        self.connectButton_6.clicked.connect(self.clickButton)
        self.connectButton_7.clicked.connect(self.clickButton)
        self.pushButton.clicked.connect(self.close)
        self.sudu.valueChanged.connect(self.valueChange)

    def valueChange(self):
        self.speed.display(self.sudu.value())
        val_speed = str(self.sudu.value())
        mpu_ser.com.write("@S".encode('utf-8') + val_speed.encode('utf-8') + "RC".encode('utf-8'))

    def clickButton_cont(self):
        if self.timer_camera.isActive():
            self.timer_camera.stop()
            self.cap.release()
        self.start1_camera()
        self.mode = 0


    def clickButton(self):
        sender = self.sender()
        if sender.text() == '蓝色取球':
            if self.timer_camera.isActive():
                self.timer_camera.stop()
                self.cap.release()
            self.start_camera()
            print("蓝色取球点击")
            self.mode = 1
            print('ok')
            # mpu_ser.com.write("@".encode('utf-8') + "2".encode('utf-8') + "RC".encode('utf-8'))
        elif sender.text() == '红色射塔':
            if self.timer_camera.isActive():
                self.timer_camera.stop()
                self.cap.release()
            print("红色射塔点击")
            self.start1_camera()
            self.mode = 2
            print('ok')
            # mpu_ser.com.write("@".encode('utf-8') + "3".encode('utf-8') + "RC".encode('utf-8'))
        elif sender.text() == '红色取球':
            if self.timer_camera.isActive():
                self.timer_camera.stop()
                self.cap.release()
            print("红色取球点击")
            self.start_camera()
            self.mode = 3
            print('ok')
            # mpu_ser.com.write("@".encode('utf-8') + "4".encode('utf-8') + "RC".encode('utf-8'))
        elif sender.text() == '蓝色射塔':
            if self.timer_camera.isActive():
                self.timer_camera.stop()
                self.cap.release()
            print("蓝色射塔点击")

            self.start1_camera()
            self.mode = 4
            self.timer_camera.timeout.connect(self.show_camera)
            print('ok')
            # mpu_ser.com.write("@".encode('utf-8') + "1".encode('utf-8') + "RC".encode('utf-8'))
        elif sender.text() == '取球':
            print('测试：取球f')
            mpu_ser.com.write("@".encode('utf-8') + "f".encode('utf-8') + "RC".encode('utf-8'))
        elif sender.text() == '上膛':
            print('测试：上膛h')
            mpu_ser.com.write("@".encode('utf-8') + "h".encode('utf-8') + "RC".encode('utf-8'))
        else:
            pass

    def closeEvent(self, event):
        print("关闭的操作")
        result = QMessageBox.question(self, "注意：", "您真的要关闭窗体吗？", QMessageBox.Yes | QMessageBox.No, QMessageBox.No)
        if result == QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()
            QMessageBox.information(self, "消息", "射爆")

    def keyPressEvent(self, event):
        ke_1 = event.key()
        mpu_ser.com.write("@".encode('utf-8') + str(ke_1).encode('utf-8') + "RC".encode('utf-8'))
        print(ke_1)
        # if event.key() == 65:
        #     print('测试：a')
        #     mpu_ser.com.write("@".encode('utf-8') + "a".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 83:
        #     print('测试：s')
        #     mpu_ser.com.write("@".encode('utf-8') + "s".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 45:
        #     # 减号退出
        #     print('减号退出')
        #     self.pushButton.click()
        # elif event.key() == 87:
        #     print('测试：w')
        #     mpu_ser.com.write("@".encode('utf-8') + "w".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 68:
        #     print('测试：d')
        #     mpu_ser.com.write("@".encode('utf-8') + "d".encode('utf-8') + "RC".encode('utf-8'))
        #
        # elif event.key() == 74:
        #     print('测试：j')
        #     mpu_ser.com.write("@".encode('utf-8') + "j".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 76:
        #     print('测试：l')
        #     mpu_ser.com.write("@".encode('utf-8') + "l".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 75:
        #     print('测试：k')  # 75实际是k，通信有错，这里改方便
        #     mpu_ser.com.write("@".encode('utf-8') + "i".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 73:
        #     print('测试：i')
        #     mpu_ser.com.write("@".encode('utf-8') + "k".encode('utf-8') + "RC".encode('utf-8'))
        # # elif event.key() == 53:
        # #     print('测试：停止自动模式')
        # #     mpu_ser.com.write("@".encode('utf-8') + "4".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 70:
        #     print('测试：f')
        #     mpu_ser.com.write("@".encode('utf-8') + "f".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 72:
        #     print('测试：上膛h')
        #     mpu_ser.com.write("@".encode('utf-8') + "h".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 69:
        #     print('测试：切换自控/手控e')
        #     mpu_ser.com.write("@".encode('utf-8') + "e".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 90:
        #     print('测试：切换遥控器控制z')
        #     mpu_ser.com.write("@".encode('utf-8') + "z".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 81:
        #     print('测试：移动停止q')
        #     mpu_ser.com.write("@".encode('utf-8') + "q".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 79:
        #     print('测试：舵轮左旋o')
        #     mpu_ser.com.write("@".encode('utf-8') + "o".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 80:
        #     print('测试：舵轮右旋p')
        #     mpu_ser.com.write("@".encode('utf-8') + "p".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 88:
        #     print('测试：解锁轮子')
        #     mpu_ser.com.write("@".encode('utf-8') + "x".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 67:
        #     print('测试：强锁')
        #     mpu_ser.com.write("@".encode('utf-8') + "c".encode('utf-8') + "RC".encode('utf-8'))
        # elif event.key() == 49:
        #     print('测试：蓝色射塔1')


        #     self.connectButton_3.click()
        # elif event.key() == 50:
        #     print('测试：蓝色取球2')
        #     self.connectButton_2.click()
        # elif event.key() == 51:
        #     print('测试：红色射塔3')
        #     self.connectButton_4.click()
        # elif event.key() == 52:
        #     print('测试：红色取球4')
        #     self.connectButton_5.click()
        # else:
        #     pass

    def keyReleaseEvent(self, event):
        mpu_ser.com.write("@".encode('utf-8') + str(81).encode('utf-8') + "RC".encode('utf-8'))

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            print("鼠标左键点击")
            # mpu_ser.com.write("@".encode('utf-8') + "f".encode('utf-8') + "RC".encode('utf-8'))
        else:
            pass

    def draw_cross(self, frame, x, y):
        """画图函数"""
        cv2.circle(frame, (x, y), 1, (255, 0, 255), 2)  # 半径足够小的圆就是画点
        cv2.line(frame, (int(x + 5), int(y)), (int(x + 10), int(y)), (255, 0, 255), 1)  # 画出断十字
        cv2.line(frame, (int(x - 5), int(y)), (int(x - 10), int(y)), (255, 0, 255), 1)
        cv2.line(frame, (int(x), int(y + 5)), (int(x), int(y + 10)), (255, 0, 255), 1)
        cv2.line(frame, (int(x), int(y - 5)), (int(x), int(y - 10)), (255, 0, 255), 1)

    def draw_dis(self, show, x, y, meter):
        cv2.line(show, (1280-x, y), (x, y), (255, 0, 255), 2)
        cv2.putText(show, meter, (x+10, y), cv2.FONT_HERSHEY_PLAIN, 1, (0, 255, 0), thickness=2)

    def draw_small(self, show):
        for i in range(20):
            cv2.line(show, (640, 480+10*i), (660, 480+10*i), (255, 0, 255), 1)

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
        if not self.com.is_open:
            self.com.open()

    def read_data(self):

        while True:
            data = self.com.readline().decode('utf-8')

            print(data)
            if data is not None:
                ui.distance.display(data[0:5])

if __name__ == '__main__':
    mpu_ser = Comcontrol(port='COM1',  # 单片机串口实例化/dev/ttyTHS0     COM1
                             baudrate=115200,
                             bytesize=8,
                             stopbits=1,
                             timeout=0.8,
                             parity='N')
    app = QApplication(sys.argv)
    ui = CamShow()
    ui.show()
    cv2.waitKey(0)
    sys.exit(app.exec_())
