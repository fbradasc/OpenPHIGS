ADD_EXECUTABLE(billions billions.c)
ADD_EXECUTABLE(billions_color billions_color.c)
ADD_EXECUTABLE(billions_ind billions_ind.c)
ADD_EXECUTABLE(buttons buttons.c)
ADD_EXECUTABLE(first first.c)
ADD_EXECUTABLE(houses houses.c)
ADD_EXECUTABLE(sedit sedit.c)

TARGET_LINK_LIBRARIES(billions ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(billions_color ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(billions_ind ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(buttons ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(first ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(houses ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(sedit ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            billions
            billions_color
            billions_ind
            buttons
            first
            houses
            sedit
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
