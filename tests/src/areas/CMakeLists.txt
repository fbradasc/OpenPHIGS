ADD_EXECUTABLE(fas fas.c)
ADD_EXECUTABLE(fasd fasd.c)
ADD_EXECUTABLE(fillarea fillarea.c)
ADD_EXECUTABLE(quad quad.c)
ADD_EXECUTABLE(sofas sofas.c)
ADD_EXECUTABLE(surface surface.c)
ADD_EXECUTABLE(surface_trimmed surface_trimmed.c)
ADD_EXECUTABLE(tristrip tristrip.c)

TARGET_LINK_LIBRARIES(fas ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(fasd ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(fillarea ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(quad ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(sofas ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(surface ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(surface_trimmed ${PHIGS_LIBRARIES})
TARGET_LINK_LIBRARIES(tristrip ${PHIGS_LIBRARIES})

INSTALL(TARGETS
            fas
            fasd
            fillarea
            quad
            sofas
            surface
            surface_trimmed
            tristrip
        RUNTIME DESTINATION share/phigs/tests
        COMPONENT tests
        EXCLUDE_FROM_ALL
)
