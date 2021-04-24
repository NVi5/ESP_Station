idf_component_register( SRCS "sensor.c" "utils.c" "user_i2c.c" "bme280.c" "http.c" "ota.c" "wifi.c" "main.c"
                        INCLUDE_DIRS "."
                        EMBED_TXTFILES "cert.pem")

if (DEFINED SSID)
    add_definitions(-DSSID="${SSID}")
else()
    add_definitions(-DSSID="")
endif()

if (DEFINED PASS)
    add_definitions(-DPASS="${PASS}")
else()
    add_definitions(-DPASS="")
endif()

if (DEFINED BUILD_VERSION)
    add_definitions(-DBUILD_VERSION=${BUILD_VERSION})
else()
    add_definitions(-DBUILD_VERSION=0)
endif()