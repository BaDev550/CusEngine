file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_executable(Playground ${SOURCECODE})

target_include_directories(Playground PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Playground PRIVATE
	Engine
)