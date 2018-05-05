cmake_minimum_required(VERSION 3.5)
project(TestFiles C)

if(DEFINED BITS)
	message("BITS set to ${BITS}")
else()
	set(BITS 64)
	message("BITS is not defined! Defaulting to ${BITS}")
endif()

#################################################
#												#
#				Program Constants				#
#												#
#################################################


# Location of the test file directory
# This must end with a slash
set( TEST_DIR test-files/ )

# Tests cases to run always
# These are assumed to be c files in the test file directory
set( TESTS_ANYBIT
	signals
	threads
	forks
	exec
	toy
)

# Test cases to only be run on 32 / 64 bit
# These are assumed to be c files in the test file directory
set ( TESTS_32BIT hacked_toy32 )
set ( TESTS_64BIT hacked_toy64 )


# Select options based of bit number
set ( TESTS ${TESTS_ANYBIT} ${TESTS_${BITS}BIT} )
set ( CMAKE_C_FLAGS "-m${BITS}" )

#################################################
#												#
#				Test file creation				#
#												#
#################################################


# Find packages
find_package ( Threads )

# Compile each test case
FOREACH ( FNAME ${TESTS} )
	add_executable ( ${FNAME} ${TEST_DIR}${FNAME}.c )
ENDFOREACH ( FNAME )

# Link required libraries
target_link_libraries ( threads Threads::Threads )
