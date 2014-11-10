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
#include "foamcutapp.hpp"
#include <Host.hpp>

FoamcutApp::FoamcutApp(int &argc, char **argv) : QApplication(argc, argv)
{
	host_.reset(new stepper::Host());
	host_->connectToSimulator();
}

FoamcutApp::~FoamcutApp()
{
}

FoamcutApp *FoamcutApp::instance()
{
	return dynamic_cast<FoamcutApp*>(QCoreApplication::instance());
}