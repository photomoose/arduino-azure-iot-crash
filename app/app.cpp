#include <Arduino.h>
#include <WiFi101.h>
#include <AzureIoTUtility.h>
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <sys/time.h>
#include "Config.h"

bool connectWifi();
bool pollNTP();
bool initializeIoTClient();
void pollIoT();
void connectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void *userContextCallback);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_POOL, NTP_OFFSET);
IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;

extern "C" char *sbrk(int i);

int getFreeMemory()
{
    char stack_dummy = 0;
    return &stack_dummy - sbrk(0);
}

void setup() {

    Serial.begin(115200);
    WiFi.setPins(8, 7, 4, 2);

    while (!Serial) { }

    while (!connectWifi()) {
        delay(1000);
    }

    while (!pollNTP()) {
        delay(1000);
    }

    initializeIoTClient();

    // Disconnect from Wifi to simulate network outage
    WiFi.end();
}


void loop() {
    LogInfo("Free memory: %d", getFreeMemory());
    pollIoT();

    delay(100);
}


bool connectWifi() {

    LogInfo("Connecting to %s...", WIFI_SSID);

    if (WiFi.begin(WIFI_SSID, WIFI_PASSWORD) != WL_CONNECTED) {
        LogError("Failed to connect to %s", WIFI_SSID);
        return false;
    }

    LogInfo("Connected to %s", WIFI_SSID);

    return true;
}

bool pollNTP() {
    LogInfo("Requesting time from %s...", NTP_POOL);

    if (!timeClient.update()) {
        LogInfo("Failed to receive time");
        timeClient.end();
        return false;
    }

    struct timeval tv;
    tv.tv_sec = timeClient.getEpochTime();
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);

    LogInfo("Received time: %s", timeClient.getFormattedTime().c_str());

    return true;
}

bool initializeIoTClient() {
    LogInfo("Initializing IOT Client...");

    if (platform_init() != 0) {
        LogError("Failed to initialize platform");
        return false;
    }

    if (serializer_init(NULL) != SERIALIZER_OK) {
        LogError("Failed to initialize serializer");
        return false;
    }

    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(IOT_CONNECTION_STRING, MQTT_Protocol);

    if (iotHubClientHandle == NULL) {
        LogError("Failed to create IOTHUB_CLIENT_LL_HANDLE");
        return false;
    }

    bool traceEnabled = IOT_TRACE_ENABLED;
    if (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &traceEnabled) != IOTHUB_CLIENT_OK) {
        LogError("Failed to set option OPTION_LOG_TRACE");
        return false;
    }

    bool rawTraceEnabled = IOT_RAW_TRACE_ENABLED;
    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "rawlogtrace", &rawTraceEnabled) != IOTHUB_CLIENT_OK) {
        LogError("Failed to set option rawlogtrace");
        return false;
    }

    size_t tokenLifetime = 30;
    if (IoTHubClient_LL_SetOption(iotHubClientHandle, OPTION_SAS_TOKEN_LIFETIME, &tokenLifetime) != IOTHUB_CLIENT_OK) {
        LogError("Failed to set option OPTION_SAS_TOKEN_LIFETIME");
        return false;
    }

    if (IoTHubClient_LL_SetConnectionStatusCallback(iotHubClientHandle, connectionStatusCallback, NULL) != IOTHUB_CLIENT_OK) {
        LogError("Failed to set connection status callback");
        return false;
    }

    if (IoTHubClient_LL_SetRetryPolicy(iotHubClientHandle, IOTHUB_CLIENT_RETRY_INTERVAL, 0) != IOTHUB_CLIENT_OK) {
        LogError("Failed to set retry policy");
        return false;
    }

    LogInfo("Initializing IOT Client complete");

    return true;
}

void pollIoT() {
    LogInfo("Polling IOT Hub...");

    IoTHubClient_LL_DoWork(iotHubClientHandle);

    LogInfo("Polling IOT Hub complete");
}

void connectionStatusCallback(IOTHUB_CLIENT_CONNECTION_STATUS result, IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason, void *userContextCallback)
{
    LogInfo("Connection Status: %s | Reason: %s", MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS, result), MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONNECTION_STATUS_REASON, reason));
}


