# arduino-azure-iot-crash

## Not working code!

Continuously calling `IoTHubClient_LL_DoWork()` when network is unavailable crashes Arduino, probably due to accessing an invalid pointer.

Logs at point of crash:

```
Error: Time:Tue Jan 28 11:30:55 2020 File:/Users/tom.davis/Documents/Arduino/libraries/AzureIoTHub/src/iothubtransport_mqtt_common.c Func:InitializeConnection Line:2385 mqtt_client timed out waiting for CONNACK
Info: --> DisconnectFromClient
Info: --> xio_destroy
Info: --> tlsio_arduino_destroy
Error: Time:Tue Jan 28 11:30:55 2020 File:/Users/tom.davis/Documents/Arduino/libraries/AzureIoTUtility/src/adapters/tlsio_arduino.c Func:tlsio_arduino_destroy Line:198 tlsio_arduino_destroy called while not in TLSIO_STATE_CLOSED.
Info: --> internal_close
Info: --> process_and_destroy_head_message
Info: <-- process_and_destroy_head_message
Info: <-- internal_close
Info: about to call STRING_delete(tls_io_instance->hostname)...
Info: --> STRING_delete
Info: address of STRING* is 53197
Info: about to access value->s
```

### Dependencies
```
arduino-cli lib install WiFi101
arduino-cli lib install AzureIoTHub
arduino-cli lib install AzureIoTUtility
arduino-cli lib install AzureIoTProtocol_MQTT
arduino-cli lib install RTCZero
arduino-cli lib install NTPClient
```

### Build
```
arduino-cli compile --fqbn adafruit:samd:adafruit_feather_m0 -v app
```

### Deploy
```
arduino-cli upload -p /dev/cu.usbmodem14301 --fqbn adafruit:samd:adafruit_feather_m0 app
```

### Monitor Serial
```
screen /dev/cu.usbmodem14301 115200
```
