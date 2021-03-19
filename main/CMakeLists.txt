idf_component_register( SRCS "blink.c"
                        INCLUDE_DIRS "."
                        EMBED_TXTFILES "cert.pem")

add_definitions(-DSSID="${SSID}")
add_definitions(-DPASS="${PASS}")

if (DEFINED ${BUILD_VERSION})
    add_definitions(-DBUILD_VERSION=${BUILD_VERSION})
else()
    add_definitions(-DBUILD_VERSION=0)
endif()