ADD_EXECUTABLE(map map.c)

TARGET_LINK_LIBRARIES(map ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            map
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
