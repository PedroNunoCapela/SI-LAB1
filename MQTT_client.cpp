#include <stdio.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>
#include<vector>
#include<string>
#include<thread>

#include <interface.h>
#include<warehouseWebServer.h>
#include<localControl.h>

#include <MqttClientManager.h>

#include <MQTT_client.h>

extern bool MqttSystemIsRunning;

#define MQTT_BROKER_URL  (const char *) "tcp://localhost:1883"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include<chrono>
#include<thread>
bool MqttSystemIsRunning = true;


void onMqttActuatorsConnectFailure(void* context, MQTTAsync_failureData* response)
{
	printf("Mqtt Actuators connect FAILURE, rc = %d\n", response->code);

}

void onMqttActuatorsConnectionLost(void* context, char* cause) {
	printf("\nConnection lost\n");
	printf("     cause: %s\n", cause);
}

int onMqttActuatorsMessageArrived(void* context, char* topicName, int messageLen, MQTTAsync_message* message)
{
	char* payload = (char*)message->payload;

	if (strcmp(topicName, "menu_keyboard") == 0) {
		// mosquitto_pub -h localhost -p 1883 -t "menu_keyboard" -m "a"
		int key_command = payload[0];  // consider only the first character
		executeLocalControl(key_command);
	}
	else if (strcmp(topicName, "actuator") == 0) {
		try {
			json jsonMessage = json::parse(payload);

			std::string name = jsonMessage["name"];
			std::string value = jsonMessage["value"];

			printf("\nParsed JSON - name: %s, value: %s", name.c_str(), value.c_str());

			// Control logic for the actuator motor_x
			if (name == "motor_x") { // the class string has got operator ==
				int direction = std::stoi(value);  // Convert value to an integer
				if (direction == 0) { stopX(); } // if 
				if (direction == 1) { moveXRight(); } // else if
				if (direction == -1) { moveXLeft(); } // else if
			}
			// Add more actuator controls here as needed...

			// Control logic for the actuator motor_y
			if (name == "motor_y") { // the class string has got operator ==
				int direction = std::stoi(value);  // Convert value to an integer
				if (direction == 0) { stopY(); } // if 
				if (direction == 1) { moveYInside(); } // else if
				if (direction == -1) { moveYOutside(); } // else if
			}

			// Control logic for the actuator motor_z
			if (name == "motor_z") { // the class string has got operator ==
				int direction = std::stoi(value);  // Convert value to an integer
				if (direction == 0) { stopZ(); } // if 
				if (direction == 1) { moveZUp(); } // else if
				if (direction == -1) { moveZDown(); } // else if
			}

			// Control logic for the actuator left_station
			if (name == "left_station") { // the class string has got operator ==
				int direction = std::stoi(value);  // Convert value to an integer
				if (direction == 0) { stopLeftStation(); } // if 
				if (direction == 1) { moveLeftStationInside(); } // else if
				if (direction == -1) { moveLeftStationOutside(); } // else if
			}

			// Control logic for the actuator right_station
			if (name == "right_station") { // the class string has got operator ==
				int direction = std::stoi(value);  // Convert value to an integer
				if (direction == 0) { stopRightStation(); } // if 
				if (direction == 1) { moveRightStationInside(); } // else if
				if (direction == -1) { moveRightStationOutside(); } // else if
			}
		}
		catch (json::exception& e) {
			// Catch any errors during JSON parsing
			printf("\nError parsing JSON: %s", e.what());
		}
	}
	return 1;
}

void onMqttActuatorsConnectSuccess(void* context, MQTTAsync_successData* response)
{
	printf("Mqtt Actuators connect SUCCESS\n");
	MqttClientManager* client = (MqttClientManager*)context;
	std::vector<std::string> topics = { "menu_keyboard", "actuator" };
	client->subscribe(topics, QOS_1, onMqttActuatorsConnectionLost, onMqttActuatorsMessageArrived, NULL);
}

void startMQTTActuatorsOperation() {
	// the static specifier below is to avoid the variable 
	// being declared in the stack, which is volatile. 
	static MqttClientManager mqttActuators;
	// it can be any unique identifier
	mqttActuators.create("actuators_client_id_1", MQTT_BROKER_URL);
	mqttActuators.connect(onMqttActuatorsConnectSuccess, onMqttActuatorsConnectFailure);
}

void mqttMonitorXAxis(MqttClientManager& mqttClientManager) {
	static auto lastTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

	static int previous_x_position = -1, previous_x_moving = -9999;
	int xPosition = getXPosition();
	int xMoving = getXDirection();

	char message[128] = "";
	// publish the x position info when: the position has changed, the movement has changed, and every 10 seconds.
	if ((xPosition != previous_x_position) || (xMoving != previous_x_moving) || (duration > 10000)) {
		sprintf(message, "{\"name\": \"x_is_at\", \"value\": \"%d\"}", xPosition);
		previous_x_position = xPosition;
		lastTime = currentTime;
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		sprintf(message, "{\"name\": \"x_direction\", \"value\": \"%d\"}", xMoving);
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		previous_x_moving = xMoving;
	}
}

