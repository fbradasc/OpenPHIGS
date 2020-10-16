#!/bin/bash -x

function getScriptDir
{
    cd $( dirname $( readlink -f $0 ) ) >& /dev/null
    pwd
    cd - >& /dev/null
}

function getDestPath
{
    cd "${1}" >& /dev/null
    pwd
    cd - >& /dev/null
}

SRC_FILE="${1}"
DST_PATH="$( getDestPath \"${2}\" )"
DST_FILE="${DST_PATH}/$( basename \"${SRC_FILE}\" ).html"
SRC_PATH=$( getScriptDir )

mkdir -p "${DST_PATH}"

cd "${SRC_PATH}"

cat phigs.macros "${SRC_FILE}" \
| \
groff -S -s -p -t -e -Tascii -mandoc - \
| \
./man2html -noheads -seealso -compress -cgiurl "${SRC_FILE}.opm" -title "${SRC_FILE}.opm" \
> \
"${DST_FILE}"
