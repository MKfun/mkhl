set( CMAKE_FOLDER "External" )


add_subdirectory( glad )
add_subdirectory( nlohmann_json )
add_subdirectory( pcre )
add_subdirectory( SDL2 )
add_subdirectory( semver.c )
set(RMLUI_SAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory( RmlUi )

if (WIN32)
    add_subdirectory( windows )
endif()

set( SDL2_INCLUDE_FILES "${SDL2_INCLUDE_FILES}" PARENT_SCOPE )
