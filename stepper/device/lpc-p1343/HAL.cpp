#include "MemoryAllocator.hpp"
#include <Stepper.hpp>
#include <HAL.hpp>
#include <core/gpio/gpio.h>
#include <lpc134x.h>
#include "step_timer.h"
#include <core/usbcdc/cdcuser.h>


namespace stepper { namespace device {
    
    enum {
        X_STEP_PORT    = 3,
        X_STEP_PIN     = 0,
        X_DIR_PORT     = 3,
        X_DIR_PIN      = 1,
        X_FWD_LIM_PORT = 2,
        X_FWD_LIM_PIN  = 0,
        X_REV_LIM_PORT = 2,
        X_REV_LIM_PIN  = 1,

        Y_STEP_PORT    = 3,
        Y_STEP_PIN     = 2,
        Y_DIR_PORT     = 3,
        Y_DIR_PIN      = 3,
        Y_FWD_LIM_PORT = 2,
        Y_FWD_LIM_PIN  = 2,
        Y_REV_LIM_PORT = 2,
        Y_REV_LIM_PIN  = 3,

        Z_STEP_PORT    = 2,
        Z_STEP_PIN     = 4,
        Z_DIR_PORT     = 2,
        Z_DIR_PIN      = 5,
        Z_FWD_LIM_PORT = 2,
        Z_FWD_LIM_PIN  = 8,
        Z_REV_LIM_PORT = 2,
        Z_REV_LIM_PIN  = 9,

