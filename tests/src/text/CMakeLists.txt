ADD_EXECUTABLE(text text.c)
TARGET_LINK_LIBRARIES(text ${PHIGS_LIBRARIES})

ADD_EXECUTABLE(extent extent.c ../xbasic/openxws.c)
TARGET_LINK_LIBRARIES(extent ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            text
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
