import cv2
import imutils
from skimage.metrics import structural_similarity
import time
from skimage import filters, img_as_ubyte
import numpy as np


def cmpHash(hash1, hash2):
    # Hash值对比
    # 算法中1和0顺序组合起来的即是图片的指纹hash。顺序不固定，但是比较的时候必须是相同的顺序。
    # 对比两幅图的指纹，计算汉明距离，即两个64位的hash值有多少是不一样的，不同的位数越小，图片越相似
    # 汉明距离：一组二进制数据变成另一组数据所需要的步骤，可以衡量两图的差异，汉明距离越小，则相似度越高。汉明距离为0，即两张图片完全一样
    n = 0
    # hash长度不同则返回-1代表传参出错
    if len(hash1) != len(hash2):
        return -1
    # 遍历判断
    for i in range(len(hash1)):
        # 不相等则n计数+1，n最终为相似度
        if hash1[i] != hash2[i]:
            n = n + 1
    return n

def pHash(img):
    # 感知哈希算法
    # 缩放32*32
    img = cv2.resize(img, (32, 32))  # , interpolation=cv2.INTER_CUBIC

    # 转换为灰度图
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # 将灰度图转为浮点型，再进行dct变换
    dct = cv2.dct(np.float32(gray))
    # opencv实现的掩码操作
    dct_roi = dct[0:8, 0:8]

    hash = []
    avreage = np.mean(dct_roi)
    for i in range(dct_roi.shape[0]):
        for j in range(dct_roi.shape[1]):
            if dct_roi[i, j] > avreage:
                hash.append(1)
            else:
                hash.append(0)
    return hash

start = time.time()
# 读入图像，转为灰度图像
src = cv2.imread('pics/B84FE410243A39E3905AACA78ECEA92C.jpg')
img = cv2.imread('pics/2976851C10E0E681B33B533EE3C1D592.jpg')
a = pHash(src)
b = pHash(img)
n = cmpHash(a, b)
print(n)
grayA = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)
grayB = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)



# 计算两个灰度图像之间的结构相似度
(score, diff) = structural_similarity(grayA, grayB, win_size=101, full=True)
diff = (diff * 255).astype("uint8")
cv2.namedWindow("diff", cv2.WINDOW_NORMAL)
cv2.imshow("diff", diff)
print("SSIM:{}".format(score))

# 找到不同的轮廓以致于可以在表示为 '不同'的区域放置矩形
# 全局自适应阈值分割（二值化），返回值有两个，第一个是阈值，第二个是二值图像
dst = cv2.threshold(diff, 0, 255, cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)[1]
cv2.namedWindow("threshold", cv2.WINDOW_NORMAL)
cv2.imshow('threshold', dst)
# findContours找轮廓，返回值有两个，第一个是轮廓信息，第二个是轮廓的层次信息（“树”状拓扑结构）
# cv2.RETR_EXTERNAL：只检测最外层轮廓
# cv2.CHAIN_APPROX_SIMPLE：压缩水平方向、垂直方向和对角线方向的元素，保留该方向的终点坐标，如矩形的轮廓可用4个角点表示
contours, hierarchy = cv2.findContours(dst.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
print(contours)
newimg = np.zeros(dst.shape, np.uint8)  # 定义一个和图像分割处理后相同大小的黑色图
# drawContours画轮廓，将找到的轮廓信息画出来
cv2.drawContours(newimg, contours, -1, (255, 255, 255), 1)
cv2.namedWindow("contours", cv2.WINDOW_NORMAL)
cv2.imshow('contours', newimg)
# cnts = cnts[0] if imutils.is_cv3() else cnts[0]    取findContours函数的第一个返回值，即取轮廓信息

# 找到一系列区域，在区域周围放置矩形
for c in contours:
    (x, y, w, h) = cv2.boundingRect(c)  # boundingRect函数：计算轮廓的垂直边界最小矩形，矩形是与图像上下边界平行的
    cv2.rectangle(src, (x, y), (x + w, y + h), (0, 255, 0), 2)  # rectangle函数：使用对角线的两点pt1，pt2画一个矩形轮廓
    cv2.rectangle(img, (x, y), (x + w, y + h), (0, 255, 0), 2)  # 画矩形的图, pt1, pt2,(对角线两点的坐标), 矩形边框的颜色，矩形边框的粗细

end = time.time()
print(end - start)
# 用cv2.imshow 展现最终对比之后的图片
cv2.namedWindow("right", cv2.WINDOW_NORMAL)
cv2.imshow('right', src)
cv2.namedWindow("left", cv2.WINDOW_NORMAL)
cv2.imshow('left', img)
cv2.waitKey(0)
cv2.destroyAllWindows()
