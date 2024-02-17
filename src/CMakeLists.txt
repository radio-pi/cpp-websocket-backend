add_executable(radiopi main.cc)
target_link_libraries(radiopi PRIVATE ${LIBVLC_LIBRARY} Crow::Crow)
