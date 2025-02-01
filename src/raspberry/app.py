from flask import Flask, render_template, redirect, url_for, Response, jsonify
import cv2
import serial
import time
# import openmeteo_requests

import pandas as pd
from retry_requests import retry
import os  # Needed for file operations in capture endpoint
from datetime import datetime  # For date and time endpoints

app = Flask(__name__)

# Initialize serial connection to Arduino on COM12.
try:
    ser = serial.Serial('/dev/ttyUSB0', 250000, timeout=1)
    time.sleep(2)  # Allow the serial connection to settle.
    print("Serial connection established on COM12")
except Exception as e:
    print(f"Error connecting to Arduino on COM12: {e}")
    ser = None

# Initialize the webcam (0 is typically the default).
camera = cv2.VideoCapture(0)

def generate_frames():
    """Capture frames from the webcam and encode them as JPEG."""
    while True:
        success, frame = camera.read()
        if not success:
            break
        ret, buffer = cv2.imencode('.jpg', frame)
        frame = buffer.tobytes()
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/')
def index():
    """Render the main page."""
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    """Route that provides the video feed."""
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

# Original control endpoint (kept for compatibility; returns a redirect)
@app.route('/control/<direction>')
def control(direction):
    valid_directions = [
        "forward", "backward", "left", "right",
        "forward-left", "forward-right", "backward-left", "backward-right", "stop"
    ]
    if direction in valid_directions:
        print(f"{direction.capitalize()} button pressed!")
    else:
        print("Unknown command received:", direction)

    if ser and ser.is_open and direction in valid_directions:
        try:
            command = direction + "\n"  # Append newline.
            ser.write(command.encode())
            print(f"Sent command to Arduino: {command.strip()}")
            time.sleep(0.1)
            if ser.in_waiting > 0:
                response = ser.readline().decode().strip()
                print("Arduino response:", response)
            else:
                print("No response from Arduino.")
        except Exception as e:
            print(f"Failed to send command '{direction}': {e}")
    else:
        if not ser or not ser.is_open:
            print("Serial connection not available.")

    return redirect(url_for('index'))

# New endpoint for AJAX control commands (used for continuous sending)
@app.route('/control_command/<direction>', methods=['GET'])
def control_command(direction):
    valid_directions = [
        "forward", "backward", "left", "right",
        "forward-left", "forward-right", "backward-left", "backward-right", "stop"
    ]
    if direction in valid_directions:
        print(f"{direction.capitalize()} command received via AJAX!")
    else:
        print("Unknown command received:", direction)

    if ser and ser.is_open and direction in valid_directions:
        try:
            command = direction + "\n"
            ser.write(command.encode())
            print(f"Sent command to Arduino: {command.strip()}")
            time.sleep(0.1)
            if ser.in_waiting > 0:
                response = ser.readline().decode().strip()
                print("Arduino response:", response)
            else:
                print("No response from Arduino.")
        except Exception as e:
            print(f"Failed to send command '{direction}': {e}")
    else:
        if not ser or not ser.is_open:
            print("Serial connection not available.")

    return "OK"

@app.route('/capture', methods=['GET'])
def capture():
    """Capture a screenshot from the webcam and save it."""
    success, frame = camera.read()
    if not success:
        return "Failed to capture screenshot", 500

    # Ensure the screenshots directory exists inside the static folder
    screenshot_dir = os.path.join(app.static_folder, 'screenshots')
    if not os.path.exists(screenshot_dir):
        os.makedirs(screenshot_dir)

    # Generate a filename based on timestamp
    timestamp = int(time.time())
    filename = f'screenshot_{timestamp}.jpg'
    file_path = os.path.join(screenshot_dir, filename)

    # Save the image
    cv2.imwrite(file_path, frame)
    print(f"Screenshot saved: {file_path}")

    # Return a link to the saved screenshot
    return f"Screenshot saved as <a href='/static/screenshots/{filename}' target='_blank'>{filename}</a>"

# ================= Weather API Code (COMMENTED OUT) =================
'''
# Setup the Open-Meteo API client with cache and retry on error
cache_session = requests_cache.CachedSession('.cache', expire_after=3600)
retry_session = retry(cache_session, retries=5, backoff_factor=0.2)
openmeteo = openmeteo_requests.Client(session=retry_session)

# Define the Open-Meteo API URL and parameters
url = "https://api.open-meteo.com/v1/forecast"
params = {
    "latitude": 45.5053313,
    "longitude": -73.6163859,
    "current": [
        "temperature_2m", "relative_humidity_2m", "is_day",
        "precipitation", "rain", "showers", "snowfall",
        "wind_speed_10m", "wind_direction_10m", "wind_gusts_10m"
    ],
    "timezone": "America/New_York",
    "forecast_days": 1
}

@app.route('/weather_data')
def weather_data():
    """
    Calls the Open-Meteo API and returns current weather data as JSON.
    (Cached calls will update automatically after 1 hour.)
    """
    responses = openmeteo.weather_api(url, params=params)
    response_weather = responses[0]
    current = response_weather.Current()

    # Extract and round the values to the nearest whole number.
    weather = {
        "temperature": round(current.Variables(0).Value()),
        "relative_humidity": round(current.Variables(1).Value()),
        "precipitation": round(current.Variables(3).Value()),
        "wind_speed": round(current.Variables(7).Value())
    }
    return jsonify(weather)
'''
# =====================================================

# ================= Date & Time Endpoints =================
@app.route('/date_today')
def date_today():
    """Return the current date (YYYY-MM-DD). This value should update once a day."""
    today = datetime.now().strftime("%Y-%m-%d")
    return jsonify({"date": today})

@app.route('/time_now')
def time_now():
    """Return the current time (HH:MM:SS). This value updates every second."""
    now = datetime.now().strftime("%H:%M:%S")
    return jsonify({"time": now})
# =====================================================

if __name__ == '__main__':
    # Disable the reloader to prevent the serial port from opening twice.
    app.run(host='0.0.0.0', port=5000, debug=True, use_reloader=False)
