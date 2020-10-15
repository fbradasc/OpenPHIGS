SET(DOC_DIR "doc/OpenPHIGS")

FILE(GLOB MAN_FILES manpages/*)

IF(CMAKE_HOST_UNIX)
#FOREACH(m IN LISTS MAN_NAMES)
#  SET(mf ${CMAKE_BINARY_DIR}/${m})
#  SET(ms ${CMAKE_SOURCE_DIR}/${m}.txt)
#  ADD_CUSTOM_COMMAND(OUTPUT ${mf}
#    COMMAND ${A2X_EXECUTABLE} ${A2X_OPTS} ${ms}
#    DEPENDS ${ms}
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#    COMMENT "Building manpage ${mf}"
#    VERBATIM)
#  LIST(APPEND MAN_FILES ${mf})
#ENDFOREACH()

#    INSTALL(FILES
#                ${MAN_FILES}
#            DESTINATION
#            ${DOC_DIR}/manpages
#    )

    INSTALL(CODE
            "EXECUTE_PROCESS( \
                COMMAND ${CMAKE_SOURCE_DIR}/src/doc/link_phigs_man.sh ${DESTINATION} \
            )"
    )
ENDIF(CMAKE_HOST_UNIX)
