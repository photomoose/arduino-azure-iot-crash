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
