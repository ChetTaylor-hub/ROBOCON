# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import numpy as np
import cv2
from matplotlib import pyplot as plt


from math import pi, exp
from  scipy import ndimage
import cv2
import imutils
import numpy
import numpy as np
import scipy
from matplotlib import pyplot as plt

width = 640  # 分辨率宽
height = 480  # 分辨率高
#cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)  # 捕捉视频
#cap.set(3, width)  # 设置分辨率宽640
#cap.set(4, height)  # 设置分辨率高480

MIN_MATCH_COUNT = 2
img = cv2.imread('B3AB862852FA3A305F4890D09F317517.jpg', 0)
weight, height = img.shape[::-1]
img2 = img.copy()
#img2 = cv2.resize(img2, [1500, 1500])
template1 = cv2.imread('template.jpg', 0)
template2 = cv2.imread('template2.jpg', 0)
template3 = cv2.imread('template3.jpg', 0)
template4 = cv2.imread('template5.png', 0)
w1, h1 = template1.shape[::-1]
w2, h2 = template2.shape[::-1]
w3, h3 = template3.shape[::-1]
w4, h4 = template4.shape[::-1]
templates=[(template1,w1, h1), (template2, w2, h2), (template3, w3, h3), (template4, w4, h4)]
cls=["木棍", "泡沫球", "栏杆", "独木桥"]
MIN_MATCH_COUNT = 2
left_x=10000
right_x=0
top_y=10000
bottom_y=0
bais=100
startX=0
startY=0
endX=0
endY=0
degree_list=[]
max_value=0
flag=0

def SIFT(img, template):
    global left_x, right_x, top_y, bottom_y, degree_list
    # Initiate SIFT detector
    sift = cv2.xfeatures2d.SIFT_create()

    # find the keypoints and descriptors with SIFT
    kp1, des1 = sift.detectAndCompute(template,None)
    kp2, des2 = sift.detectAndCompute(img,None)

    FLANN_INDEX_KDTREE = 0
    index_params = dict(algorithm = FLANN_INDEX_KDTREE, trees = 5)
    search_params = dict(checks = 50)

    flann = cv2.FlannBasedMatcher(index_params, search_params)

    matches = flann.knnMatch(des1,des2,k=2)

    # store all the good matches as per Lowe's ratio test.
    good = []
    for m,n in matches:
        if m.distance < 0.7*n.distance:
            good.append(m)
    if len(good)>MIN_MATCH_COUNT:
        src_pts = np.float32([ kp1[m.queryIdx].pt for m in good ]).reshape(-1,1,2)
        dst_pts = np.float32([ kp2[m.trainIdx].pt for m in good ]).reshape(-1,1,2)

        M, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC,5.0)
        matchesMask = mask.ravel().tolist()

        h,w = template.shape
        pts = np.float32([ [0,0],[0,h-1],[w-1,h-1],[w-1,0] ]).reshape(-1,1,2)
        dst = cv2.perspectiveTransform(pts,M)
        dst = np.int32(abs(dst))
        for i in range(4):
            if dst[i][0][0] < left_x:
                left_x = dst[i][0][0]
            if dst[i][0][0] > right_x:
                right_x = dst[i][0][0]
            if dst[i][0][1] < top_y:
                top_y = dst[i][0][1]
            if dst[i][0][1] > bottom_y:
                bottom_y = dst[i][0][1]
        im3 = img2[top_y:bottom_y, left_x:right_x]
        degree = calculate(im3, template)
        degree_list.append(degree)
        print(degree)

        img = cv2.polylines(img,[np.int32(dst)],True,255,3, cv2.LINE_AA)

    else:
        print("Not enough matches are found - %d/%d" % (len(good),MIN_MATCH_COUNT))
        matchesMask = None
    draw_params = dict(matchColor = (0,255,0), # draw matches in green color
                       singlePointColor = None,
                       matchesMask = matchesMask, # draw only inliers
                       flags = 2)

    img3 = cv2.drawMatches(template,kp1,img,kp2,good,None,**draw_params)
    #plt.savefig("output_pendrive.png")
    plt.imshow(img3, 'gray'),plt.show()


def calculate(image1, image2):
    # 灰度直方图算法
    # 计算单通道的直方图的相似值
    hist1 = cv2.calcHist([image1], [0], None, [256], [0.0, 255.0])
    hist2 = cv2.calcHist([image2], [0], None, [256], [0.0, 255.0])
    # 计算直方图的重合度
    degree = 0
    for i in range(len(hist1)):
        if hist1[i] != hist2[i]:
            degree = degree + \
                (1 - abs(hist1[i] - hist2[i]) / max(hist1[i], hist2[i]))
        else:
            degree = degree + 1
    degree = degree / len(hist1)
    return degree


