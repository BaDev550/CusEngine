file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 
add_library(RHI_OpenGL STATIC ${SOURCECODE})

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

target_compile_definitions(RHI_OpenGL PRIVATE ENGINE_EXPORTS_STATIC_LIB)
target_include_directories(RHI_OpenGL PRIVATE ${CMAKE_SOURCE_DIR}/Source/Engine)

target_link_libraries(RHI_OpenGL PRIVATE
	glfw
	glad::glad
	glm::glm-header-only
)