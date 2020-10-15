ADD_EXECUTABLE(sphere sphere.c)
ADD_EXECUTABLE(sphere2 sphere2.c)
ADD_EXECUTABLE(sphere3 sphere3.c)
ADD_EXECUTABLE(sphere4 sphere4.c)
ADD_EXECUTABLE(dcue dcue.c)
ADD_EXECUTABLE(light_tool light_tool.c)

TARGET_LINK_LIBRARIES(sphere ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(sphere2 ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(sphere3 ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(sphere4 ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(dcue ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(light_tool ${PHIGS_LIBRARIES} ${MOTIF_LIBRARIES})

INSTALL(TARGETS
            sphere
            sphere2
            sphere3
            sphere4
            dcue
            light_tool
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
