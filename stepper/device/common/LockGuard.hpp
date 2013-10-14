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
#ifndef stepper_device_LockGuard_hpp
#define stepper_device_LockGuard_hpp

namespace stepper { namespace device {

/** Scoped lock. */
template <typename lock_type>
class LockGuard
{
public:
    explicit LockGuard(lock_type &mtx) : mtx_(mtx) {
        mtx_.lock();
    }

    ~LockGuard() {
        mtx_.unlock();

    }

private:
    lock_type &mtx_;
};

}}

#endif
