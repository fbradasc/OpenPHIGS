CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(PHIGS)

SET(CMAKE_MODULE_PATH ${PHIGS_SOURCE_DIR}/cmake)
FIND_PACKAGE(X11)
FIND_PACKAGE(XMU)
FIND_PACKAGE(Motif)

FIND_LIBRARY(X11_Xaw3d_LIB
             NAMES Xaw3d Xaw3d
             /usr/X11/include/X11/
             /usr/X11/lib
             ${X11_LIB_SEARCH_PATH}
)

# SET(CMAKE_C_FLAGS -Wall)

ADD_DEFINITIONS(
    -DNON_PORTABLE
    -DNARROWPROTO
    -DFUNCPROTO=15
    -D_DEFAULT_SOURCE
    -D_XOPEN_SOURCE=500L
    -D_POSIX_SOURCE
    -D_POSIX_C_SOURCE=199309L
)

SET(P_COMMON_FLAGS
    "${CMAKE_C_FLAGS} -fomit-frame-pointer -pedantic -ansi -fno-strength-reduce"
)

SET(CMAKE_C_FLAGS
    "-std=c99 -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast ${P_COMMON_FLAGS}"
)

# -Wall
SET(CMAKE_CXX_FLAGS
    "-fno-exceptions -Wno-return-type ${P_COMMON_FLAGS}"
)

INCLUDE_DIRECTORIES(
    ${PHIGS_SOURCE_DIR}/src/include
    ${PHIGS_SOURCE_DIR}/src/include/phigs
    ${PHIGS_SOURCE_DIR}/src/include/phigs/PEX5
)

SET(PHIGS_LIBRARIES
    phigs
    ${X11_LIBRARIES}
    ${X11_Xt_LIB}
    ${X11_Xaw3d_LIB}
    ${XMU_LIBRARY}
    m
)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tests)
