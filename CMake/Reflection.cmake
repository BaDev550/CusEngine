function(add_reflection TARGET)
    cmake_parse_arguments(ARG "" "PROJECT;SOURCE;EXPORT_MACRO" "" ${ARGN})

    if(NOT ARG_PROJECT)
        set(ARG_PROJECT ${TARGET})
    endif()
    if(NOT ARG_SOURCE)
        set(ARG_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    set(EXPORT_ARGS "")
    if(DEFINED ARG_EXPORT_MACRO)
        set(EXPORT_ARGS "--export-macro=${ARG_EXPORT_MACRO}")
    else()
        get_target_property(_type ${TARGET} TYPE)
        if(_type STREQUAL "EXECUTABLE")
            set(EXPORT_ARGS "--export-macro=")
        endif()
    endif()

    add_custom_target(${TARGET}_ReflectGen
        COMMAND ${Python3_EXECUTABLE} ${REFLECTION_TOOL}
                --root ${CMAKE_SOURCE_DIR}
                --project ${ARG_PROJECT}
                --source "${ARG_SOURCE}"
                ${EXPORT_ARGS}
        COMMENT "Generating reflection for ${ARG_PROJECT}"
        VERBATIM)

    add_dependencies(${TARGET} ${TARGET}_ReflectGen)
    target_include_directories(${TARGET} PRIVATE ${REFLECTION_INCLUDE_PATH})
endfunction()
