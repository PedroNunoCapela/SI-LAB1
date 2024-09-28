#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <interface.h>
#include <warehouseWebServer.h>
#include <niDAQWebInterface.h>
#include <localControl.h>
#include <MQTT_client.h>

int main() {
	printf("Welcome to Intelligent Supervision\n");
	printf("Press any key to start\n");

	configure_simulator_server();
	start_mg_server();
	initializeHardwarePorts();

	startMQTTActuatorsOperation();
	startMqttSensorsOperation();

	int keyboard = 0;

	showLocalMenu();

	while (keyboard != 27) {  // 27 is the ASCII code for ESC
		if (_kbhit()) {
			keyboard = _getch();
			executeLocalControl(keyboard);
		}
		else {
			keyboard = 0;
		}
		Sleep(1);
	}

	stop_mg_server();

	return 0;
}
