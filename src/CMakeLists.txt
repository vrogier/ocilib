set(OCILIB_SOURCES
	array.c
	bind.c
	callback.c
	connection.c
	define.c
	exception.c
	handle.c
	iterator.c
	lob.c
	mutex.c
	resultset.c
	string.c
	timestamp.c
	collection.c
	pool.c
	element.c
	file.c
	hash.c
	library.c
	long.c
	number.c
	typeinfo.c
	thread.c
	transaction.c
	column.c
	date.c
	error.c
	format.c
	interval.c
	list.c
	memory.c
	object.c
	statement.c
	ref.c
	threadkey.c
	dirpath.c
	event.c
	subscription.c
	agent.c
	dequeue.c
	enqueue.c
	msg.c
	queue.c)

set(OCILIB_INCLUDES
	oci_api.h
	oci_defs.h
	oci_import.h
	ocilib_checks.h
	ocilib_defs.h
	ocilib_internal.h
	ocilib_types.h
	oci_loader.h
	oci_types.h)

add_library(ocilib_shared SHARED ${OCILIB_SOURCES} ${OCILIB_INCLUDES})
target_link_libraries(ocilib_shared ${ORACLE_OCI_LIBRARIES})
set_target_properties(ocilib_shared PROPERTIES OUTPUT_NAME "ocilib")

add_library(ocilib_static STATIC ${OCILIB_SOURCES} ${OCILIB_INCLUDES})
target_link_libraries(ocilib_static ${ORACLE_OCI_LIBRARIES})
set_target_properties(ocilib_static PROPERTIES OUTPUT_NAME "ocilib")

install(TARGETS
	ocilib_shared
	ocilib_static
	DESTINATION "lib")
