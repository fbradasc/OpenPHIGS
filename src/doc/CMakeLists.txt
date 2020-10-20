SET(DOC_DIR "doc/OpenPHIGS")

FILE(GLOB MAN_FILES manpages/* )

IF(CMAKE_HOST_UNIX)
    INSTALL(DIRECTORY htmlsrc/ DESTINATION ${DOC_DIR}/html)
    FOREACH(m IN LISTS MAN_FILES)
        INSTALL(CODE
            "EXECUTE_PROCESS( \
            COMMAND ${CMAKE_SOURCE_DIR}/src/doc/phigs_man_to_html.sh ${m} \${CMAKE_INSTALL_PREFIX}/${DOC_DIR}/html \
        )"
    )
    ENDFOREACH()

    #INSTALL(DIRECTORY htmlsrc/ DESTINATION ${DOC_DIR}/html)
    INSTALL(DIRECTORY manpages DESTINATION ${DOC_DIR})

    INSTALL(CODE
        "EXECUTE_PROCESS( \
            COMMAND ${CMAKE_SOURCE_DIR}/src/doc/link_phigs_man.sh \${CMAKE_INSTALL_PREFIX}/${DOC_DIR} \
        )"
    )
ENDIF(CMAKE_HOST_UNIX)
