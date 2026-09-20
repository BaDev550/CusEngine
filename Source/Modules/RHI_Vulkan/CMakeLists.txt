file(GLOB_RECURSE SOURCECODE CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h") 

set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/Generated")
file(MAKE_DIRECTORY ${GENERATED_DIR})

set(GENERATED_SOURCES "")

foreach(SOURCE_FILE ${SOURCECODE})
    if("${SOURCE_FILE}" MATCHES "\\.h$")
        file(READ ${SOURCE_FILE} FILE_CONTENT)
        string(REGEX MATCH "CUS_CLASS\\(\\)[ \t\r\n]*class" MACRO_FOUND "${FILE_CONTENT}")

        if(MACRO_FOUND)
            get_filename_component(BASE_NAME ${SOURCE_FILE} NAME_WE)
            set(GENERATED_CPP "${GENERATED_DIR}/${BASE_NAME}.gen.cpp")

            add_custom_command(
                OUTPUT ${GENERATED_CPP}
                COMMAND python "${CMAKE_SOURCE_DIR}/Tools/HeaderTool.py" "${SOURCE_FILE}" "${GENERATED_CPP}" "${CMAKE_CURRENT_SOURCE_DIR}"
                DEPENDS ${SOURCE_FILE} "${CMAKE_SOURCE_DIR}/Tools/HeaderTool.py"
                COMMENT "Generating reflection for ${BASE_NAME}.h (Gameplay Plugin)"
            )

            list(APPEND GENERATED_SOURCES ${GENERATED_CPP})
        endif()
    endif()
endforeach()

add_library(RHI_Vulkan SHARED ${SOURCECODE} ${GENERATED_SOURCES})

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

target_compile_definitions(RHI_Vulkan PRIVATE ENGINE_EXPORTS)
target_link_libraries(RHI_Vulkan PRIVATE
	Engine
	glfw
	glm::glm-header-only
	Vulkan::Vulkan
	GPUOpen::VulkanMemoryAllocator
)