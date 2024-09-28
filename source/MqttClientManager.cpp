#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "MQTTClient.h"

#include <vector>
#include <string>

using namespace std;

#include "MqttClientManager.h"

#define QOS         1

#define QOS_1       1
#define QOS_2       2

#define TIMEOUT     10000L



void onConnectionLost(void* context, char* cause)
{
    /*
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    printf("\nConnection lost\n");
    if (cause)
        printf("     cause: %s\n", cause);

    printf("Reconnecting\n");
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        finished = 1;
    }
    */
}


int onMessageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message)
{
    MqttClientManager * clientManager = (MqttClientManager *) context;

    clientManager->messageArrivedHandler(context, topicName, topicLen, message);


    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

void onDisconnectFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Disconnect failed, rc %d\n", response->code);
    //disc_finished = 1;
}

void onDisconnectSuccess(void* context, MQTTAsync_successData* response)
{
    printf("Successful disconnection\n");
    //disc_finished = 1;
}

void onSubscribeSuccess(void* context, MQTTAsync_successData* response)
{
    printf("\nSubscribe succeeded");
    //subscribed = 1;
}

void onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Subscribe failed, rc %d\n", response->code);
    //finished = 1;
}

void onUnSubscribeSuccess(void* context, MQTTAsync_successData* response)
{
    printf("\nUnsubscribe succeeded");
    //subscribed = 1;
}

void onUnSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    printf("Unsubscribe failed, rc %d\n", response->code);
    //finished = 1;
}


void onConnectFailure_internal(void* context, MQTTAsync_failureData* response)
{
    printf("Connect failed, rc %d\n", response->code);
    //finished = 1;
}


void onConnectSuccess_internal(void* context, MQTTAsync_successData* response)
{
    /*
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    printf("Successful connection\n");

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
        "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;
    if ((rc = MQTTAsync_subscribe(client, TOPIC, QOS, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start subscribe, return code %d\n", rc);
        finished = 1;
    }
    */
}

void onSendMessageFailure(void* context, MQTTAsync_failureData* response)
{
    /*
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    printf("Message send failed token %d error code %d\n", response->token, response->code);
    opts.onSuccess = onDisconnect;
    opts.onFailure = onDisconnectFailure;
    opts.context = client;
    if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    */
    MessageContext* messageContext = (MessageContext*)context;
    MqttClientManager* client = messageContext->mqttClient;
    if (client->onSendMessageFailureHandler != NULL) {
        client->onSendMessageFailureHandler(context, response);
    }
    delete messageContext->payload;
    delete[]messageContext->topicSent;
    delete messageContext;
}

void onSendMessageSuccess(void* context, MQTTAsync_successData* response)
{
    /*
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    printf("Message with token value %d delivery confirmed\n", response->token);
    opts.onSuccess = onDisconnect;
    opts.onFailure = onDisconnectFailure;
    opts.context = client;
    if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    */
   
    MessageContext* messageContext = (MessageContext *) context;
    MqttClientManager* client = messageContext->mqttClient;

    if (client->onSendMessageSuccessHandler != NULL) {
        client->onSendMessageSuccessHandler(context, response);
    }
    delete messageContext->payload;
    delete []messageContext->topicSent;
    delete messageContext;
}


