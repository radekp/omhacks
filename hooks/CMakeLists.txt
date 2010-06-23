include_directories(..)

add_definitions(-Wall -Werror)

add_library(hook_omhacks SHARED omhacks.c)
set_target_properties(hook_omhacks PROPERTIES
	PREFIX ""
	OUTPUT_NAME "omhacks"
	CLEAN_DIRECT_OUTPUT 1)
target_link_libraries(hook_omhacks omhacks)

#add_library(hook_at SHARED at.c)
#set_target_properties(hook_at PROPERTIES OUTPUT_NAME "at" CLEAN_DIRECT_OUTPUT 1)

install(TARGETS hook_omhacks LIBRARY DESTINATION lib/pm-utils/sleep.d/)
#install(TARGETS hook_at LIBRARY DESTINATION lib/pm-utils/sleep.d/)
install(PROGRAMS 00at DESTINATION share/doc/omhacks/examples/)
