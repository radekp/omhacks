include_directories(..)

file(GLOB ohsrc *.c)
file(GLOB ohhdr *.h)

add_definitions(-Wall -Werror)
link_libraries(dl)

add_library(omhacks SHARED ${ohsrc})
add_library(omhacks-static STATIC ${ohsrc})
set_target_properties(omhacks PROPERTIES VERSION ${omhacks_version} SOVERSION 0 CLEAN_DIRECT_OUTPUT 1)
set_target_properties(omhacks-static PROPERTIES VERSION ${omhacks_version} SOVERSION 0 OUTPUT_NAME "omhacks" CLEAN_DIRECT_OUTPUT 1)

install(TARGETS omhacks omhacks-static
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(FILES ${ohhdr} DESTINATION include/omhacks )
