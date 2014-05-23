#ifndef stepper_device_CommunicatorBase_hpp
#define stepper_device_CommunicatorBase_hpp

#include <stddef.h>
#include "CommDictionary.hpp"
#include "Lock.hpp"
#include "IDevice.hpp"

namespace stepper { namespace device {

/**
 * Base class for target Communicator.
 * Provides handling of incoming messages.
 * Uses CRTP for static polymorphism.
 */
template <class Derived>
class CommunicatorBase
{
public:
	enum Priority {
		HIGH_PRIORITY = 0,
		MEDIUM_PRIORITY = 1,
		LOW_PRIORITY = 2,
	};

    enum TestState {
        NO_TEST,
        IN_TEST,
        OUT_TEST
    };

    CommunicatorBase() : testCount_(0), testSize_(0), testState_(NO_TEST), connected_(false) {}
    ~CommunicatorBase() {}

    bool connected() const { return connected_; }

    /**
     * Send a message to the host.
     * Communicator takes ownership of message and will delete it when finished.
     * \return true if the message can be sent and false if there is a problem.
     * If false is returned, then ownership of the message reverts to the callee.
     * The Communicator will not delete the message.
     */
    bool sendMessage(Message *message, Priority priority=LOW_PRIORITY) {
        return static_cast<Derived*>(this)->sendMessage(message, priority);
    }

    /** Runs the background function once. */
    void operator()()
    {
        switch (testState_) {
        case NO_TEST:
            break;

        case IN_TEST:
            if (testCount_ > 0) {
                Message *test = Message::alloc(testSize_);
                if (test != NULL) {
                    new (test) TestDataMsg(testSize_);
                    bool s = sendMessage(test);
                    if (!s) {
                        delete test;
                    } else {
                        --testCount_;
                    }
                }
            }
            if (testCount_ == 0) {
                TestConcludedMsg *tcm = new TestConcludedMsg();
                if (tcm != NULL) {
                    bool s = sendMessage(tcm);
                    if (!s) {
                        delete tcm;
                    } else {
                        testState_ = NO_TEST;
                    }
                }
            }
            break;

        case OUT_TEST:
            break;

        default:
            break;
        }
    }

protected:
    uint16_t testCount_, testSize_;
    TestState testState_;
    volatile bool connected_;

    /** Handle messages addressed to the Communicator. */
    void handleCSMessage(Message *message)
    {
        switch(message->header().function_) {

        case PingMsg::FUNCTION:
            new (message) PingResponseMsg;
            sendMessage(message);
            break;

        case PingResponseMsg::FUNCTION:
            delete message;
            break;

        case ConnectMsg::FUNCTION:
            new (message) ConnectResponseMsg;
            sendMessage(message);
            connected_ = true;
            /// \todo Application::connectionChanged(connected_);
            break;

        case HeartbeatMsg::FUNCTION:
            {
                HeartbeatResponseMsg *hrm = new (message) HeartbeatResponseMsg;
                assert(hrm->payloadCapacity() > HeartbeatResponseMsg::PAYLOAD_SIZE);
                hrm->statusFlags_ = getStatusFlags();
                getStatusFlags().clear();
                sendMessage(message);
                break;
            }

        case InTestStartMsg::FUNCTION:
            {
                InTestStartMsg *itsm = static_cast<InTestStartMsg*>(message);
                testCount_ = itsm->testMsgCount_;
                testSize_ = itsm->testMsgSize_;
                testState_ = IN_TEST;
                delete message;
                break;
            }

        case OutTestStartMsg::FUNCTION:
            {
                OutTestStartMsg *otsm = static_cast<OutTestStartMsg*>(message);
                testCount_ = otsm->testMsgCount_;
                testSize_ = otsm->testMsgSize_;
                testState_ = OUT_TEST;
                delete message;
                break;
            }

        case TestDataMsg::FUNCTION:
            if (testState_ == OUT_TEST) {
                if (message->payloadSize() == testSize_) {
                    --testCount_;
                }
            }
            delete message;
            break;

        case TestConcludedMsg::FUNCTION:
            {
                if (testState_ == OUT_TEST) {
                    OutTestResultMsg *otrm = new (message) OutTestResultMsg();
                    if (testCount_ != 0) otrm->result_ = 0xFF;
                    bool s = sendMessage(otrm, HIGH_PRIORITY);
                    if (!s) {
                        delete otrm;
                    } else {
                        testState_ = NO_TEST;
                    }
                }
            }
            break;

        default:
            getStatusFlags().set(StatusFlags::UNRECOGNIZED_MSG);
            delete message;
            break;
        }
    }

private:
    /** Don't implement, this class is not copyable. */
	CommunicatorBase(const CommunicatorBase &cpy);

	/** Don't implement, this class is not assignable. */
	CommunicatorBase &operator=(const CommunicatorBase &cpy);
};

}}
#endif
