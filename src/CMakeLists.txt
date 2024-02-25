add_executable(radiopi main.cc)
target_link_libraries(radiopi PRIVATE ${LIBVLC_LIBRARY} Crow::Crow)

file(MAKE_DIRECTORY static)
file(COPY static/index.html DESTINATION static)
file(COPY static/index.html DESTINATION ../static)
