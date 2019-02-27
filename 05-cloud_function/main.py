import base64
import json
from datetime import datetime

from influxdb import InfluxDBClient


def iotcore_pubsub_to_influxdb(data, context):
    if data['data'] is None or data['attributes'] is None or data['attributes']['deviceId'] is None:
        print('Invalid data received: {}'.format(data))
        return

    influxdb_client = _get_influxdb_client()
    device_id = data['attributes']['deviceId']
    json_data = base64.b64decode(data['data']).decode('utf-8')
    iot_data = json.loads(json_data)
    time_str = datetime.utcfromtimestamp(iot_data['timestamp']).strftime('%Y-%m-%dT%H:%M:%SZ')

    for measurement in ['temperature', 'humidity']:
        if measurement in iot_data:
            _send_sensor_data_to_influxdb(influxdb_client, device_id, time_str, measurement, iot_data[measurement])


def _get_influxdb_client():
    host = '<host>'
    port = 8086
    username = '<username>'
    password = '<password>'
    database = 'iot'
    return InfluxDBClient(host, port, username, password, database)


def _send_sensor_data_to_influxdb(client, device_id, time_str, measurement, value):
    json_body = [
        {
            'measurement': measurement,
            'tags': {
                'device': device_id
            },
            'time': time_str,
            'fields': {
                'value': float(value)
            }
        }
    ]
    client.write_points(json_body)
