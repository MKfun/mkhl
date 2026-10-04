set( CMAKE_FOLDER "External" )


add_subdirectory( glad )
add_subdirectory( nlohmann_json )
add_subdirectory( pcre )
add_subdirectory( SDL2 )
add_subdirectory( semver.c )
set(RMLUI_SAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(RMLUI_LUA_BINDINGS ON CACHE BOOL "" FORCE)
set(RMLUI_SVG_PLUGIN ON CACHE BOOL "" FORCE)
add_subdirectory( lunasvg )
if (WIN32)
    # set(LUA_LIBRARIES "etxternal/windows/luajit/lib/lua51.lib" CACHE PATH "" FORCE)
    # set(LUA_INCLUDE_DIR "etxternal/linux/luajit/include/" CACHE PATH "" FORCE)
    add_subdirectory( windows/luajit )
    add_subdirectory( windows )
else()
    # set(LUA_LIBRARIES "etxternal/linux/luajit/lib/libluajit.a" CACHE PATH "" FORCE)
    # set(LUA_INCLUDE_DIR "etxternal/linux/luajit/include/" CACHE PATH "" FORCE)
    add_subdirectory( linux/luajit )
endif()
set(LUA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/linux/luajit/include" CACHE PATH "" FORCE)
set(LUA_LIBRARIES "LuaJIT::LuaJIT" CACHE PATH "" FORCE)

set(RMLUI_LUA_BINDINGS_LIBRARY "lua" CACHE STRING "" FORCE)


add_subdirectory( RmlUi )

set( SDL2_INCLUDE_FILES "${SDL2_INCLUDE_FILES}" PARENT_SCOPE )
