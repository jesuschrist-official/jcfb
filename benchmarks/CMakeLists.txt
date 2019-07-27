add_executable(pixel-conversion.bench ${CMAKE_CURRENT_SOURCE_DIR}/pixel-conversion.c)
add_executable(bitmap-blit.bench      ${CMAKE_CURRENT_SOURCE_DIR}/bitmap-blit.c)
add_executable(jcfb-refresh.bench     ${CMAKE_CURRENT_SOURCE_DIR}/jcfb-refresh.c)

target_link_libraries(pixel-conversion.bench jcfb m)
target_link_libraries(bitmap-blit.bench      jcfb m)
target_link_libraries(jcfb-refresh.bench     jcfb m)
