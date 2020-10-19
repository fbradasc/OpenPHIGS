#!/bin/bash

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

mkdir -p "${2}"
SRC_FILE="${1}"
DST_PATH="$( getDestPath "${2}" )"
DST_FILE="${DST_PATH}"/$( basename "${SRC_FILE}" ).html
SRC_PATH=$( getScriptDir )

cd "${SRC_PATH}"

echo "-- Processing: ${DST_FILE}" 1>&2

# cat "${SRC_FILE}"
cat phigs.macros "${SRC_FILE}" \
| \
groff -S -s -p -t -e -Tascii -mandoc -P -c - \
| \
./man2html -noheads -seealso -compress -cgiurl "${SRC_FILE}.opm" -title "${SRC_FILE}.opm" \
> \
"${DST_FILE}"
