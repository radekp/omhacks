include_directories(..)

add_definitions(-Wall -Werror)

add_library(hook_omhacks SHARED omhacks.c)

install(TARGETS hook_omhacks LIBRARY DESTINATION lib/pm-utils/sleep.d/)
set_target_properties(hook_omhacks PROPERTIES OUTPUT_NAME "omhacks" CLEAN_DIRECT_OUTPUT 1)
