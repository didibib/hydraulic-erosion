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

set(HE 
    "include/hydraulic_erosion.h"
    "src/hydraulic_erosion.cpp"
)

set(DROPLET 
    "include/droplet.h"
    "src/droplet.cpp"
)

set(HEIGHT_MAP 
    "include/height_map.h"
    "src/height_map.cpp"
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
	${HE}
    ${DROPLET}
    ${HEIGHT_MAP}
)

source_group(\\ FILES ${ROOT_FILES})
source_group(pch FILES ${PCH})
source_group(glad FILES ${GLAD})
source_group(util FILES ${UTIL})
source_group(window FILES ${WINDOW})
source_group(scene/camera FILES ${CAMERA})
source_group(scene/vertex-buffer FILES ${VERTEX_BUFFER})
source_group(scene/shader FILES ${SHADER})
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

target_link_libraries(${TARGET} PUBLIC
	glfw
	glm
    spdlog::spdlog
)