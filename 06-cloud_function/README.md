# Google Cloud Function

## Prerequisites

```sh
# Enable Cloud Functions API
gcloud services enable cloudfunctions.googleapis.com
```


## Update influxDB variables

Open `main.py` and modify InfluxDB variables (host, port, username, password) in the `_get_influxdb_client` function


## Deploy the cloud function

```sh
export PUBSUB_TOPIC="<your-topic-name>"
export REGION="us-central1" # https://cloud.google.com/functions/docs/locations
gcloud functions deploy iotcore_pubsub_to_influxdb --runtime python37 --trigger-topic $PUBSUB_TOPIC --region $REGION
```


## Test, triggering the function manually and checking logs

```sh
gcloud pubsub topics publish $PUBSUB_TOPIC --message "{\"timestamp\":$(date +%s),\"temperature\":24.48,\"humidity\":37.72}" --attribute "deviceId=test"
gcloud functions logs read --limit 50 --region $REGION
```
