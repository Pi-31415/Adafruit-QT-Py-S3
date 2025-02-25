"""
@file Communicator.py
@brief Python script acting as a UDP bridge between Unity and an Arduino-powered device controlling motion commands.
       The script listens for UDP messages from Unity and translates them into numeric commands that are sent via TCP
       to the Arduino. It also retains the ability to accept numeric commands via console input.

Command mapping:
    'UP' or '1'    : Up Pulse (activates Motor 1) -> sends '1'
    'RIGHT' or '2' : Right Pulse (activates Motor 2) -> sends '2'
    'LEFT' or '3'  : Left Pulse (activates Motor 3) -> sends '3'
    'DOWN' or '4'  : Down Pulse (activates Motor 4) -> sends '4'
    'CW' or '5'    : Clockwise sequence (Motor 1 → Motor 2 → Motor 4 → Motor 3) -> sends '5'
    'CCW' or '6'   : Counterclockwise sequence (Motor 1 → Motor 3 → Motor 4 → Motor 2) -> sends '6'
    Any other command cancels any active motion (sends '0').

@author:
    Pi Ko (pi.ko@nyu.edu)
"""

import socket
import threading

# Hardcoded configuration for the Arduino device.
ARDUINO_IP = "192.168.31.185"  # Arduino's IP address.
ARDUINO_PORT = 80             # Port on which the Arduino server is listening.

# UDP configuration for receiving messages from Unity.
UDP_IP = "0.0.0.0"            # Listen on all available network interfaces.
UDP_PORT = 5005               # UDP port for receiving commands from Unity.

def log_debug(message):
    """
    Logs a debug message to the console.

    Args:
        message (str): The debug message to log.
    """
    debug_mode = True  # Set to True to enable debug logging.
    if debug_mode:
        print(f"[DEBUG] {message}")

def send_command(command):
    """
    Sends a numeric command to the Arduino via a TCP connection.

    The command is sent as a single character string. Valid numeric commands are:
        '1' : Up Pulse (Motor 1)
        '2' : Right Pulse (Motor 2)
        '3' : Left Pulse (Motor 3)
        '4' : Down Pulse (Motor 4)
        '5' : Clockwise sequence (Motor 1 → Motor 2 → Motor 4 → Motor 3)
        '6' : Counterclockwise sequence (Motor 1 → Motor 3 → Motor 4 → Motor 2)
        '0' : Cancel/stop any active motion.

    Args:
        command (str): The command character to send.
    """
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            log_debug(f"Connecting to {ARDUINO_IP}:{ARDUINO_PORT}")
            client_socket.connect((ARDUINO_IP, ARDUINO_PORT))
            log_debug(f"Sending command: {command}")
            client_socket.sendall(f"{command}\n".encode('utf-8'))
            log_debug("Command sent successfully.")
    except Exception as e:
        log_debug(f"Error sending command: {e}")

def translate_command(cmd):
    """
    Translates a human-readable command into its corresponding numeric command.

    Supported translations:
        'UP'    -> '1'
        'RIGHT' -> '2'
        'LEFT'  -> '3'
        'DOWN'  -> '4'
        'CW'    -> '5'
        'CCW'   -> '6'

    If the command is already numeric ('1'-'6'), it is returned unchanged.
    Any other input results in a cancellation command ('0').

    Args:
        cmd (str): The command string to translate.

    Returns:
        str: The corresponding numeric command.
    """
    mapping = {
        "UP": "1",
        "RIGHT": "2",
        "LEFT": "3",
        "DOWN": "4",
        "CW": "5",
        "CCW": "6"
    }
    cmd_upper = cmd.strip().upper()
    if cmd_upper in mapping:
        return mapping[cmd_upper]
    elif cmd_upper in ['1', '2', '3', '4', '5', '6']:
        return cmd_upper
    else:
        # For invalid commands, return a cancellation command.
        return "0"

def udp_listener():
    """
    Listens for UDP messages from Unity.

    Upon receiving a UDP message, the command is translated to a numeric command and sent to the Arduino.
    """
    udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_sock.bind((UDP_IP, UDP_PORT))
    log_debug(f"UDP listener started on {UDP_IP}:{UDP_PORT}")
    while True:
        try:
            data, addr = udp_sock.recvfrom(1024)
            cmd_str = data.decode('utf-8').strip()
            log_debug(f"Received UDP command from {addr}: {cmd_str}")
            numeric_cmd = translate_command(cmd_str)
            log_debug(f"Translated command: {numeric_cmd}")
            send_command(numeric_cmd)
        except Exception as e:
            log_debug(f"Error in UDP listener: {e}")

def console_input():
    """
    Reads commands from console input and sends them to the Arduino.

    Accepts commands in either human-readable format (e.g., 'UP', 'CW') or numeric format ('1'-'6').
    """
    print("Motion Controller (Console Mode) - Enter a command to test motion functions.")
    print("Commands: UP, RIGHT, LEFT, DOWN, CW, CCW (or numeric 1-6). Type 'exit' to quit.")
    while True:
        user_input = input("Command: ").strip()
        if user_input.lower() == 'exit':
            print("Exiting Motion Controller...")
            break
        numeric_cmd = translate_command(user_input)
        if numeric_cmd:
            send_command(numeric_cmd)
        else:
            print("Invalid command. Please try again.")

if __name__ == "__main__":
    # Start the UDP listener in a separate daemon thread.
    udp_thread = threading.Thread(target=udp_listener, daemon=True)
    udp_thread.start()

    # Also allow console input for testing.
    console_input()
