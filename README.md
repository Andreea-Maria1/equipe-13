# Rover Control & Plant Identifier Web App

This project is a Flask-based web application integrated into an autonomous rover. The rover is equipped with multidirectional wheels, a camera (with an infrared sensor), and sensors that allow it to balance when pushed and avoid collisions when obstacles are detected. The web app provides a live video stream, Arduino-based directional controls, screenshot capture, and plant identification using the Plant.id API along with a CSV-based invasive species lookup.

## Features

- **Rover Platform:**  
  The system is implemented on a rover that:
  - Uses multidirectional wheels for versatile movement.
  - Employs an infrared sensor-equipped camera for enhanced vision.
  - Maintains balance when pushed.
  - Avoids collisions using sensor data.

- **Live Video Streaming & Camera Controls:**  
  The `/video_feed` route streams live video from the rover’s camera.

- **Arduino Serial Control:**  
  Control buttons send directional commands (forward, backward, left, right, diagonals, and stop) to the Arduino controlling the rover's wheels.

- **Collision Avoidance & Self-Balancing:**  
  The rover uses sensor data to detect obstacles and maintain balance, ensuring safe operation in dynamic environments.

- **Screenshot Capture:**  
  Capture and save screenshots from the live video stream. Screenshots are stored in the `static/screenshots` directory.

- **Plant Identification:**  
  Capture an image from the camera and send it to the Plant.id API to identify a plant. A CSV file (`data/invasive.csv`) is used to determine, in a case‑insensitive manner, whether the identified plant species is invasive.

- **Date, Time & Weather Data:**  
  API endpoints provide the current date (`/date_today`), time (`/time_now`), and dummy weather data (`/weather_data`) that are displayed on the app's top banner.

## Prerequisites

- **Python 3.6+**  
- **Flask**  
- **OpenCV (cv2)**  
- **pySerial**  
- **python-dotenv**  
- **Requests**

Install the dependencies using `pip` (preferably within a virtual environment):

```bash
pip install flask opencv-python pyserial python-dotenv requests
```

## Project Structure

A suggested directory structure for this project is:

```
project_root/
├── app.py                   # Main Flask application
├── data/
│   └── invasive.csv         # CSV file with invasive species data
├── static/
│   ├── screenshots/         # Folder to store captured screenshots
│   └── styles.css           # CSS file for styling the HTML interface
├── templates/
│   └── index.html           # Main HTML template
├── .env                     # Environment variables (API keys, etc.)
└── README.md                # This file
```

## Environment Variables

Create a `.env` file in the root of your project with the following content (replace with your actual API keys):

```
API_KEY=your_plant_id_api_key
API_KEY_INVASIVE=your_plant_id_invasive_api_key
```

## CSV Data

Place your CSV file (`invasive.csv`) in the `data` folder. An example CSV content:

```
Fallopia japonica,Oui
Heracleum mantegazzianum,Oui
Ailanthus altissima,Oui
Pueraria montana,Oui
Impatiens glandulifera,Non
```

This CSV is loaded at startup and used to determine if an identified plant species is invasive by performing a case-insensitive lookup.

## Arduino Serial Connection

Update the serial port configuration in `app.py` based on your system:
- On Windows, you might use `"COM12"`.
- On Linux or macOS, update accordingly (e.g., `"/dev/ttyUSB0"`).

*Note:* The sample code uses a placeholder (`'ttyUSB0'`) which you should adjust as needed.

## Running the Application

Start the application with:

```bash
python app.py
```

The Flask development server will launch (by default on port 5000). Access the web app via your browser at:

```
http://localhost:5000
```

## Usage

- **Live Video & Control Panel:**  
  The main interface displays a live video feed, directional control buttons to operate the rover, and a screenshot capture button.

- **Plant Identification:**  
  Clicking the "Identify Plant" button captures an image from the rover’s camera, sends it to the Plant.id API, and displays the identified plant name along with its invasiveness status (based on the CSV lookup).

- **Additional Information:**  
  The top banner displays the current date, time, and dummy weather data.

- **Rover Behavior:**  
  The rover’s built-in sensors ensure it avoids collisions and maintains balance when disturbed.

## Troubleshooting

- **Serial Connection:**  
  Verify that your Arduino is connected to the correct port and that you have appropriate permissions to access the port.

- **CSV File:**  
  Ensure the `data/invasive.csv` file exists and is correctly formatted.

- **Camera & Sensor Issues:**  
  Confirm that the camera is correctly connected and recognized by the system. Check any additional sensor configuration if the rover does not balance or avoid collisions as expected.

## Contributing

Contributions are welcome! Please fork the repository and submit pull requests. For major changes, please open an issue first to discuss what you would like to modify.

## License

This project is licensed under the MIT License.