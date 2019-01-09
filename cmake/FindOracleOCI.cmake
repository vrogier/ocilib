set(ORACLE_OCI_FOUND "NO")
set(ORACLE_OCI_HOME "${ORACLE_HOME}")

if("${ORACLE_OCI_HOME}" STREQUAL "")
	set(ORACLE_OCI_HOME $ENV{ORACLE_HOME})
endif()

set(ORACLE_OCI_INCLUDE_LOCATIONS
	${ORACLE_OCI_HOME}/rdbms/public
	${ORACLE_OCI_HOME}/include
	${ORACLE_OCI_HOME}/xdk/include
	${ORACLE_OCI_HOME}/sdk/include)

set(ORACLE_OCI_LIB_LOCATIONS
	${ORACLE_OCI_HOME}
	${ORACLE_OCI_HOME}/lib)

find_path(ORACLE_OCI_INCLUDES
	oci.h
	${ORACLE_OCI_INCLUDE_LOCATIONS}
	NO_DEFAULT_PATH NO_CMAKE_ENVIRONMENT_PATH NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

find_library(ORACLE_OCI_LIBRARY_CLNTSH
	NAMES libclntsh.so.12.2 libclntsh.so.12.1 libclntsh.so.11.1 libclntsh clntsh oci
	PATHS ${ORACLE_OCI_LIB_LOCATIONS})

find_library(ORACLE_OCI_LIBRARY_CLNTSHCORE
	NAMES libclntshcore.so.12.2 libclntshcore.so.12.1 libclntshcore clntshcore 
	PATHS ${ORACLE_OCI_LIB_LOCATIONS})

set(ORACLE_OCI_LIBRARIES
	${ORACLE_OCI_LIBRARY_CLNTSH}
	${ORACLE_OCI_LIBRARY_CLNTSHCORE})

if(ORACLE_OCI_INCLUDES AND ORACLE_OCI_LIBRARIES)
	set(ORACLE_OCI_FOUND "YES")	

	message(STATUS "Found Oracle include directory: ${ORACLE_OCI_INCLUDES}")
	message(STATUS "Found Oracle libraries: ${ORACLE_OCI_LIBRARIES}")
elseif(${OracleOCI_FIND_REQUIRED})
	message(FATAL_ERROR "Could not find Oracle libraries.")
else()
	message(STATUS "Could not find Oracle libraries.")
endif()

