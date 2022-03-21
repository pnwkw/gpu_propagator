function(add_shader_target target_name infile)
    get_filename_component(outfile "${infile}" NAME_WE)
    add_custom_target(${target_name} DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${outfile}.spv")
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${outfile}.spv"
            COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/${infile} -o ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.spv
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${infile}"
    )
endfunction()