def scale(gray, template, tW, tH):
    global startX
    global startY
    global endX
    global endY
    # 加载图像，转换为灰度图，初始化用于追踪匹配区域的簿记变量
    found = None
    # 遍历图像尺寸
    for scale in np.linspace(0.2, 1.0, 20)[::-1]:
        # 根据scale比例缩放图像，并保持其宽高比
        resized = imutils.resize(gray, width=int(gray.shape[1] * scale))
        r = gray.shape[1] / float(resized.shape[1])

        # 缩放到图像比模板小，则终止
        if resized.shape[0] < tH or resized.shape[1] < tW:
            break

        # 在缩放后的灰度图中检测边缘，进行模板匹配
        # 使用与模板图像完全相同的参数计算图像的Canny边缘表示；
        # 使用cv2.matchTemplate应用模板匹配；
        # cv2.minMaxLoc获取相关结果并返回一个4元组，其中分别包含最小相关值、最大相关值、最小值的（x，y）坐标和最大值的（x，y）坐标。我们只对最大值和（x，y）-坐标感兴趣，所以只保留最大值而丢弃最小值。
        edged = cv2.Canny(resized, 50, 200)
        result = cv2.matchTemplate(edged, template, cv2.TM_CCOEFF)
        (_, maxVal, _, maxLoc) = cv2.minMaxLoc(result)


        # 如果我们找到了一个新的最大校正值，更新簿记变量值
        if found is None or maxVal > found[0]:
            found = (maxVal, maxLoc, r)

        # 解包簿记变量并基于调整大小的比率，计算边界框（x，y）坐标
        (_, maxLoc, r) = found
        (startX, startY) = (int(maxLoc[0] * r), int(maxLoc[1] * r))
        (endX, endY) = (int((maxLoc[0] + tW) * r), int((maxLoc[1] + tH) * r))
    return startX, startY, endX, endY

def tes(img2, template, w, h, weight, height, bais):
    # All the 6 methods for comparison in a list
    methods = ['cv2.TM_CCOEFF', 'cv2.TM_CCOEFF_NORMED', 'cv2.TM_CCORR',
               'cv2.TM_CCORR_NORMED', 'cv2.TM_SQDIFF', 'cv2.TM_SQDIFF_NORMED']
    methods1 = ['cv2.TM_CCOEFF']
    for meth in methods1:
        img = img2.copy()
        template_ori = template.copy()
        template = cv2.Canny(template, 64, 128)
        # eval 语句用来计算存储在字符串中的有效 Python 表达式
        method = eval(meth)

        pos = scale(img, template, w, h)
        '''
        # 模板匹配
        res = cv2.matchTemplate(img, template, method)
        # 寻找最值
        min_val, max_val, min_loc, max_loc = cv2.minMaxLoc(res)
        # 使用不同的比较方法，对结果的解释不同
        if method in [cv2.TM_SQDIFF, cv2.TM_SQDIFF_NORMED]:
            top_left = min_loc
        else:
            top_left = max_loc
        '''
        top_left = (pos[0], pos[1])
        bottom_right = (pos[2], pos[3])
        print(top_left, bottom_right)
        im = img[top_left[1]:bottom_right[1], top_left[0]:bottom_right[0]]
        degree = calculate(im, template_ori)
        degree_list.append(degree)
        print(degree)

        #if (weight/2+bais) > (top_left[0] + bottom_right[0])/2 > (weight/2-bais) and (height/2+bais) > (top_left[1] + bottom_right[1])/2 > (height/2-bais):
        cv2.rectangle(img, top_left, bottom_right, 255, 2)
        #plt.subplot(121), plt.imshow(im, cmap='gray')
        #plt.title('Matching Result', plt.xticks([]), plt.yticks([]))
        plt.subplot(122), plt.imshow(img, cmap='gray')
        plt.title('Detected Point'), plt.xticks([]), plt.yticks([])
        plt.suptitle(meth)
        plt.show()


for template, w, h in templates:
    #tes(img, template, w, h, weight, height, bais)
    SIFT(img, template)


for i, degree in enumerate(degree_list):
    if max_value < degree:
        max_value = degree
        flag = i

print(cls[flag], max_value)