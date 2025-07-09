struct DeviceState {
  bool light = false;
  bool fan = false;
  bool door = false;
  bool tv = false;
  bool ac = false;
  bool heater = false;
  bool windowBlinds = false;
};

DeviceState living, bedroom, kitchen;

// Global Devices
bool waterMotor = false;
bool alarmBuzzer = false;

// Simulated Sensors
bool fireDetected = false;
bool tankFull = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Smart Home Ready");
}

void setDevice(String room, String device, bool state) {
  DeviceState* target = nullptr;

  // Handle global devices first
  if (device == "WATERMOTOR") {
    waterMotor = state;
  } else if (device == "ALARMBUZZER") {
    alarmBuzzer = state;
  } else {
    // Room validation
    if (room == "LIVING") target = &living;
    else if (room == "BEDROOM") target = &bedroom;
    else if (room == "KITCHEN") target = &kitchen;
    else {
      Serial.println("Invalid room");
      return;
    }

    // Room device setting
    if (device == "LIGHT") target->light = state;
    else if (device == "FAN") target->fan = state;
    else if (device == "DOOR") target->door = state;
    else if (device == "TV") target->tv = state;
    else if (device == "AC") target->ac = state;
    else if (device == "HEATER") target->heater = state;
    else if (device == "WINDOWBLINDS") target->windowBlinds = state;
    else {
      Serial.println("Invalid device");
      return;
    }
  }

  Serial.print(device);
  Serial.print((device == "WATERMOTOR" || device == "ALARMBUZZER") ? "successfully" : " in " + room);
  Serial.println(state ? " turned ON" : " turned OFF");
}

void handleSensors() {
  if (fireDetected) {
    if (!alarmBuzzer) {
      alarmBuzzer = true;
      Serial.println("🔥 Fire detected! ALARMBUZZER turned ON");
    }
  }

  if (tankFull && waterMotor) {
    waterMotor = false;
    Serial.println("💧 Water tank full! WATERMOTOR turned OFF automatically");
  }
}

void printRoomStatus(String name, DeviceState room) {
  Serial.print(name); Serial.print(" → ");
  Serial.print("LIGHT: "); Serial.print(room.light);
  Serial.print(" FAN: "); Serial.print(room.fan);
  Serial.print(" DOOR: "); Serial.print(room.door);
  Serial.print(" TV: "); Serial.print(room.tv);
  Serial.print(" AC: "); Serial.print(room.ac);
  Serial.print(" HEATER: "); Serial.print(room.heater);
  Serial.print(" WINDOWBLINDS: "); Serial.println(room.windowBlinds);
}

void printStatus() {
  Serial.println("=== DEVICE STATUS ===");
  printRoomStatus("LIVING", living);
  printRoomStatus("BEDROOM", bedroom);
  printRoomStatus("KITCHEN", kitchen);

  Serial.print("GLOBAL → WATERMOTOR: "); Serial.print(waterMotor);
  Serial.print(" ALARMBUZZER: "); Serial.println(alarmBuzzer);

  Serial.print("Sensors → FIRE: "); Serial.print(fireDetected);
  Serial.print(" TANK_FULL: "); Serial.println(tankFull);
}

void loop() {
  handleSensors();

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "STATUS") {
      printStatus();
    } else if (input == "FIRE_DETECTED") {
      fireDetected = true;
      Serial.println("🔥 Simulated fire detection activated!");
    } else if (input == "CLEAR_FIRE") {
      fireDetected = false;
      alarmBuzzer = false;
      Serial.println("🧯 Fire cleared. ALARMBUZZER turned OFF.");
    } else if (input == "WATER_TANK_FULL") {
      tankFull = true;
      Serial.println("💧 Simulated: Water tank full.");
    } else if (input == "RESET_TANK") {
      tankFull = false;
      Serial.println("🔄 Water tank reset. Motor can be turned ON again.");
    } else if (input.startsWith("TURN_ON") || input.startsWith("TURN_OFF")) {
      bool state = input.startsWith("TURN_ON");
      input.remove(0, state ? 8 : 9);
      int spaceIndex = input.indexOf(' ');
      if (spaceIndex == -1) {
        Serial.println("Invalid format");
        return;
      }
      String room = input.substring(0, spaceIndex);
      String device = input.substring(spaceIndex + 1);
      setDevice(room, device, state);
    } else {
      Serial.println("Unknown command");
    }
  }
}
