CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(new_detector)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED 11)

SET(INCLUDEPATH
    /usr/local/include
    )

include_directories( ${INCLUDEPATH} )

IF(CMAKE_SYSTEM MATCHES "Linux*")
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wall -std=c++17 -Wno-reorder -Wno-deprecated-declarations" )
  SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11 " )
  # Linux Specific flags
  add_definitions( -DTARGET_LINUX )
  link_directories( /usr/local/lib
                    /usr/lib/x86_64-linux-gnu
                    )
ENDIF(CMAKE_SYSTEM MATCHES "Linux*")

IF(CMAKE_SYSTEM MATCHES "Darwin*")
  # OS X specific flags
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lpthread -Wl,--no-as-needed -Wno-unused-parameter -Wall -std=c++11 -Wno-reorder -Wno-deprecated-declarations" )
  add_definitions( -DTARGET_OSX )
ENDIF(CMAKE_SYSTEM MATCHES "Darwin*")

IF (CMAKE_SYSTEM MATCHES "Win*")
ENDIF(CMAKE_SYSTEM MATCHES "Win*")

add_definitions( -DUSE_KISSFFT )

# Source files
SET (SOURCES
    demo.cpp
    detector.cpp
    Audio_WavFile.cpp
    )

SET (HEADERS
    detector.h
    Audio_WavFile.h
    )

# Name executable as new_detector
ADD_EXECUTABLE(new_detector ${SOURCES})

# Link all needed libraries
# TARGET_LINK_LIBRARIES(m)
