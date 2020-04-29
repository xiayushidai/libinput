

project(input-device-interface-test)

set(TARGET_NAME input-device-interface-test)

FILE(GLOB SC_FILES "*.cpp" "*.h")

include_directories(../src)



add_executable(${TARGET_NAME} ${SC_FILES})


link_directories("./lib")
target_link_libraries(${TARGET_NAME}  input-device-interface)
