// This file contains your configuration used to connect to Cloud IoT Core

// Wifi network details.
const char *ssid = "<your-ssid>";
const char *password = "<your-password>";

// Cloud IoT details.
const char *project_id = "<your-project-id>";
const char *location = "<region-name>";
const char *registry_id = "iotcore-registry";
const char *device_id = "<device-name>";

// To get the private key run (where private-key.pem is the ec private key
// used to create the certificate uploaded to google cloud iot):
// openssl ec -in <private-key.pem> -noout -text
// and copy priv: part.
// The key length should be exactly the same as the key length bellow (32 pairs
// of hex digits). If it's bigger and it starts with "00:" delete the "00:". If
// it's smaller add "00:" to the start. If it's too big or too small something
// is probably wrong with your key.
const char *private_key_str =
    "63:5b:61:a5:e7:7d:46:72:76:ce:56:3f:6f:1c:54:"
    "80:cd:f9:d2:e3:2a:ca:cf:03:a8:71:d7:56:61:dc:"
    "a8:13";
