ADD_EXECUTABLE(mclip mclip.c ../xbasic/openxws.c)
ADD_EXECUTABLE(orbit orbit.c)
ADD_EXECUTABLE(sulfate sulfate.c)
ADD_EXECUTABLE(sulfate2 sulfate2.c)

TARGET_LINK_LIBRARIES(mclip ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(orbit ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(sulfate ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(sulfate2 ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            mclip
            orbit
            sulfate
            sulfate2
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
