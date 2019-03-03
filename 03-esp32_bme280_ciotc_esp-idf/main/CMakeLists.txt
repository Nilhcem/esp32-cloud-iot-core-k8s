set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

register_component()

target_add_binary_data(${COMPONENT_TARGET} "certs/private_key.pem" TEXT)
