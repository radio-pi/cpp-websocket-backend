add_executable(radiopi player.cc main.cc)
target_link_libraries(radiopi PRIVATE ${LIBVLC_LIBRARY} Crow::Crow)

file(MAKE_DIRECTORY static)
file(COPY static/index.html DESTINATION static)
file(COPY static/index.html DESTINATION ../static)
