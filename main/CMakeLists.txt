idf_component_register(SRCS "blink.c"
                    INCLUDE_DIRS ".")

add_definitions(-DSSID="${SSID}")
add_definitions(-DPASS="${PASS}")