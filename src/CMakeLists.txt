include_directories(..)
link_libraries(omhacks)

add_definitions(-Wall -Werror)
add_executable(om om.c om-cmdline.c)
add_executable(om-led om-led.c om-cmdline.c)

add_executable(pm-suspend pm-action.c)
target_link_libraries(pm-suspend dl -rdynamic)

install(TARGETS om om-led RUNTIME DESTINATION bin)
install(TARGETS pm-suspend RUNTIME DESTINATION sbin)
