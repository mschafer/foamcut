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
#ifndef stepper_sim_SimComm_hpp
#define stepper_sim_SimComm_hpp

#include <Communicator.hpp>

namespace stepper { namespace device {

class SimComm : public Communicator
{
public:
	SimComm();
	virtual ~SimComm();

	void initialize();
	void operator()();
	bool connected() const;

protected:
	void startSending();
};

}}

#endif
