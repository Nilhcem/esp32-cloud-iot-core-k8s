#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_EMBED_TXTFILES := certs/private_key.pem

# Print an error if the certificate/key files are missing
$(COMPONENT_PATH)/certs/private_key.pem:
	@echo "Missing PEM file $@. This file identifies the ESP32 to Google Cloud IoT Core for the example, see README for details."
	exit 1
