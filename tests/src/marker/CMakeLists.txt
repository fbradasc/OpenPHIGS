ADD_EXECUTABLE(marker marker.c)

TARGET_LINK_LIBRARIES(marker ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            marker
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
