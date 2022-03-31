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
	${HE}
    ${DROPLET}
    ${HEIGHT_MAP}
)

source_group(\\ FILES ${ROOT_FILES})
source_group(pch FILES ${PCH})
source_group(glad FILES ${GLAD})
source_group(util FILES ${UTIL})
source_group(window FILES ${WINDOW})
source_group(camera FILES ${CAMERA})
source_group(hydraulic-erosion FILES ${HE} ${DROPLET} ${HEIGHT_MAP})

set_target_output_directories(${TARGET})

set_target_precompiled_header_msvc(${TARGET} "hepch.h" "src/hepch.cpp")

set(IGNORE_PCH 
	"src/glad.c"
)
ignore_precompiled_header_msvc(IGNORE_PCH)

target_include_directories(${TARGET} PUBLIC
    "include"
)

target_link_libraries(${TARGET} PUBLIC
	glfw
	glm
    spdlog::spdlog
)