ADD_EXECUTABLE(bundles bundles.c)

TARGET_LINK_LIBRARIES(bundles ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            bundles
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
