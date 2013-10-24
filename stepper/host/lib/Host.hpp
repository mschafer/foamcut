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
#ifndef stepper_Host_hpp
#define stepper_Host_hpp

#include <memory>

namespace stepper {

class TCPLink;

namespace device {
class Simulator;
}

class Host
{
public:
	Host();
	~Host();

	bool connectToSimulator();


private:
	std::unique_ptr<device::Simulator> sim_;
	std::unique_ptr<TCPLink> link_;
};

}

#endif
