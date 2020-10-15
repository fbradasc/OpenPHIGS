FIND_PROGRAM(AWK awk gawk mawk nawk)

ADD_CUSTOM_COMMAND(OUTPUT
                   errnum.h
                   COMMAND
                   "${AWK} -f errgen.awk phigserr.h > errnum.h"
)

ADD_CUSTOM_COMMAND(OUTPUT
                   PHIGSerr.txt
                   COMMAND
                   "${AWK} -f errmsgen.awk phigserr.h > PHIGSerr.txt"
)

INSTALL(FILES
            phigs.h
            attr.h
            errnum.h
            phigscfunc.h
            phigsextrn.h
            phigserr.h
        DESTINATION
            include/phigs
)

INSTALL(FILES
            PHIGSerr.txt
            PHIGSfnc.txt
        DESTINATION
            lib/X11/PEX
)

