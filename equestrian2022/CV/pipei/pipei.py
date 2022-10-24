from math import pi, exp

import cv2
import imutils
import numpy
import numpy as np
import scipy
from matplotlib import pyplot as plt

width = 640  # 分辨率宽
height = 480  # 分辨率高
cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)  # 捕捉视频
cap.set(3, width)  # 设置分辨率宽640
cap.set(4, height)  # 设置分辨率高480


img = cv2.imread('1s.jpg', 0)
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
bais=100
startX=0
startY=0
endX=0
endY=0
degree_list=[]
max_value=0
flag=0


def compute_ssim(img_mat_1, img_mat_2):
    # Variables for Gaussian kernel definition
    gaussian_kernel_sigma = 1.5
    gaussian_kernel_width = 11
    gaussian_kernel = numpy.zeros((gaussian_kernel_width, gaussian_kernel_width))

    # Fill Gaussian kernel
    for i in range(gaussian_kernel_width):
        for j in range(gaussian_kernel_width):
            gaussian_kernel[i, j] = \
                (1 / (2 * pi * (gaussian_kernel_sigma ** 2))) * \
                exp(-(((i - 5) ** 2) + ((j - 5) ** 2)) / (2 * (gaussian_kernel_sigma ** 2)))

    # Convert image matrices to double precision (like in the Matlab version)
    img_mat_1 = img_mat_1.astype(numpy.float)
    img_mat_2 = img_mat_2.astype(numpy.float)

    # Squares of input matrices
    img_mat_1_sq = img_mat_1 ** 2
    img_mat_2_sq = img_mat_2 ** 2
    img_mat_12 = img_mat_1 * img_mat_2

    # Means obtained by Gaussian filtering of inputs
    img_mat_mu_1 = scipy.ndimage.filters.convolve(img_mat_1, gaussian_kernel)
    img_mat_mu_2 = scipy.ndimage.filters.convolve(img_mat_2, gaussian_kernel)

    # Squares of means
    img_mat_mu_1_sq = img_mat_mu_1 ** 2
    img_mat_mu_2_sq = img_mat_mu_2 ** 2
    img_mat_mu_12 = img_mat_mu_1 * img_mat_mu_2

    # Variances obtained by Gaussian filtering of inputs' squares
    img_mat_sigma_1_sq = scipy.ndimage.filters.convolve(img_mat_1_sq, gaussian_kernel)
    img_mat_sigma_2_sq = scipy.ndimage.filters.convolve(img_mat_2_sq, gaussian_kernel)

    # Covariance
    img_mat_sigma_12 = scipy.ndimage.filters.convolve(img_mat_12, gaussian_kernel)

    # Centered squares of variances
    img_mat_sigma_1_sq = img_mat_sigma_1_sq - img_mat_mu_1_sq
    img_mat_sigma_2_sq = img_mat_sigma_2_sq - img_mat_mu_2_sq
    img_mat_sigma_12 = img_mat_sigma_12 - img_mat_mu_12;

    # c1/c2 constants
    # First use: manual fitting
    c_1 = 6.5025
    c_2 = 58.5225

    # Second use: change k1,k2 & c1,c2 depend on L (width of color map)
    l = 255
    k_1 = 0.01
    c_1 = (k_1 * l) ** 2
    k_2 = 0.03
    c_2 = (k_2 * l) ** 2

    # Numerator of SSIM
    num_ssim = (2 * img_mat_mu_12 + c_1) * (2 * img_mat_sigma_12 + c_2)
    # Denominator of SSIM
    den_ssim = (img_mat_mu_1_sq + img_mat_mu_2_sq + c_1) * \
               (img_mat_sigma_1_sq + img_mat_sigma_2_sq + c_2)
    # SSIM
    ssim_map = num_ssim / den_ssim
    index = numpy.average(ssim_map)

    print(index)

    return index


def aHash(gray):
    # 均值哈希算法
    # 缩放为8*8
    gray = cv2.resize(gray, (8, 8))

    # s为像素和初值为0，hash_str为hash值初值为''
    s = 0
    hash_str=0
    # 遍历累加求像素和
    for i in range(8):
        for j in range(8):
            s = s+gray[i, j]
    # 求平均灰度
    avg = s/64
    # 灰度大于平均值为1相反为0生成图片的hash值
    for i in range(8):
        for j in range(8):
            if gray[i, j] > avg:
                hash_str = hash_str+1
            else:
                hash_str = hash_str+0
    return hash_str


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

while True:
    ret, frame = cap.read()
    for template, w, h in templates:
        tes(frame, template, w, h, weight, height, bais)


    for i, degree in enumerate(degree_list):
        if max_value < degree:
            max_value = degree
            flag = i

    print(cls[flag], max_value)