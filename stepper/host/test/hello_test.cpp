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
#define BOOST_TEST_MODULE Test Stepper

// use debug stream to print to visual console on windows
#ifdef _WIN32
#include "debug_stream.hpp"
#endif

#include <iostream>
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

template<size_t N>
class Aggregator
{
public:
	typedef std::pair<uint8_t *, size_t> Range;

	Aggregator() : in_(0), out_(0) {}

	bool empty() const { return (in_ == out_); }

	bool full() const {
		return (in_+1 == out_ || (out_ == 0 && in_ == N));
	}

	void clear() { in_ = out_ = 0; }

	size_t count() const {
		size_t r = in_ + N - out_;
		r = r<N ? r : r-N;
		return r;
	}

	size_t space() const { return N - count() - 1; }

	bool put(Range range) {
		if (space() < range.second) return false;

		// copy to the end of the buffer
		size_t end = (std::min)(range.second, N-in_);
		std::copy(range.first, range.first+end, &buff_[in_]);
		if (end == range.second) {
			size_t nin = in_ + end;
			in_ = nin<N ? nin : 0;
		} else {
			// wrap
			std::copy(range.first+end, range.first+range.second, buff_);
			in_ = range.second - end;
		}
		return true;
	}

	Range range() {
		Range r;
		r.first = buff_ + out_;
		if (out_ <= in_) {
			r.second = in_ - out_;
		} else {
			r.second = N - out_;
		}
		return r;
	}

	void release(size_t count) {
		///\< todo need to check count < count()
		size_t nout = out_ + count;
		out_ = nout<N ? nout : nout-N;
	}

private:
	size_t in_;
	size_t out_;
	uint8_t buff_[N];
};


BOOST_AUTO_TEST_CASE( agg_test )
{
	const size_t N = 20;
	typedef Aggregator<N> A;
	A ag;
	BOOST_CHECK(ag.empty() == true);
	BOOST_CHECK(ag.full() == false);
	BOOST_CHECK(ag.space() == N-1);

	uint8_t *p = (uint8_t*)malloc(30);
	for (int i=0; i<30; ++i) {
		p[i] = i;
	}

	A::Range r(p, 30);
	BOOST_CHECK(ag.put(r) == false);

	r.second = 10;
	BOOST_CHECK(ag.put(r) == true);
	BOOST_CHECK(ag.count() == 10);
	BOOST_CHECK(ag.space() == 9);

	r = ag.range();
	uint8_t *buff = r.first;
	BOOST_CHECK(*r.first == 0 && r.first[r.second-1] == r.second-1);
	BOOST_CHECK(r.second == 10);

	ag.release(5);
	BOOST_CHECK(ag.count() == 5);
	r = ag.range();
	BOOST_CHECK(*r.first == 5 && r.second == 5);

	r.first = p+10;
	r.second = 12;
	BOOST_CHECK(ag.put(r) == true);
	BOOST_CHECK_MESSAGE(ag.count() == 17, "count " << ag.count());

	A::Range r2 = ag.range();
	BOOST_CHECK_MESSAGE(r2.second == 15, "count " << r2.second);
	ag.release(r2.second);
	BOOST_CHECK(ag.count() == 2);

	r2 = ag.range();
	BOOST_CHECK(r2.first == buff && r2.second == 2);
	ag.release(r2.second);
	r2 = ag.range();
	BOOST_CHECK(r2.second == 0);

	delete p;
}
