"""
@file LED_Controller.py
@brief Python script to communicate with an Arduino-powered NeoPixel over Wi-Fi.
       Sends commands to control the LED color.
@author Pi Ko (pi.ko@nyu.edu)
"""

import socket

# Configuration
ARDUINO_IP = "192.168.31.185"  # Replace with the Arduino's IP address
PORT = 80                    # Port the Arduino server is listening on

def log_debug(message):
    """Logs debug messages."""
    debug_mode = True
    if debug_mode:
        print(f"[DEBUG] {message}")

def send_command(command):
    """
    Sends a command to the Arduino server to control the NeoPixel.

    Args:
        command (str): Command to send ('R', 'G', 'B').
    """
    try:
        # Create a socket connection
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            log_debug(f"Connecting to {ARDUINO_IP}:{PORT}")
            client_socket.connect((ARDUINO_IP, PORT))
            log_debug(f"Sending command: {command}")
            client_socket.sendall(f"{command}\n".encode('utf-8'))c
            log_debug("Command sent successfully.")
    except Exception as e:
        log_debug(f"Error: {e}")

if __name__ == "__main__":
    print("Enter R, G, or B to control the NeoPixel. Type 'exit' to quit.")
    while True:
        user_input = input("Command (R/G/B): ").strip().upper()
        if user_input in ['R', 'G', 'B']:
            send_command(user_input)
        elif user_input == "EXIT":
            print("Exiting...")
            break
        else:
            print("Invalid command. Please enter R, G, or B.")
