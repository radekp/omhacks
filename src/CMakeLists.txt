include_directories(..)
link_libraries(omhacks)

add_definitions(-Wall -Werror)
add_executable(om om.c om-cmdline.c)
add_executable(om-led om-led.c om-cmdline.c)

# TODO: see how to add -rdynamic only to pm-suspend
add_definitions(-rdynamic)
# TODO: see how to add -ldl only to pm-suspend
link_libraries(dl)
add_executable(pm-suspend pm-action.c)

install(TARGETS om om-led RUNTIME DESTINATION bin)
install(TARGETS pm-suspend RUNTIME DESTINATION sbin)
