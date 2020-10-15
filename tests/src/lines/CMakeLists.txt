ADD_EXECUTABLE(circle circle.c)
ADD_EXECUTABLE(curves curves.c)
ADD_EXECUTABLE(plineset plineset.c)
ADD_EXECUTABLE(polyline polyline.c)

TARGET_LINK_LIBRARIES(circle ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(curves ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(plineset ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(polyline ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            circle
            curves
            plineset
            polyline
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
