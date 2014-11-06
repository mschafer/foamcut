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

namespace stepper {
class Host;
}

class FoamcutApp : public QApplication
{
public:
	explicit FoamcutApp(int &argc, char **argv);
	~FoamcutApp();

private:
	std::unique_ptr<stepper::Host> host_;
};

#endif