        U_STEP_PORT    = 2,
        U_STEP_PIN     = 6,
        U_DIR_PORT     = 2,
        U_DIR_PIN      = 7,
        U_FWD_LIM_PORT = 2,
        U_FWD_LIM_PIN  = 10,
        U_REV_LIM_PORT = 2,
        U_REV_LIM_PIN  = 11
    };        
        
MemoryAllocator &MemoryAllocator::instance()
{
	static MemoryAllocator ma;
	return ma;
}

namespace HAL {
    Message *rxMsg_ = NULL;
    uint16_t rxPos_ = 0;
};

void HAL::initialize()
{
    // X axis i/o pin setup
    gpioSetPinFunction(X_STEP_PORT,    X_STEP_PIN,    GPIO_OUTPUT_PIN);
    gpioSetPinFunction(X_DIR_PORT,     X_DIR_PIN,     GPIO_OUTPUT_PIN);
    gpioSetPinFunction(X_FWD_LIM_PORT, X_FWD_LIM_PIN, GPIO_INPUT_PULLUP_PIN);
    gpioSetPinFunction(X_REV_LIM_PORT, X_REV_LIM_PIN, GPIO_INPUT_PULLUP_PIN);
    
    // Y axis i/o pin setup
    gpioSetPinFunction(Y_STEP_PORT,    Y_STEP_PIN,    GPIO_OUTPUT_PIN);
    gpioSetPinFunction(Y_DIR_PORT,     Y_DIR_PIN,     GPIO_OUTPUT_PIN);
    gpioSetPinFunction(Y_FWD_LIM_PORT, Y_FWD_LIM_PIN, GPIO_INPUT_PULLUP_PIN);
    gpioSetPinFunction(Y_REV_LIM_PORT, Y_REV_LIM_PIN, GPIO_INPUT_PULLUP_PIN);
    
    // Z axis i/o pin setup
    gpioSetPinFunction(Z_STEP_PORT,    Z_STEP_PIN,    GPIO_OUTPUT_PIN);
    gpioSetPinFunction(Z_DIR_PORT,     Z_DIR_PIN,     GPIO_OUTPUT_PIN);
    gpioSetPinFunction(Z_FWD_LIM_PORT, Z_FWD_LIM_PIN, GPIO_INPUT_PULLUP_PIN);
    gpioSetPinFunction(Z_REV_LIM_PORT, Z_REV_LIM_PIN, GPIO_INPUT_PULLUP_PIN);
    
    // U axis i/o pin setup
    gpioSetPinFunction(U_STEP_PORT,    U_STEP_PIN,    GPIO_OUTPUT_PIN);
    gpioSetPinFunction(U_DIR_PORT,     U_DIR_PIN,     GPIO_OUTPUT_PIN);
    gpioSetPinFunction(U_FWD_LIM_PORT, U_FWD_LIM_PIN, GPIO_INPUT_PULLUP_PIN);
    gpioSetPinFunction(U_REV_LIM_PORT, U_REV_LIM_PIN, GPIO_INPUT_PULLUP_PIN);
    
    step_timer_init();
    
    rxMsg_ = NULL;
    rxPos_ = 0;
}

void HAL::setStepDirBits(const StepDir &s)
{
    gpioSetPin(X_DIR_PORT, X_DIR_PIN, s.xDir());
    gpioSetPin(Y_DIR_PORT, Y_DIR_PIN, s.yDir());
    gpioSetPin(Z_DIR_PORT, Z_DIR_PIN, s.zDir());
    gpioSetPin(U_DIR_PORT, U_DIR_PIN, s.uDir());

    gpioSetPin(X_STEP_PORT, X_STEP_PIN, s.xStep());
    gpioSetPin(Y_STEP_PORT, Y_STEP_PIN, s.yStep());
    gpioSetPin(Z_STEP_PORT, Z_STEP_PIN, s.zStep());
    gpioSetPin(U_STEP_PORT, U_STEP_PIN, s.uStep());
}

LimitSwitches HAL::readLimitSwitches()
{
    LimitSwitches ret;
    ret.xFwd(gpioGetPin(X_FWD_LIM_PORT, X_FWD_LIM_PIN));
    ret.xRev(gpioGetPin(X_REV_LIM_PORT, X_REV_LIM_PIN));

    ret.yFwd(gpioGetPin(Y_FWD_LIM_PORT, Y_FWD_LIM_PIN));
    ret.yRev(gpioGetPin(Y_REV_LIM_PORT, Y_REV_LIM_PIN));

    ret.zFwd(gpioGetPin(Z_FWD_LIM_PORT, Z_FWD_LIM_PIN));
    ret.zRev(gpioGetPin(Z_REV_LIM_PORT, Z_REV_LIM_PIN));

    ret.uFwd(gpioGetPin(U_FWD_LIM_PORT, U_FWD_LIM_PIN));
    ret.uRev(gpioGetPin(U_REV_LIM_PORT, U_REV_LIM_PIN));

	return ret;
}

ErrorCode HAL::sendMessage(Message *m, Message::Priority priority)
{
    CDC_BlockingWriteInEp(m->transmitStart(), m->transmitSize());
    ErrorCode ec;
    return ec;
}

Message *HAL::receiveMessage()
{
    if (rxMsg_ == NULL) {
        rxMsg_ = Message::alloc(Message::maxPayloadCapacity());
        if (rxMsg_ == NULL) return NULL;
    }
    
    uint8_t r;
    uint8_t *p = rxMsg_->transmitStart();
    while (rxPos_ < sizeof(MessageHeader)) {
        r = CDC_ReadByte(&p[rxPos_]);
        if (r) ++rxPos_;
        else return NULL;
    }
    
    while (rxPos_ < rxMsg_->transmitSize()) {
        r = CDC_ReadByte(&p[rxPos_]);
        if (r) ++rxPos_;
        else return NULL;
    }
    
    if (rxPos_ == rxMsg_->transmitSize()) {
        Message *ret = rxMsg_;
        rxMsg_ = NULL;
        return ret;
    }
    return NULL;
}

void HAL::startTimer(uint32_t period)
{
    // timer assumes 5 uSec ticks
    step_timer_start(5*period);
}

void HAL::stopTimer()
{
    step_timer_stop();
}

}}

extern "C" void TIMER32_0_IRQHandler(void)
{
    uint8_t v;
    /* clear the interrupt flag */
    TMR_TMR32B0IR = TMR_TMR32B0IR_MR0;

#ifdef CFG_FOAMCUT
    stepper::device::Stepper::instance().onTimerExpired();

#else
    v = gpioGetPin(2, 7);
    v = (v == 0) ? 1 : 0;
    gpioSetPin(2, 7, v);
#endif
}

