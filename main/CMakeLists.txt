idf_component_register( SRCS "wifi_functions.c" "main.c"
                        INCLUDE_DIRS "."
                        EMBED_TXTFILES "cert.pem")

if (DEFINED ${SSID})
    add_definitions(-DSSID="${SSID}")
else()
    add_definitions(-DSSID="[308] Jan")
endif()

if (DEFINED ${PASS})
    add_definitions(-DPASS="${PASS}")
else()
    add_definitions(-DPASS="9a126d53f6")
endif()

if (DEFINED ${BUILD_VERSION})
    add_definitions(-DBUILD_VERSION=${BUILD_VERSION})
else()
    add_definitions(-DBUILD_VERSION=0)
endif()