get_filename_component(DIR ${CMAKE_CURRENT_LIST_DIR} NAME)
project(${DIR})
add_executable(${DIR}
    main.cpp rendering_context.cpp camera_control.cpp passthrough_vshader.glsl

    vertices_grid.cpp vertices_skybox.cpp vertices_quad.cpp
    vertices_camera_path.cpp vertices_camera_pictorial.cpp
    vertices_duck.cpp

    rendering_simple.cpp      rendering_simple_vshader.glsl      rendering_simple_fshader.glsl
    heightmap.cpp             heightmap_fshader.glsl
    terrain.cpp               terrain_vshader.glsl               terrain_fshader.glsl
    skybox.cpp                skybox_vshader.glsl                skybox_fshader.glsl
    shadowmap.cpp             shadowmap_vshader.glsl             shadowmap_fshader.glsl
    water.cpp                 water_vshader.glsl                 water_fshader.glsl
    camera_path_controls.cpp  camera_path_controls_vshader.glsl  camera_path_controls_gshader.glsl
    particles_control.cpp     particles_control_fshader.glsl
    particles_render.cpp      particles_render_vshader.glsl      particles_render_fshader.glsl
    duck.cpp		      duck_vshader.glsl		         duck_fshader.glsl
    post_processing.cpp       post_processing_fshader.glsl
    duck.cpp		      duck_vshader.glsl			 duck_fshader.glsl
)

target_link_libraries(${DIR} ${COMMON_LIBS})
target_stringify_shader(${DIR} passthrough_vshader)
target_stringify_shader(${DIR} particles_control_fshader)
target_stringify_shader(${DIR} heightmap_fshader)
target_stringify_shader(${DIR} terrain_vshader)
target_stringify_shader(${DIR} terrain_fshader)
target_stringify_shader(${DIR} skybox_vshader)
target_stringify_shader(${DIR} skybox_fshader)
target_stringify_shader(${DIR} shadowmap_vshader)
target_stringify_shader(${DIR} shadowmap_fshader)
target_stringify_shader(${DIR} water_vshader)
target_stringify_shader(${DIR} water_fshader)
target_stringify_shader(${DIR} particles_render_vshader)
target_stringify_shader(${DIR} particles_render_fshader)
target_stringify_shader(${DIR} rendering_simple_vshader)
target_stringify_shader(${DIR} rendering_simple_fshader)
target_stringify_shader(${DIR} camera_path_controls_vshader)
target_stringify_shader(${DIR} camera_path_controls_gshader)
target_stringify_shader(${DIR} duck_vshader)
target_stringify_shader(${DIR} duck_fshader)
target_stringify_shader(${DIR} post_processing_fshader)
target_stringify_shader(${DIR} duck_vshader)
target_stringify_shader(${DIR} duck_fshader)
