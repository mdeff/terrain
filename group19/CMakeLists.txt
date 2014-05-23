get_filename_component(DIR ${CMAKE_CURRENT_LIST_DIR} NAME)
project(${DIR})
add_executable(${DIR} main.cpp rendering_context.cpp
                      vertices_grid.cpp vertices_skybox.cpp vertices_quad.cpp vertices_bezier.cpp
                      heightmap.cpp terrain.cpp skybox.cpp shadowmap.cpp watermap.cpp waterReflection.cpp camera.cpp
                      particles_control.cpp particles_render.cpp
                      heightmap_vshader.glsl heightmap_fshader.glsl
                      terrain_vshader.glsl   terrain_fshader.glsl
                      skybox_vshader.glsl    skybox_fshader.glsl
                      shadowmap_vshader.glsl shadowmap_fshader.glsl
                      watereflection_vshader.glsl	watereflection_fshader.glsl		   
                      watermap_vshader.glsl  watermap_fshader.glsl
                      camera_vshader.glsl    camera_fshader.glsl
                      particles_control_vshader.glsl particles_control_fshader.glsl
                      particles_render_vshader.glsl  particles_render_fshader.glsl)

target_link_libraries(${DIR} ${COMMON_LIBS})
target_stringify_shader(${DIR} heightmap_vshader)
target_stringify_shader(${DIR} heightmap_fshader)
target_stringify_shader(${DIR} terrain_vshader)
target_stringify_shader(${DIR} terrain_fshader)
target_stringify_shader(${DIR} skybox_vshader)
target_stringify_shader(${DIR} skybox_fshader)
target_stringify_shader(${DIR} shadowmap_vshader)
target_stringify_shader(${DIR} shadowmap_fshader)
target_stringify_shader(${DIR} watereflection_vshader)
target_stringify_shader(${DIR} watereflection_fshader)
target_stringify_shader(${DIR} watermap_vshader)
target_stringify_shader(${DIR} watermap_fshader)
target_stringify_shader(${DIR} camera_vshader)
target_stringify_shader(${DIR} camera_fshader)
target_stringify_shader(${DIR} particles_control_vshader)
target_stringify_shader(${DIR} particles_control_fshader)
target_stringify_shader(${DIR} particles_render_vshader)
target_stringify_shader(${DIR} particles_render_fshader)
