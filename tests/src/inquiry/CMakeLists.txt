ADD_EXECUTABLE(complex_inq complex_inq.c)

TARGET_LINK_LIBRARIES(complex_inq ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            complex_inq
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
