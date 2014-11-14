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
#ifndef foamcutapp_hpp
#define foamcutapp_hpp

#include <QApplication>
#include <memory>
#include <Host.hpp>

namespace stepper {
class Host;
}

///\todo fold settings into this class

class FoamcutApp : public QApplication
{
public:
	explicit FoamcutApp(int &argc, char **argv);
	~FoamcutApp();

	static FoamcutApp *instance();

	stepper::Host &host() { return *host_.get(); }

private:
	std::unique_ptr<stepper::Host> host_;
};

#endif