int MqttClientManager::create(const char* clientID, const char* brokerURI) {
    int rc = EXIT_SUCCESS;
    if (this->isClientCreatedFlag == 0) {

        if ((rc = MQTTAsync_create(&this->mqttClient, brokerURI, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTASYNC_SUCCESS)
        {
            printf("Failed to create client object, return code %d\n", rc);
            return(EXIT_FAILURE);
        }
        strcpy(this->clientID, clientID);
        strcpy(this->brokerURI, brokerURI);
        this->isClientCreatedFlag = 1;
    }
    return rc;
}

int MqttClientManager::connect(MQTTAsync_onSuccess onConnectSuccessHandler, MQTTAsync_onFailure onConnectFailureHandler) {
	int rc = EXIT_FAILURE;

    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;


	//conn_opts.keepAliveInterval = 10; // keep connection for 10 seconds
	conn_opts.keepAliveInterval = 0;    // remain connected forever.
	conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnectSuccessHandler;
    conn_opts.onFailure = onConnectFailureHandler;
    conn_opts.context   = this;


    if (this->isClientCreatedFlag == 0) {
        printf("You must call the method create() before  calling method connect()");
        return EXIT_FAILURE;
    }
    
    if (isConnectionSuccessFlag == 0) {

        if ((rc = MQTTAsync_setCallbacks(this->mqttClient, this,
            onConnectionLost,
            onMessageArrived,
            NULL)) != MQTTASYNC_SUCCESS)
        {
            printf("Failed to set callbacks, return code %d\n", rc);
            
            rc = EXIT_FAILURE;
            return rc;
        }



        if ((rc = MQTTAsync_connect(this->mqttClient, &conn_opts)) != MQTTASYNC_SUCCESS)
        {
            printf("MQTT: Failed to connect, return code %d\n", rc);
            //exit(EXIT_FAILURE);
            return(EXIT_FAILURE);
        }
    }

	return(rc);
}


bool MqttClientManager::isConnected() {
    return MQTTAsync_isConnected(this->mqttClient);
}

int MqttClientManager::disconnect() {
    int rc= EXIT_SUCCESS;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;

    if (this->isConnectionSuccessFlag == 1) {
        disc_opts.onSuccess = onDisconnectSuccess;
        disc_opts.onFailure = onDisconnectFailure;

        if ((rc = MQTTAsync_disconnect(this->mqttClient, &disc_opts)) != MQTTASYNC_SUCCESS) {
            printf("MQTT: Failed to disconnect, return code %d\n", rc);
        }
        this->isConnectionSuccessFlag = 0;
    }
    return rc;
}

int MqttClientManager::destroy() {
    if (this->isClientCreatedFlag == 1) {
        MQTTAsync_destroy(&this->mqttClient);
        this->isClientCreatedFlag = 0;
    }
    return 1;
}


/*
int MqttClientManager::setWorkingCallbacks(void* context, MQTTAsync_connectionLost* cl, MQTTAsync_messageArrived* ma, MQTTAsync_deliveryComplete* dc) {
    this->context = context;
    this->connectionLostHandler = cl;
    this->messageArrivedHandler = ma;
    this->deliveryCompleteHandler = dc;
}
*/

int MqttClientManager::publish(const char* payload, const char* topic, int qos, MQTTAsync_onSuccess * onSendSuccess, MQTTAsync_onFailure *onSendFailure) {
    
    //MQTTClient_deliveryToken token;
    int rc = EXIT_FAILURE;
    /*
    pubmsg.payload = (void*)payload;
    pubmsg.payloadlen = (int)strlen(payload);
    pubmsg.qos = qos;
    pubmsg.retained = 0;
    if ((rc = MQTTAsync_sendMessage(this->mqttClient, topic, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
        printf("MQTT: Failed to publish message, return code %d\n", rc);
        //exit(EXIT_FAILURE);
        return(EXIT_FAILURE);
    }

    //printf("Waiting for up to %d seconds for publication of %s\n"
      //  "on topic %s for client with ClientID: %s\n",
      //  (int)(TIMEOUT / 1000), payload, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(this->mqttClient, token, TIMEOUT);
    //printf("Message with delivery token %d delivered\n", token);
    */

    
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    
    

    //printf("Successful connection\n");

    opts.onSuccess = onSendMessageSuccess;
    opts.onFailure = onSendMessageFailure;

    this->onSendMessageSuccessHandler = onSendSuccess;
    this->onSendMessageFailureHandler = onSendFailure;

    
    MessageContext *messageContext = new MessageContext(this, topic, payload, strlen(payload) );
    opts.context = messageContext;
    pubmsg.payload = (void *) payload;
    pubmsg.payloadlen = (int)strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    if ((rc = MQTTAsync_sendMessage(this->mqttClient, topic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        //exit(EXIT_FAILURE);
    }



    return rc;
}








// https://eclipse.github.io/paho.mqtt.c/MQTTClient/html/subasync.html
int MqttClientManager::subscribe(const char *topic,
    int qos,    MQTTAsync_connectionLost *connlost,    MQTTAsync_messageArrived *msgarrvd,    
    MQTTAsync_deliveryComplete * delivered    ) {

    int rc = EXIT_SUCCESS;

    this->connectionLostHandler     = connlost;
    this->messageArrivedHandler     = msgarrvd;
    this->deliveryCompleteHandler   = delivered;

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    opts.onSuccess = onSubscribeSuccess;
    opts.onFailure = onSubscribeFailure;
    opts.context = this;
    if ((rc = MQTTAsync_subscribe(this->mqttClient, topic, qos, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start subscribe, return code %d\n", rc);        
    }
    return rc;
}

int MqttClientManager::subscribe(std::vector<std::string> &topics,
    int qos, MQTTAsync_connectionLost* connlost, MQTTAsync_messageArrived* msgarrvd,
    MQTTAsync_deliveryComplete* delivered) {

    for (const std::string& topic : topics) {
        const char* topicCStr = topic.c_str();
        this->subscribe(topicCStr, qos, connlost, msgarrvd, delivered);
    }
    return 1;
}


int MqttClientManager::unsubscribe(const char* topic) {
    int rc= EXIT_SUCCESS;
    MQTTAsync_responseOptions responseOptions = MQTTAsync_responseOptions_initializer;
    responseOptions.onSuccess = onUnSubscribeSuccess;
    responseOptions.onFailure = onUnSubscribeFailure;

    if ((rc = MQTTAsync_unsubscribe(this->mqttClient, topic, &responseOptions)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to unsubscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }
    return(rc);
}




