add_library(
    jcfb
    ${CMAKE_CURRENT_SOURCE_DIR}/jcfb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/keyboard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mouse.c
    ${CMAKE_CURRENT_SOURCE_DIR}/pixel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bitmap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bitmap-io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/primitive.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ttf.c
)

install(TARGETS jcfb DESTINATION lib)
