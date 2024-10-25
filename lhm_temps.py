import requests
import time
import serial

# URL for Libre Hardware Monitor's web server
url = "http://localhost:8085/data.json"

# Establish a serial connection with Arduino
arduino_port = 'COM4'  # Adjust based on your system
baud_rate = 9600

# Start the serial connection
arduino = serial.Serial(arduino_port, baud_rate, timeout=1)
time.sleep(5)  # Allow time for the Arduino to reset

try:
    arduino.write("START\n".encode('utf-8'))
    while True:
        # Get the JSON data from Libre Hardware Monitor
        print("\033c", end="")  # For Windows/Linux to clear console
        try:
            response = requests.get(url)
            if response.status_code == 200:
                json_data = response.json()

                # Extract CPU and GPU temperatures (assuming values are like "46.0 °C")
                cpu_temp_str = json_data['Children'][0]['Children'][0]['Children'][3]['Children'][6]['Value']
                gpu_temp_str = json_data['Children'][0]['Children'][1]['Children'][1]['Children'][0]['Value']

                # Remove the "°C" and convert to integers (assuming temperature is float with "°C" unit)
                cpu_temp = int(float(cpu_temp_str.replace("°C", "").strip()))
                gpu_temp = int(float(gpu_temp_str.replace("°C", "").strip()))

                # Create the data to send
                data_to_send = "{},{}\n".format(cpu_temp, gpu_temp)

                # Send data over serial
                arduino.write(data_to_send.encode('utf-8'))

                print(f"CPU Temperature: {cpu_temp}°C, GPU Temperature: {gpu_temp}°C")

            else:
                raise Exception("No data from monitor")
        
        except requests.exceptions.JSONDecodeError:
            print("Error: Unable to decode JSON response.")
        
        except Exception as e:
            print(f"An error occurred: {e}")

        time.sleep(0.5)

except KeyboardInterrupt:
    print("Monitoring stopped.")
finally:
    arduino.close()
