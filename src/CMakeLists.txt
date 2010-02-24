include_directories(..)
link_libraries(omhacks)

add_definitions(-Wall -Werror -DVERSION="${omhacks_version}")
add_executable(om om.c om-cmdline.c)
add_executable(om-led om-led.c om-cmdline.c)

set( bindir ${CMAKE_CURRENT_BINARY_DIR} )
set( srcdir ${CMAKE_CURRENT_SOURCE_DIR} )

find_program(HELP2MAN help2man)
if (HELP2MAN)
	message("-- help2man found at ${HELP2MAN}: building man page")

	add_custom_command(
	    TARGET om
	    POST_BUILD
	    DEPENDS om
	    COMMAND ${HELP2MAN} -n "control OpenMoko phone hardware" --no-info -I${srcdir}/om.help2man ${bindir}/om > ${bindir}/om.1 )

	add_custom_command(
	    TARGET om-led
	    POST_BUILD
	    DEPENDS om-led
	    COMMAND ${HELP2MAN} -n "control OpenMoko phone LEDs" --no-info -I${srcdir}/om-led.help2man ${bindir}/om-led > ${bindir}/om-led.1 )

	install(FILES ${bindir}/om.1 ${bindir}/om-led.1 DESTINATION share/man/man1 )
else ()
	message("-- help2man not found: not building man page")
endif ()

install(TARGETS om om-led RUNTIME DESTINATION bin)
