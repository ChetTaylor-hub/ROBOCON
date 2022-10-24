# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import cv2 as cv
import numpy as np
from matplotlib import pyplot as plt

cap = cv.VideoCapture(1)
def video():
    success, frame = cap.read()
    img = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    return img
img = cv.imread('1.jpg',0)
img2 = img.copy()
template = cv.imread('IMG_20220312_214912.jpg',0)
w, h = template.shape[::-1]
# All the 6 methods for comparison in a list
methods = ['cv.TM_CCOEFF']
for meth in methods:
    while True:
        img = img2.copy()
        img = video()
        method = eval(meth)
        # Apply template Matching
        res = cv.matchTemplate(img,template,method)
        min_val, max_val, min_loc, max_loc = cv.minMaxLoc(res)
        # If the method is TM_SQDIFF or TM_SQDIFF_NORMED, take minimum
        if method in [cv.TM_SQDIFF, cv.TM_SQDIFF_NORMED]:
            top_left = min_loc
        else:
            top_left = max_loc
        bottom_right = (top_left[0] + w, top_left[1] + h)
        print(top_left)
        cv.rectangle(img,top_left, bottom_right, 255, 2)
        # 按任意键退出
        cv.waitKey(0)
        cv.destroyAllWindows()
        plt.subplot(121),plt.imshow(res,cmap = 'gray')
        plt.title('Matching Result'), plt.xticks([]), plt.yticks([])
        plt.subplot(122),plt.imshow(img,cmap = 'gray')
        plt.title('Detected Point'), plt.xticks([]), plt.yticks([])
        plt.suptitle(meth)
        plt.show()
