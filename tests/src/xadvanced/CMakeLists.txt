ADD_EXECUTABLE(ux_xlib2 ux_xlib2.c)
ADD_EXECUTABLE(Motif2 Motif2.c)
ADD_EXECUTABLE(ux_visual ux_visual.c)

TARGET_LINK_LIBRARIES(ux_xlib2 ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(Motif2 ${PHIGS_LIBRARIES} ${MOTIF_LIBRARIES})
TARGET_LINK_LIBRARIES(ux_visual ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            ux_xlib2
            Motif2
            ux_visual
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
