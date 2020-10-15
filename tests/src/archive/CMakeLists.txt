ADD_EXECUTABLE(shapes shapes.c )
ADD_EXECUTABLE(simple simple.c)

TARGET_LINK_LIBRARIES(shapes ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(simple ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            shapes
            simple
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
