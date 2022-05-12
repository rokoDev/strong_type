target_sources(strong_type
	PRIVATE
    include/strong_type/strong_type.h
  )

set_property(TARGET strong_type APPEND PROPERTY SRC_DIRS "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(strong_type INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
