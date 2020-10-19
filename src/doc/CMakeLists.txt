SET(DOC_DIR "doc/OpenPHIGS")

FILE(GLOB MAN_FILES manpages/* )

IF(CMAKE_HOST_UNIX)
    FOREACH(m IN LISTS MAN_FILES)
        #        GET_FILENAME_COMPONENT(h ${m} NAME_WE)
        #        INSTALL(CODE
        #            "EXECUTE_PROCESS( \
        #                COMMAND groff -S -s -p -t -e -T html -mandoc ${m} \
        #                OUTPUT_FILE \${CMAKE_INSTALL_PREFIX}/${DOC_DIR}/html/${h}.html \
        #            )"
        #        )
        INSTALL(CODE
            "EXECUTE_PROCESS( \
            COMMAND ${CMAKE_SOURCE_DIR}/src/doc/phigs_man_to_html.sh ${m} \${CMAKE_INSTALL_PREFIX}/${DOC_DIR}/html \
        )"
    )
    ENDFOREACH()

    INSTALL(DIRECTORY htmlsrc/ DESTINATION ${DOC_DIR}/html)
    INSTALL(DIRECTORY manpages DESTINATION ${DOC_DIR})

    INSTALL(CODE
        "EXECUTE_PROCESS( \
            COMMAND ${CMAKE_SOURCE_DIR}/src/doc/link_phigs_man.sh \${CMAKE_INSTALL_PREFIX}/${DOC_DIR} \
        )"
    )
ENDIF(CMAKE_HOST_UNIX)
