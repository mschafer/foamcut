/*
 * (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Test Kernel

// use dynamic boost test library on non-Windows arches
#ifdef _WIN32
#include "debug_stream.hpp"
#endif


#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <vector>
#include <cfloat>
#include <boost/bind.hpp>
#include <boost/function.hpp>

int add( int i, int j ) { return i+j; }

struct test_output_redirect {
	test_output_redirect() { 

#ifdef _WIN32
		boost::unit_test::unit_test_log.set_stream(test_log_);
#endif

		boost::unit_test::unit_test_log.set_threshold_level( boost::unit_test::log_messages );
	}

	~test_output_redirect() { boost::unit_test::unit_test_log.set_stream( std::cout ); }

#ifdef _WIN32
	dostream test_log_;
#endif
};

BOOST_GLOBAL_FIXTURE ( test_output_redirect );

BOOST_AUTO_TEST_CASE( hello_test )
{
    // seven ways to detect and report the same error:
    BOOST_CHECK( add( 2,2 ) == 4 );        // #1 continues on error
    
    BOOST_REQUIRE( add( 2,2 ) == 4 );      // #2 throws on error
    
    if( add( 2,2 ) != 4 )
        BOOST_ERROR( "Ouch..." );            // #3 continues on error
    
    if( add( 2,2 ) != 4 )
        BOOST_FAIL( "Ouch..." );             // #4 throws on error
    
    if( add( 2,2 ) != 4 ) throw "Ouch..."; // #5 throws on error
    
    BOOST_CHECK_MESSAGE( add( 2,2 ) == 4,  // #6 continues on error
                        "add(..) result: " << add( 2,2 ) );
    
    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );	  // #7 continues on error
}
