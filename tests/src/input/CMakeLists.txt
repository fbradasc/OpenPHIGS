ADD_EXECUTABLE(move move.c)
ADD_EXECUTABLE(movex movex.c ../xbasic/openxws.c)
ADD_EXECUTABLE(pick pick.c)
ADD_EXECUTABLE(pickx pickx.c ../xbasic/openxws.c)

TARGET_LINK_LIBRARIES(move ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(movex ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(pick ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(pickx ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            move
            movex
            pick
            pickx
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
