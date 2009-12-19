include_directories(..)
link_libraries(omhacks)

add_definitions(-Wall -Werror -DVERSION="${omhacks_version}")
add_executable(om om.c om-cmdline.c)
add_executable(om-led om-led.c om-cmdline.c)

install(TARGETS om om-led RUNTIME DESTINATION bin)
