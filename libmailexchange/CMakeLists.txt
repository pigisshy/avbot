project(libpop3)
find_package(Boost 1.54 COMPONENTS regex system locale)

add_library(mailexchange STATIC pop3.cpp smtp.cpp)
target_link_libraries(mailexchange ${Boost_LIBRARIES})

if(ENABLE_STATIC)

	set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
	if( EXISTS /usr/lib/libicuuc.a AND EXISTS /usr/lib/libicudata.a)
		target_link_libraries(mailexchange /usr/lib/libicuuc.a /usr/lib/libicudata.a)
	else()
		find_package(ICU )
		if(ICU_FOUND)
			target_link_libraries(mailexchange ${ICU_LIBRARIES})
		endif()
	endif()
endif()

if(WIN32 AND WITH_ICONV)
	target_link_libraries(mailexchange -liconv -lcrypt32 -lws2_32 )
endif()

if( ENABLE_TEST )
add_definitions(-DDEBUG)
add_executable(pop3test pop3test.cpp)
target_link_libraries(pop3test mailexchange ${CMAKE_THREAD_LIBS_INIT} )

add_executable(smtptest smtptest.cpp)
target_link_libraries(smtptest mailexchange ${CMAKE_THREAD_LIBS_INIT} )

endif()