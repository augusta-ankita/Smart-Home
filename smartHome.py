import serial
import time
import random

# Change to your actual Arduino COM port
arduino = serial.Serial(port='COM3', baudrate=9600, timeout=1)
time.sleep(2)

rooms = ["LIVING", "BEDROOM", "KITCHEN"]
room_devices = ["LIGHT", "FAN", "DOOR", "TV", "AC", "HEATER", "WINDOWBLINDS"]
global_devices = ["WATERMOTOR", "ALARMBUZZER"]
sensor_commands = ["FIRE_DETECTED", "CLEAR_FIRE", "WATER_TANK_FULL", "RESET_TANK"]

def send_command(cmd):
    arduino.write((cmd + '\n').encode())
    time.sleep(0.5)
    while arduino.in_waiting:
        print("Arduino:", arduino.readline().decode().strip())

def simulate_presence():
    return {room: random.choice([True, False]) for room in rooms}

def auto_mode():
    print("🔄 Auto mode every 7 seconds. Ctrl+C to stop.")
    try:
        while True:
            presence = simulate_presence()
            anyone_home = False

            for room in rooms:
                if presence[room]:
                    anyone_home = True
                    print(f"👤 Presence in {room} → Turning ON room devices")
                    for device in room_devices:
                        send_command(f"TURN_ON {room} {device}")
                else:
                    print(f"🚫 No presence in {room} → Turning OFF room devices")
                    for device in room_devices:
                        send_command(f"TURN_OFF {room} {device}")

            # Water motor logic only (alarm handled by Arduino)
            if anyone_home:
                send_command("TURN_ON GLOBAL WATERMOTOR")
            else:
                send_command("TURN_OFF GLOBAL WATERMOTOR")

            time.sleep(7)
    except KeyboardInterrupt:
        print("🔕 Auto mode stopped.")

def print_menu():
    print("\n=== Smart Home Controller ===")
    print("Manual Control:")
    print("  TURN_ON <ROOM or GLOBAL> <DEVICE>")
    print("  TURN_OFF <ROOM or GLOBAL> <DEVICE>")
    print("  STATUS")
    print("  AUTO")
    print("  EXIT")
    print("Simulated Sensor Commands:")
    print("  FIRE_DETECTED   → Simulate fire (alarm ON)")
    print("  CLEAR_FIRE      → Reset fire (alarm OFF)")
    print("  WATER_TANK_FULL → Tank full (motor OFF)")
    print("  RESET_TANK      → Reset tank status")

def main():
    print_menu()

    while True:
        cmd = input("Enter command: ").strip().upper()

        if cmd == "EXIT":
            break
        elif cmd == "STATUS":
            send_command("STATUS")
        elif cmd == "AUTO":
            auto_mode()
        elif cmd in sensor_commands:
            send_command(cmd)
        elif cmd.startswith("TURN_ON") or cmd.startswith("TURN_OFF"):
            parts = cmd.split()
            if len(parts) != 3:
                print("❌ Invalid format. Use: TURN_ON <ROOM> <DEVICE>")
                continue

            action, room, device = parts
            if room not in rooms and room != "GLOBAL":
                print(f"❌ Invalid room: {room}. Allowed: {rooms} or GLOBAL")
                continue

            send_command(cmd)
        else:
            print("❌ Invalid command. Try again.")
            print_menu()

    arduino.close()

if __name__ == "__main__":
    main()
