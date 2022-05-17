# python
# 陈涛
# 开发时间：$[DATE] $[TIME]
import cv2
cap = cv2.VideoCapture(0)
#ret,frame = cap.read()
ret = cap.isOpened()
fps = cap.get(5)/10000  #查询帧率
while ret:
    ret,frame = cap.read()
    tstep = cap.get(1)
    cv2.imshow("frame",frame)
    iloop=fps/2  #每秒处理2帧
    while iloop:
        cap.grab()  #只取帧不解码，
        iloop =iloop - 1
        if iloop < 1:
            break
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
cv2.destroyAllWindows()
cap.release()
