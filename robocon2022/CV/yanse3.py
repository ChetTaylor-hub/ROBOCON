# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import cv2
cap = cv2.VideoCapture(1)
center_x=0
center_y=0
center_shu_x=[]
center_shu_y=[]
bias_x=0
bias_y=0
num=0
flag=0
brand=0
symbol=1

def detect(frame):
    o = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    # o=cv2.imread("1.jpg",cv2.IMREAD_GRAYSCALE)
    # o = cv2.resize(o, (1200, 560))
    r1 = cv2.Canny(o, 128, 200)
    r2 = cv2.Canny(o, 32, 128)
    r3 = cv2.Canny(o, 32, 128, L2gradient=True)
    contours1, heriachy1 = cv2.findContours(r3, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)  # 获取轮廓点集(坐标)

    for i, contour in enumerate(contours1):
        peri = cv2.arcLength(contour, True)  # 计算得到弧长，形状是闭合的（True）
        approx = cv2.approxPolyDP(contour, 0.015 * peri, True)  # 传入轮廓的点集
        bian = len(approx)
        # if(bian > 4 and bian < 8):
        area1 = cv2.contourArea(contour)
        print(area1)
        if area1 > 30000 and bian >= 8 and bian <= 10:
            print(bian)
            peri = cv2.arcLength(contours1[i], True)  # 计算得到弧长，形状是闭合的（True）
            approx = cv2.approxPolyDP(contours1[i], 0.015 * peri, True)  # 传入轮廓的点集
            (x, y), radius = cv2.minEnclosingCircle(approx)
            center_blue = (int(x), int(y))
            radius = int(radius)
            return x, y
            # res = cv.circle(frame,center, radius, (0, 255, 0), 2)
            cv2.circle(o1, center_blue, radius, (0, 255, 0), 5)
            # print(center_blue)
            cv2.imshow("original", o1)
            cv2.imshow("result1", r1)
            cv2.imshow("result2", r2)
            cv2.imshow("result3", r3)
        else:
            cv2.imshow("original", o1)
            cv2.imshow("result1", r1)
            cv2.imshow("result2", r2)
            cv2.imshow("result3", r3)
            return 0, 0

    return 0 , 0
while True:

    ret, frame = cap.read()


    def on_EVENT_LBUTTONDOWN(event, x, y, flags, param):
        if event == cv2.EVENT_LBUTTONDOWN:
            xy = "%d,%d" % (x, y)
            cv2.circle(frame, (x, y), 1, (255, 0, 0), thickness=-1)
            cv2.putText(frame, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                       1.0, (0, 0, 0), thickness=1)
            cv2.imshow("image", frame)


    if num <= 50:
        x, y = detect(frame)
        print(x, y)
        if x != 0 or y != 0:  # 到时候可以卡定范围
            center_shu_x.append(x)
            center_shu_y.append(y)
            num = num + 1
    elif num <= 60:
        center_x = sum(center_shu_x) / len(center_shu_x)
        center_y = sum(center_shu_y) / len(center_shu_y)

    if num > 60:
        center = (center_x, center_y)
        print("red:{}".format(center))
        # print("blue:{}".format(center_blue))

        x, y = detect(frame)
        bias_x = center_x - x
        bias_y = center_y - y
        print("偏差x{}， y{}".format(bias_x, bias_y))
        if bias_x < 10 and bias_y < 10:
            symbol = 1

    cv2.imshow("frame", frame)
    c = cv2.waitKey(20)
    if c == 27:
        break
    cv2.namedWindow("image")
    cv2.setMouseCallback("image", on_EVENT_LBUTTONDOWN)
    cv2.imshow("image", frame)

cv2.destroyAllWindows()

