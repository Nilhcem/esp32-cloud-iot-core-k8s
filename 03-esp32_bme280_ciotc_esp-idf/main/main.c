#include <iotc.h>
#include <iotc_jwt.h>
#include <stdio.h>
#include <string.h>

#include "./init_time.h"
#include "./init_wifi.h"
#include "./init_bme280.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEVICE_PATH "projects/%s/locations/%s/registries/%s/devices/%s"
#define PUBLISH_TOPIC_EVENT "/devices/%s/events"

extern const uint8_t ec_pv_key_start[] asm("_binary_private_key_pem_start");

static const char *TAG = "APP";

static iotc_timed_task_handle_t delayed_publish_task = IOTC_INVALID_TIMED_TASK_HANDLE;
iotc_context_handle_t iotc_context = IOTC_INVALID_CONTEXT_HANDLE;
struct bme280_data bme280_data;


int generate_json(char **json)
{
  int rslt = get_bme280_data(&bme280_data);

  if (rslt)
  {
    asprintf(json, "{\"timestamp\":%u, \"temperature\":%.2f, \"humidity\":%.2f}", get_timestamp(), bme280_data.temperature, bme280_data.humidity);
  }
  return rslt;
}

void publish_telemetry_event(iotc_context_handle_t context_handle,
                             iotc_timed_task_handle_t timed_task, void *user_data)
{
  char *publish_topic = NULL;
  asprintf(&publish_topic, PUBLISH_TOPIC_EVENT, CONFIG_GIOT_DEVICE_ID);

  char *publish_message = NULL;
  if (generate_json(&publish_message))
  {
    ESP_LOGI(TAG, "publishing msg \"%s\" to topic: \"%s\"\n", publish_message, publish_topic);
    iotc_publish(context_handle, publish_topic, publish_message, IOTC_MQTT_QOS_AT_LEAST_ONCE, /*callback=*/NULL, /*user_data=*/NULL);
    free(publish_message);
  }
  free(publish_topic);
}

void on_connection_state_changed(iotc_context_handle_t in_context_handle,
                                 void *data, iotc_state_t state)
{
  iotc_connection_data_t *conn_data = (iotc_connection_data_t *)data;

  switch (conn_data->connection_state) {
  case IOTC_CONNECTION_STATE_OPENED:
    printf("connected!\n");
    // Create a timed task to publish every 10 seconds.
    delayed_publish_task = iotc_schedule_timed_task(in_context_handle, publish_telemetry_event, 10, 15, /*user_data=*/NULL);
    break;

  case IOTC_CONNECTION_STATE_OPEN_FAILED:
    printf("ERROR!\tConnection has failed reason %d\n\n", state);
    // Exit it out of the application by stopping the event loop.
    iotc_events_stop();
    break;

  case IOTC_CONNECTION_STATE_CLOSED:
    if (IOTC_INVALID_TIMED_TASK_HANDLE != delayed_publish_task) {
      iotc_cancel_timed_task(delayed_publish_task);
      delayed_publish_task = IOTC_INVALID_TIMED_TASK_HANDLE;
    }

    if (state == IOTC_STATE_OK) {
      iotc_events_stop();
    } else {
      printf("connection closed - reason %d!\n", state);
      iotc_connect(in_context_handle, conn_data->username, conn_data->password, conn_data->client_id, conn_data->connection_timeout, conn_data->keepalive_timeout, &on_connection_state_changed);
    }
    break;

  default:
    printf("wrong value\n");
    break;
  }
}

void mqtt_task(void *pvParameters)
{
  iotc_crypto_key_data_t iotc_connect_private_key_data;
  iotc_connect_private_key_data.crypto_key_signature_algorithm = IOTC_CRYPTO_KEY_SIGNATURE_ALGORITHM_ES256;
  iotc_connect_private_key_data.crypto_key_union_type = IOTC_CRYPTO_KEY_UNION_TYPE_PEM;
  iotc_connect_private_key_data.crypto_key_union.key_pem.key = (char *) ec_pv_key_start;

  // Initialize iotc library and create a context to use to connect to the GCP IoT Core Service.
  const iotc_state_t error_init = iotc_initialize();

  if (IOTC_STATE_OK != error_init) {
    printf(" iotc failed to initialize, error: %d\n", error_init);
    vTaskDelete(NULL);
  }

  // Create a connection context. A context represents a Connection on a single socket, and can be used to publish and subscribe to numerous topics.
  iotc_context = iotc_create_context();
  if (IOTC_INVALID_CONTEXT_HANDLE >= iotc_context) {
    printf(" iotc failed to create context, error: %d\n", -iotc_context);
    vTaskDelete(NULL);
  }

  // Generate the client authentication JWT, which will serve as the MQTT password.
  char jwt[IOTC_JWT_SIZE] = {0};
  size_t bytes_written = 0;
  iotc_state_t state = iotc_create_iotcore_jwt(CONFIG_GIOT_PROJECT_ID, /*jwt_expiration_period_sec=*/3600, &iotc_connect_private_key_data, jwt, IOTC_JWT_SIZE, &bytes_written);

  if (IOTC_STATE_OK != state) {
    printf("iotc_create_iotcore_jwt returned with error: %ul", state);
    vTaskDelete(NULL);
  }

  char *device_path = NULL;
  asprintf(&device_path, DEVICE_PATH, CONFIG_GIOT_PROJECT_ID, CONFIG_GIOT_LOCATION, CONFIG_GIOT_REGISTRY_ID, CONFIG_GIOT_DEVICE_ID);
  iotc_connect(iotc_context, NULL, jwt, device_path, /*connection_timeout=*/0, /*keepalive_timeout=*/20, &on_connection_state_changed);
  free(device_path);

  iotc_events_process_blocking();
  iotc_delete_context(iotc_context);
  iotc_shutdown();

  vTaskDelete(NULL);
}

void app_main()
{
  if (!init_bme280())
  {
    printf("Error initializing BME280");
  }
  init_wifi();
  init_time();

  xTaskCreate(&mqtt_task, "mqtt_task", 8192, NULL, 5, NULL);
}
