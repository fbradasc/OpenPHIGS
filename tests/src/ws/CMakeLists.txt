ADD_EXECUTABLE(ws_is_open ws_is_open.c)

TARGET_LINK_LIBRARIES(ws_is_open ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            ws_is_open
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
