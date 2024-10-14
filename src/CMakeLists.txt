add_executable(radiopi player.cc main.cc)
target_link_libraries(radiopi PRIVATE ${LIBVLC_LIBRARY} Crow::Crow)

install (TARGETS radiopi RUNTIME DESTINATION usr/bin)
install (FILES static/index.html DESTINATION usr/bin/static)
