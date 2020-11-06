cmake_minimum_required(VERSION 3.17)
project(tests)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

set(LDFLAGS -lm -ldl -lpthread)

set(BLACKMAGIC_SDK_DIR ../include/)
MESSAGE( "Searching for Blackmagic SDK in: ${BLACKMAGIC_SDK_DIR}" )
find_path( BLACKMAGIC_DECKLINK_INCLUDE_DIR "DeckLinkAPI.h"
  PATHS ${BLACKMAGIC_SDK_DIR} )
if( NOT BLACKMAGIC_DECKLINK_INCLUDE_DIR )
  MESSAGE( FATAL_ERROR "Cannot find DeckLinkAPI.h, please give a path to the Blackmagic SDK by setting BLACKMAGIC_SDK_DIR" )
endif()
MESSAGE( "Including Blackmagic SDK from: ${BLACKMAGIC_DECKLINK_INCLUDE_DIR}" )

include_directories(
        ../include/
        ${BLACKMAGIC_DECKLINK_INCLUDE_DIR}
)

add_executable(tests main.cpp DisplayVideo3D.cpp ../include/DeckLinkAPIDispatch.cpp)
target_link_libraries(tests ${OpenCV_LIBS} ${LDFLAGS})