# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import numpy as np
import cv2
from matplotlib import pyplot as plt

MIN_MATCH_COUNT = 2
left_x=10000
right_x=0
top_y=10000
bottom_y=0
img1 = cv2.imread('template2.jpg',0)          # queryImage
img2 = cv2.imread('pics/471F8FBB9872C2C60F753106CC48D04B.jpg',0) # trainImage
im3 = img2.copy()

# Initiate SIFT detector
sift = cv2.xfeatures2d.SIFT_create()

# find the keypoints and descriptors with SIFT
kp1, des1 = sift.detectAndCompute(img1,None)
kp2, des2 = sift.detectAndCompute(img2,None)

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

    h,w = img1.shape
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
    im3 = cv2.resize(im3, (500, 500))
    cv2.imshow("a", im3)


    img2 = cv2.polylines(img2,[np.int32(dst)],True,255,3, cv2.LINE_AA)


else:
    print("Not enough matches are found - %d/%d" % (len(good),MIN_MATCH_COUNT))
    matchesMask = None
draw_params = dict(matchColor = (0,255,0), # draw matches in green color
                   singlePointColor = None,
                   matchesMask = matchesMask, # draw only inliers
                   flags = 2)

img3 = cv2.drawMatches(img1,kp1,img2,kp2,good,None,**draw_params)
#plt.savefig("output_pendrive.png")
plt.imshow(img3, 'gray')
plt.show()
cv2.waitKey(0)