// Monitor Y axis
void monitorYAxis(MqttClientManager& mqttClientManager) {
	static auto lastTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

	static int previous_y_position = -1, previous_y_moving = -9999;
	int yPosition = getYPosition();
	int yMoving = getYDirection();

	char message[128] = "";
	// publish the y position info when: the position has changed, the movement has changed, and every 10 seconds.
	if ((yPosition != previous_y_position) || (yMoving != previous_y_moving) || (duration > 10000)) {
		sprintf(message, "{\"name\": \"y_is_at\", \"value\": \"%d\"}", yPosition);
		previous_y_position = yPosition;
		lastTime = currentTime;
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		sprintf(message, "{\"name\": \"y_direction\", \"value\": \"%d\"}", yMoving);
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		previous_y_moving = yMoving;
	}
}

// Monitor Z axis
void monitorZAxis(MqttClientManager& mqttClientManager) {
	static auto lastTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

	static int previous_z_position = -1, previous_z_moving = -9999;
	int zPosition = getZPosition();
	int zMoving = getZDirection();

	char message[128] = "";
	// publish the z position info when: the position has changed, the movement has changed, and every 10 seconds.
	if ((zPosition != previous_z_position) || (zMoving != previous_z_moving) || (duration > 10000)) {
		sprintf(message, "{\"name\": \"z_is_at\", \"value\": \"%d\"}", zPosition);
		previous_z_position = zPosition;
		lastTime = currentTime;
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		sprintf(message, "{\"name\": \"z_direction\", \"value\": \"%d\"}", zMoving);
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		previous_z_moving = zMoving;
	}
}

// monitor left station
void monitorLeftStation(MqttClientManager& mqttClientManager) {
	static auto lastTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

	static int previous_ls_moving = -9999;
	int lsMoving = getLeftStationDirection();

	char message[128] = "";
	// publish the station state when the movement has changed, and every 10 seconds.
	if ((lsMoving != previous_ls_moving) || (duration > 10000)) {
		lastTime = currentTime;
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		sprintf(message, "{\"name\": \"ls_direction\", \"value\": \"%d\"}", lsMoving);
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		previous_ls_moving = lsMoving;
	}
}
// monitor right station
void monitorRightStation(MqttClientManager& mqttClientManager) {
	static auto lastTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

	static int previous_rs_moving = -9999;
	int rsMoving = getRightStationDirection();

	char message[128] = "";
	// publish the station state when the movement has changed, and every 10 seconds.
	if ((rsMoving != previous_rs_moving) || (duration > 10000)) {
		lastTime = currentTime;
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		sprintf(message, "{\"name\": \"rs_direction\", \"value\": \"%d\"}", rsMoving);
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		previous_rs_moving = rsMoving;
	}
}
// monitor cage
void monitorCage(MqttClientManager& mqttClientManager) {
	static auto lastTime = std::chrono::system_clock::now();
	auto currentTime = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

	bool cageState = isPartInCage();

	char message[128] = "";
	// publish the cage state position info every 10 seconds.
	if (duration > 10000) {
		lastTime = currentTime;
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
		sprintf(message, "{\"name\": \"cage_state\", \"value\": \"%d\"}", cageState);
		mqttClientManager.publish(message, "sensor", QOS_1, NULL, NULL);
	}
}
void startMqttSensorsOperation() {
	std::thread t([]() {
		printf("\nmonitoring sensors started...");
		MqttClientManager mqttMonitoring;
		mqttMonitoring.create("sensors_client_id_1", MQTT_BROKER_URL);
		mqttMonitoring.connect(NULL, NULL);

		// here, instead of doing this code after success connection handle 
		// we wait till successful connection
		// the program does not freeze here, because this is a new thread.
		printf("\nWaiting for mqtt monitoring_client_1 connection...");
		while (mqttMonitoring.isConnected() == false) {
			putchar('.');
			Sleep(1000);
		}
		while (MqttSystemIsRunning) {
			mqttMonitorXAxis(mqttMonitoring);
			/*monitorYAxis(mqttMonitoring);
			monitorZAxis(mqttMonitoring);*/
			monitorLeftStation(mqttMonitoring);
			monitorRightStation(mqttMonitoring);
			monitorCage(mqttMonitoring);
		}
		mqttMonitoring.disconnect();
		printf("\nmonitoring finishing...");
		});
	t.detach();
}