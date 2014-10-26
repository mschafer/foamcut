#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <vector>
#include <cfloat>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "Port.hpp"
#include "Synchronizer.hpp"
#include "SerialProtocol.hpp"

BOOST_AUTO_TEST_CASE( simple_port_test )
{
    PortPair pp(5);
    Port &A = pp.portA();
    Port &B = pp.portB();

    BOOST_CHECK(A.recvAvailable() == 0);
    BOOST_CHECK(B.recvAvailable() == 0);
    BOOST_CHECK(A.sendAvailable() > 0);
    BOOST_CHECK(B.sendAvailable() > 0);

    uint8_t data[] = {'a', 'b', 'c', 'd'};

    size_t s = sizeof(data);
    BOOST_CHECK(A.send(data, s) == Port::PORT_SUCCESS);
    BOOST_CHECK(s == sizeof(data));
    BOOST_CHECK(B.recvAvailable() == sizeof(data));
    BOOST_CHECK(A.send(data, s) == Port::PORT_SUCCESS);
    BOOST_CHECK(s == 1);
    BOOST_CHECK(B.recvAvailable() == 5);
}

BOOST_AUTO_TEST_CASE( sync_test)
{
    PortPair pp(2048);
    Port &A = pp.portA();
    Port &B = pp.portB();

    SerialProtocol spA(A);
    SerialProtocol spB(B);

    spA.run();
    spB.run();
    spA.run();
    spB.run();
    spA.run();
    spB.run();

    BOOST_CHECK(spA.state() == SerialProtocol::SYNCHRONIZED);
    BOOST_CHECK(spB.state() == SerialProtocol::SYNCHRONIZED);
}
