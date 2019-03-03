# ESP32 BME280 Google Cloud IoT Core (Arduino)

ESP32 Arduino code that sends BME280 sensor data to Google Cloud IoT Core

## Dependencies

You'll need to install from the Arduino IDE those dependencies:

- BME280:
  - **Adafruit BME280 Library** by Adafruit ([more info](https://github.com/adafruit/Adafruit_BME280_Library))
  - **Adafruit Unified Sensor** by Adafruit ([more info](https://github.com/adafruit/Adafruit_Sensor))
- Cloud IoT Core:
  - **Google Cloud IoT Core JWT** by Vladimir Korukov ([more info](https://github.com/GoogleCloudPlatform/google-cloud-iot-arduino))
  - **rBase64** by Abhijit Bose ([more info](https://github.com/boseji/rBASE64))
- MQTT bridge:
  - **MQTT** by Joel Gaehwiler ([more info](https://github.com/256dpi/arduino-mqtt))


## Setup Cloud IoT Core

Install the [gcloud](https://cloud.google.com/sdk/gcloud/) command-line tool, and run the following:

```sh
export PROJECT_ID="<project-id>"
export PUBSUB_TOPIC="iotcore-topic"
export PUBSUB_SUBSCRIPTION="iotcore-subscription"
export REGISTRY_NAME="iotcore-registry"
export DEVICE_NAME="esp32"

# Choose your region: https://cloud.google.com/compute/docs/regions-zones/
# Valid cloud regions are {asia-east1,europe-west1,us-central1}
export REGION_NAME="us-central1"

# Authorize gcloud to access the Cloud Platform with Google user credentials
gcloud auth login

# Create a new project
gcloud projects create $PROJECT_ID

# Set default project
gcloud config set project $PROJECT_ID

# Enable Cloud Pub/Sub
gcloud services enable pubsub.googleapis.com

# Enable billing (required for Cloud IoT. You can also use the `gcloud beta billing` CLI)
open https://console.cloud.google.com/iot/api?project=$PROJECT_ID

# Enable Cloud IoT
gcloud services enable cloudiot.googleapis.com

# Give permission to Cloud IoT Core to publish messages on Cloud Pub/Sub
gcloud projects add-iam-policy-binding $PROJECT_ID --member=serviceAccount:cloud-iot@system.gserviceaccount.com --role=roles/pubsub.publisher

# Create Cloud Pub/Sub topic for event notifications
gcloud pubsub topics create $PUBSUB_TOPIC

# Create Cloud Pub/Sub subscription
gcloud pubsub subscriptions create $PUBSUB_SUBSCRIPTION --topic $PUBSUB_TOPIC

# Create Cloud IoT registry specifying Cloud Pub/Sub topic name
gcloud iot registries create $REGISTRY_NAME \
  --region=$REGION_NAME \
  --event-notification-config=topic=$PUBSUB_TOPIC \
  --enable-mqtt-config --enable-http-config

# Generate an Elliptic Curve (EC) ES256 private / public key pair
openssl ecparam -genkey -name prime256v1 -noout -out ec_private.pem
openssl ec -in ec_private.pem -pubout -out ec_public.pem

# Create a new Cloud IoT device
gcloud iot devices create $DEVICE_NAME --region=$REGION_NAME --registry=$REGISTRY_NAME --public-key="path=./ec_public.pem,type=es256"
```


## ciotc_config.h

- Update the `ciotc_config.h` files with your Wi-Fi and Cloud IoT details.  
- Set the `private_key_str` constant using data from the following command:
```sh
$ openssl ec -in ec_private.pem -noout -text
```


## Wiring

The BME280 sensor should be connected to the ESP32 over I2C.  
If the BME280 I2C address is not `0x76`, you'll need to modify the `BME280_ADDRESS` constant.  
Depending on your board, you'll also need to modify the SDA and SCL pin names in the `Wire.begin(SDA, SCL);` call.

**Wemos Lolin D32 Pro**
- SDA: `21`
- SCL: `22`

**Wemos Lolin32 Lite**
- SDA: `15`
- SCL: `2`

**DOIT Esp32 DevKit v1**
- SDA: `21`
- SCL: `22`


## Verify that it works

After flashing the ESP32, you can verify that it sends data to Google Cloud, by pulling Cloud/PubSub messages from the subscription:
```sh
$ gcloud pubsub subscriptions pull --auto-ack $PUBSUB_SUBSCRIPTION --limit=1
```
