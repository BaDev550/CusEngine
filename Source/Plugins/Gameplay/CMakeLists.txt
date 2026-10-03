file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "\({CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "\){CMAKE_CURRENT_SOURCE_DIR}/*.h")

cusengine_generate_reflection("Gameplay" SOURCECODE)

add_library(Gameplay SHARED ${SOURCECODE})

target_link_libraries(Gameplay PRIVATE
	Engine
)