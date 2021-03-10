cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ocilib)

set(VERSION_MAJOR 4)
set(VERSION_MINOR 6)
set(VERSION_PATCH 0)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME "ocilib")
set(CPACK_PACKAGE_CONTACT "")
set(CPACK_PACKAGE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ocilib ${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://vrogier.github.io/ocilib/")
set(CPACK_STRIP_FILES TRUE)

find_package(OracleOCI REQUIRED)
include(CPack)
include(CheckIncludeFiles)

check_include_files(dlfcn.h HAVE_DLFCN_H)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmakein ${CMAKE_BINARY_DIR}/config.h)

add_definitions(-DHAVE_CONFIG_H)

include_directories(
	${ORACLE_OCI_INCLUDES}
	${CMAKE_BINARY_DIR}
	${CMAKE_SOURCE_DIR}/include)

install(FILES
	${CMAKE_SOURCE_DIR}/include/ocilib.h
	${CMAKE_SOURCE_DIR}/include/ocilib.hpp
	${CMAKE_SOURCE_DIR}/include/ocilib_core.hpp
	${CMAKE_SOURCE_DIR}/include/ocilib_impl.hpp
	DESTINATION include)

add_subdirectory(src)

