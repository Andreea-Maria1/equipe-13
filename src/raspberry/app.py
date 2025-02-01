from flask import Flask, render_template, redirect, url_for, Response, jsonify, request  
import cv2
import serial
import time
import os
from datetime import datetime
import requests
from dotenv import load_dotenv
import io  # For in‑memory binary streams
import csv

load_dotenv()  # Load environment variables from the .env file

app = Flask(__name__)

# ===================== LOAD INVASIVE CSV =====================
# Assume your CSV is located at: <project_root>/data/invasive.csv
# CSV content example:
# Fallopia japonica,Oui
# Heracleum mantegazzianum,Oui
# Ailanthus altissima,Oui
# Pueraria montana,Oui
# Impatiens glandulifera,Non

invasive_species = {}
csv_path = os.path.join(os.path.dirname(__file__), 'data', 'invasive.csv')
if os.path.exists(csv_path):
    with open(csv_path, newline='', encoding='utf-8') as f:
        reader = csv.reader(f)
        for row in reader:
            if len(row) >= 2:
                species = row[0].strip().lower()  # Make key lowercase for case-insensitive matching
                status = row[1].strip().lower()   # Expected "oui" or "non"
                invasive_species[species] = status
else:
    print("CSV file not found:", csv_path)

# ===================== PLANT.ID API CONFIGURATION =====================
API_KEY = os.getenv("API_KEY")
API_KEY_INVASIVE = os.getenv("API_KEY_INVASIVE")
PLANT_ID_URL = "https://api.plant.id/v2/identify"
PLANT_ID_URL_INVASIVE = "https://api.plant.id/v2/check"

# ===================== SERIAL & CAMERA INITIALIZATION =====================
# Update the serial port for your system. On Linux (e.g., Raspberry Pi), use "/dev/ttyUSB0"
try:
    print("Attempting to open serial port /dev/ttyUSB0")
    ser = serial.Serial('/dev/ttyUSB0', 250000, timeout=1)
    time.sleep(2)  # Allow the serial connection to settle.
    print("Serial connection established on /dev/ttyUSB0")
except Exception as e:
    print(f"Error connecting to Arduino on /dev/ttyUSB0: {e}")
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

# ===================== MAIN INTERFACE ROUTES =====================
@app.route('/')
def index():
    """Render the main page with video stream, controls, and plant identification."""
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    """Route that provides the video feed from the webcam."""
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/control/<direction>')
def control(direction):
    """Control endpoint for redirect-based commands (for legacy support)."""
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

@app.route('/control_command/<direction>', methods=['GET'])
def control_command(direction):
    """AJAX endpoint for continuous control commands."""
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

# ===================== DATE & TIME ENDPOINTS =====================
@app.route('/date_today')
def date_today():
    """Return the current date (YYYY-MM-DD)."""
    today = datetime.now().strftime("%Y-%m-%d")
    return jsonify({"date": today})

@app.route('/time_now')
def time_now():
    """Return the current time (HH:MM:SS)."""
    now = datetime.now().strftime("%H:%M:%S")
    return jsonify({"time": now})

# ===================== PLANT IDENTIFICATION ENDPOINT =====================
@app.route('/identify', methods=['GET'])
def identify():
    """
    Capture an image from the webcam, send it to the Plant.id APIs,
    and return the identification result as JSON.
    """
    success, frame = camera.read()
    if not success:
        return jsonify({'error': 'Failed to capture image.'})
    
    # Encode frame as JPEG
    ret, buffer = cv2.imencode('.jpg', frame)
    if not ret:
        return jsonify({'error': 'Failed to encode image.'})
    
    image_bytes = buffer.tobytes()
    # Create a file‑like object from the image bytes
    image_file = io.BytesIO(image_bytes)
    image_file.name = 'screenshot.jpg'
    
    # Call the primary Plant.id API
    response = requests.post(
        PLANT_ID_URL,
        files={"images": image_file},
        data={"api_key": API_KEY, "organs": "leaf"}
    )
    
    if response.status_code == 200:
        data = response.json()
        if data.get("suggestions"):
            suggestion = data["suggestions"][0]
        else:
            return jsonify({'error': 'No plant identified.'})
    else:
        return jsonify({'error': 'Error connecting to Plant.id API.'})
    
    # Use our CSV to determine invasiveness, case-insensitively.
    plant_name = suggestion.get("plant_name", "Unknown")
    plant_name_lower = plant_name.strip().lower()
    if plant_name_lower in invasive_species:
        # "oui" indicates invasive; "non" indicates not invasive.
        invasiveness = "Invasive" if invasive_species[plant_name_lower] == "oui" else "Not invasive"
    else:
        invasiveness = "Unknown"
    
    # Build the result without including toxicity information.
    result = {
        "plant_name": plant_name,
        "invasiveness": invasiveness
    }
    return jsonify(result)

# ===================== OPTIONAL: Dummy Weather Data Endpoint =====================
@app.route('/weather_data')
def weather_data():
    # Dummy weather data for testing purposes
    dummy_weather = {
        "temperature": -15,
        "relative_humidity": 67,
        "precipitation": 0,
        "wind_speed": 8
    }
    return jsonify(dummy_weather)

# ===================== RUN APP =====================
if __name__ == '__main__':
    # Disable the reloader to prevent the serial port from opening twice.
    app.run(host='0.0.0.0', port=5000, debug=True, use_reloader=False)
