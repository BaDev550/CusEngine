file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_executable(LevelEditor ${SOURCECODE})

find_package(imgui CONFIG REQUIRED)

target_include_directories(LevelEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(LevelEditor PRIVATE
	Engine
	imgui::imgui
)