# python
# 陈涛
# 开发时间：$[DATE] $[TIM#Import necessary libraries
from flask import Flask, render_template, Response
import cv2
#Initialize the Flask app
from gevent import pywsgi

app = Flask(__name__)

cap = cv2.VideoCapture(0)

def gen_frames():
    while True:
        ret, frame = cap.read()
        frame = cv2.resize(frame, (200, 200))
        if not ret:
            break
        else:
            ret, buffer = cv2.imencode(".jpg", frame)
            frame = buffer.tobytes()
            yield (b'--frame\r\n'b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == "__main__":
    server = pywsgi.WSGIServer(('192.168.43.219', 5000), app)
    server.serve_forever()