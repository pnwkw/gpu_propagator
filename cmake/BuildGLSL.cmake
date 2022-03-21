function(add_shader_target target_name infile)
    get_filename_component(outfile "${infile}" NAME_WE)
    add_custom_target(${target_name} DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${outfile}.glsl")
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${outfile}.glsl"
            COMMAND python3 ${CMAKE_SOURCE_DIR}/cmake/process_glsl.py --includepath ${CMAKE_CURRENT_SOURCE_DIR} ${infile} ${CMAKE_CURRENT_BINARY_DIR}/${outfile}.glsl
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${infile}"
    )
endfunction()
