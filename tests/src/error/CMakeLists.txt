ADD_EXECUTABLE(error error.c)

TARGET_LINK_LIBRARIES(error ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            error
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
