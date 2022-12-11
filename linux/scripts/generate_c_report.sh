#!/bin/sh

FILE=$1.rpt
echo '/*  MAKE AND VALGRIND REPORT' > $FILE
make $1 | sed -e 's/^/ *  /' >> $FILE
valgrind ./$1 2>&1 | sed -e 's/^/ *  /' >> $FILE
echo ' */' >> $FILE

rm $1
