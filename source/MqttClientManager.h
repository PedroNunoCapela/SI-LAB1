#pragma once
//#include "MQTTClient.h"


#include <MQTTAsync.h>


#include <vector>
#include <string>

//#define ADDRESS     "tcp://mqtt.eclipseprojects.io:1883"
//#define ADDRESS     "tcp://localhost:1883"
//#define CLIENTID    "SI_2023_p187"  // it can be an id specificed with your concatenated student numbers
//#define TOPIC       "SI"
//#define PAYLOAD     "Hello World!"
#define QOS         1

#define QOS_1       1
#define QOS_2       2

#define TIMEOUT     10000L



class MqttClientManager {
public:
	MQTTAsync  mqttClient	= NULL;
	bool isClientCreatedFlag	    = 0;
	bool isConnectionSuccessFlag = 0;

	char clientID[128]		= "";
	char brokerURI[255] = "";

	// working callbacks
	MQTTAsync_connectionLost*	connectionLostHandler	= NULL; 
	MQTTAsync_messageArrived*	messageArrivedHandler	= NULL; 
	MQTTAsync_deliveryComplete* deliveryCompleteHandler = NULL;

	// connectin callbacks
	MQTTAsync_onSuccess* onConnectionSuccess = NULL;
	MQTTAsync_onFailure* onConnectionFailure = NULL;

	//disconnection callbacks


	// on send messages callbackes
	MQTTAsync_onSuccess* onSendMessageSuccessHandler = NULL;
	MQTTAsync_onFailure* onSendMessageFailureHandler = NULL;
	
	void* context = NULL;
		

	MqttClientManager() {		
	}

	int create(const char* clientID, const char* brokerAddress);
	int connect(MQTTAsync_onSuccess onSuccessHandler, MQTTAsync_onFailure onFailureHandler);
	int disconnect();
	int destroy();
	bool isConnected();
	//int setWorkingCallbacks(void *context, MQTTAsync_connectionLost* cl, MQTTAsync_messageArrived* ma, MQTTAsync_deliveryComplete* dc);



	
	int publish(const char* payload, const char* topic, int qos, MQTTAsync_onSuccess* onSendSuccess, MQTTAsync_onFailure* onSendFailure);
	int subscribe(const char* topic,
		int qos,
		MQTTAsync_connectionLost* connlost,
		MQTTAsync_messageArrived* msgarrvd,
		MQTTAsync_deliveryComplete* delivered);

	int subscribe(std::vector<std::string>& topics,
		int qos, MQTTAsync_connectionLost* connlost, MQTTAsync_messageArrived* msgarrvd,
		MQTTAsync_deliveryComplete* delivered);

	int unsubscribe(const char* topic);


	


};

class MessageContext {
public:
	MqttClientManager* mqttClient;
	char* topicSent = NULL;
	char* payload = NULL;
	size_t payloadSize = 0; 
	MessageContext(MqttClientManager* mqttClient, const char *topicSent, const char *payload, size_t payloadSize) {
		this->mqttClient = mqttClient;
		this->payload = new char[1 + payloadSize];
		this->payloadSize = payloadSize; 
		strcpy(this->payload, payload);
		size_t size = (topicSent != NULL) ? strlen(topicSent) : 1;
		this->topicSent = new char[1 + size]; // creates at least 1 if topic is ""
		this->topicSent[0] = '\0';
		if (topicSent != NULL) {
			strcpy(this->topicSent, topicSent);
		}
	}
};

