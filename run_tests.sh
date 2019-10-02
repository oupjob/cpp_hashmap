#!/bin/bash

PROJ_ROOT=`dirname $PWD/$0 | sed -r "s/(\/)\.(\/){0,1}/\1/g"`

if [[ $1 == "from_makefile" ]]; then
	PROJ_ROOT=`dirname $0`;
	PROJ_ROOT="$PROJ_ROOT/";
fi

TESTS_DIR="$PROJ_ROOT"build/test
LIBS_DIR="$PROJ_ROOT"build/lib
export LD_LIBRARY_PATH=`echo $LD_LIBRARY_PATH:$LIBS_DIR | sed s/^://g`
SEPARATOR="###############################################################################################################"

export SUCCES_COUNT=0
TESTS_COUNT=0

function test_failed {
	printf "\nTEST FAILED:      \"$1\" \n"
}

function test_success {
	printf "\nTEST SUCCESSFUL:  \"$1\" \n"
	let "SUCCES_COUNT=SUCCES_COUNT+1"
}

for i in $TESTS_DIR/*; do
	TEST_NAME=`basename $i`;
	if [[ "${TEST_NAME##*.}" == 'o' ]]; then continue; fi
	
	let "TESTS_COUNT=TESTS_COUNT+1"
	
	echo $SEPARATOR
	echo "RUNNING TEST:     \"$TEST_NAME\" ...";
	echo
	$i && test_success $TEST_NAME || test_failed $TEST_NAME
done

echo
echo $SEPARATOR
echo "Success tests/tests count = ($SUCCES_COUNT/$TESTS_COUNT)"
[ $TESTS_COUNT == $SUCCES_COUNT ] && echo "All tests completed succesfully" || echo "Some tests completed unsuccesfully"

echo

[ $TESTS_COUNT == $SUCCES_COUNT ] && exit 0 || exit 1
