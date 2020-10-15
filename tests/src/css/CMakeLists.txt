ADD_EXECUTABLE(css_probe css_probe.c )
ADD_EXECUTABLE(paths paths.c)

TARGET_LINK_LIBRARIES(css_probe ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(paths ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            css_probe
            paths
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
