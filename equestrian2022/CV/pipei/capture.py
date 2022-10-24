# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import cv2

capture = cv2.VideoCapture('outputluan(2).avi')
timepoint = 1  ###(单位:秒) 想截取视频中 10s 那一帧
timer = 0  ### 用来计算帧数，
fps = round(capture.get(5))  ### 1 秒有多少帧, 使用的参数是5,表示帧速 (要用怎么自己查表)
### 一般来说，1 秒 约等于 24 帧，我这个视频是 23.976023976023978, 所以直接用round(), 四舍五入,将其约等于 24.
while (capture.isOpened()):  ## 检测视频打开是否有问题
    timer += 1  ## 开始计算过去了多少帧
    ret, frame = capture.read()  ## 逐帧读取视频
    cv2.imwrite('D:\study_struction\opencv\\robocan\equestrian\pic/ ' + str(timer) + 's.jpg', frame)  ## 存图，第一个参数递质，第二个要存的图是哪个
    print(timer)
capture.release()  ## 清内存，好习惯
