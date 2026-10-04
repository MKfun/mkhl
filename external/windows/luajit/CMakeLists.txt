add_library( luajit STATIC IMPORTED GLOBAL )
add_library( LuaJIT::LuaJIT ALIAS luajit )

set_target_properties( luajit PROPERTIES
	IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/lua51.lib"
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
