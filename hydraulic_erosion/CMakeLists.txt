set(TARGET hydraulic-erosion)

set(ROOT_FILES
    "src/main.cpp"
)

set(PCH
    "include/hepch.h"
    "src/hepch.cpp"
)

set(GLAD
    "include/glad/glad.h"
    "src/glad.c"
)

set(UTIL
    "include/util.h"
    "include/logger.h"
    "src/logger.cpp"    #"include/perlin_noise.hpp"
)

set(WINDOW
    "include/window.h"
    "src/window.cpp"
    "include/program.h"
)

set(CAMERA
    "include/camera.h"
    "src/camera.cpp"
)

set(VERTEX_BUFFER
    "include/vertex.h"
    "include/vertex_buffer.h"
    "src/vertex_buffer.cpp"
)

set(SHADER
    "include/shader.h"
    "src/shader.cpp"
)

set(BASIC_SHADER
    "include/basic_shader.h"
    "src/basic_shader.cpp"
)

set(TESSEL_SHADER
    "include/tessel_shader.h"
    "src/tessel_shader.cpp"
)

set(HE
    "include/droplet_erosion.h"
    "src/droplet_erosion.cpp"
    "include/shallow_erosion.h"
    "src/shallow_erosion.cpp"
)

set(DROPLET
    "include/droplet.h"
    "src/droplet.cpp"
)

set(HEIGHT_MAP
    "include/height_map.h"
    "src/height_map.cpp"
)

set(COLORS
    "include/colors.h"
    "src/colors.cpp"
)

set(TEXTURE 
    "include/stb_image.h"
    "include/texture_manager.h"
    "src/texture_manager.cpp"
)

add_executable(${TARGET}
    ${ROOT_FILES}
	${PCH}
	${GLAD}
	${UTIL}
	${WINDOW}
	${CAMERA}
    ${VERTEX_BUFFER}
    ${SHADER}
    ${BASIC_SHADER}
    ${TESSEL_SHADER}
	${HE}
    ${DROPLET}
    ${HEIGHT_MAP}
    ${COLORS}
    ${TEXTURE}
)

source_group(\\ FILES ${ROOT_FILES})
source_group(pch FILES ${PCH})
source_group(glad FILES ${GLAD})
source_group(util FILES ${UTIL} ${COLORS})
source_group(window FILES ${WINDOW})
source_group(scene/camera FILES ${CAMERA})
source_group(scene/vertex-buffer FILES ${VERTEX_BUFFER})
source_group(scene/shader FILES ${SHADER} ${BASIC_SHADER} ${TESSEL_SHADER})
source_group(scene/texture FILES ${TEXTURE})
source_group(hydraulic-erosion FILES ${HE} ${DROPLET} ${HEIGHT_MAP})

set_target_output_directories(${TARGET})

set_target_precompiled_header_msvc(${TARGET} "hepch.h" "src/hepch.cpp")

set(IGNORE_PCH
	"src/glad.c"
)
ignore_precompiled_header_msvc(IGNORE_PCH)

set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders/")
target_compile_definitions(${TARGET} PUBLIC SHADER_DIR_M=${SHADER_DIR})


target_include_directories(${TARGET} PUBLIC
    "include"
)

find_package(OpenMP)

target_link_libraries(${TARGET} PUBLIC
	glfw
	glm
    spdlog::spdlog
    OpenMP::OpenMP_CXX
)