ADD_EXECUTABLE(ux_xlib ux_xlib.c openxws.c)
ADD_EXECUTABLE(ux_xlib_rr ux_xlib_rr.c openxws.c)
ADD_EXECUTABLE(Motif Motif.c openxws.c)
ADD_EXECUTABLE(Motif_rr Motif_rr.c openxws.c)

TARGET_LINK_LIBRARIES(ux_xlib ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(ux_xlib_rr ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(Motif ${PHIGS_LIBRARIES} ${MOTIF_LIBRARIES})
TARGET_LINK_LIBRARIES(Motif_rr ${PHIGS_LIBRARIES} ${MOTIF_LIBRARIES})

INSTALL(TARGETS
            ux_xlib
            ux_xlib_rr
            Motif
            Motif_rr
        RUNTIME DESTINATION share/phigs/tests
        EXCLUDE_FROM_ALL
)
