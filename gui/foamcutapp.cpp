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
#include <QString>

namespace {
// stepper settings
const QString xStepSizeName("stepper/xStepSize");
const QString yStepSizeName("stepper/yStepSize");
const QString xLengthName("stepper/xLength");
const QString yLengthName("stepper/yLength");
const QString frameSeparationName("stepper/frameSeparation");
const QString maxStepRateName("stepper/maxStepRate");
const QString portName("stepper/port");
const QString xLeftReverseName("stepper/xLeftReverse");
const QString xRightReverseName("stepper/xRightReverse");
const QString yLeftReverseName("stepper/yLeftReverse");
const QString yRightReverseName("stepper/yRightReverse");
const QString activeLowStepsName("stepper/activeLowSteps");
const QString cutSpeedName("stepper/cutSpeed");
const QString moveDistanceName("move/distance");
const QString moveFastName("move/fast");
}

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

double FoamcutApp::xStepSize()
{
	return settings_.value(xStepSizeName, .001).toDouble();
}

void FoamcutApp::xStepSize(double v)
{
	settings_.setValue(xStepSizeName, v);
}

double FoamcutApp::yStepSize()
{
	return settings_.value(yStepSizeName, .001).toDouble();
}

void FoamcutApp::yStepSize(double v)
{
	settings_.setValue(yStepSizeName, v);
}

double FoamcutApp::xLength()
{
	return settings_.value(xLengthName, 1.e9).toDouble();
}

void FoamcutApp::xLength(double v)
{
	settings_.setValue(xLengthName, v);
}

double FoamcutApp::yLength()
{
	return settings_.value(yLengthName, 1.e9).toDouble();
}

void FoamcutApp::yLength(double v)
{
	settings_.setValue(yLengthName, v);
}

double FoamcutApp::frameSeparation()
{
	return settings_.value(frameSeparationName, 30.).toDouble();
}

void FoamcutApp::frameSeparation(double v)
{
	settings_.setValue(frameSeparationName, v);
}

int FoamcutApp::maxStepRate()
{
	return settings_.value(maxStepRateName, 30.).toInt();
}

void FoamcutApp::maxStepRate(int v)
{
	settings_.setValue(maxStepRateName, v);
}

QString FoamcutApp::port()
{
	return settings_.value(portName, "Simulator").toString();
}

void FoamcutApp::port(const QString &v)
{
	settings_.setValue(portName, v);
}


bool FoamcutApp::xLeftReverse()
{
	return settings_.value(xLeftReverseName, false).toBool();
}

void FoamcutApp::xLeftReverse(bool v)
{
	settings_.setValue(xLeftReverseName, v);
}

bool FoamcutApp::xRightReverse()
{
	return settings_.value(xRightReverseName, false).toBool();
}

void FoamcutApp::xRightReverse(bool v)
{
	settings_.setValue(xRightReverseName, v);
}

bool FoamcutApp::yLeftReverse()
{
	return settings_.value(yLeftReverseName, false).toBool();
}

void FoamcutApp::yLeftReverse(bool v)
{
	settings_.setValue(yLeftReverseName, v);
}

bool FoamcutApp::yRightReverse()
{
	return settings_.value(yRightReverseName, false).toBool();
}

void FoamcutApp::yRightReverse(bool v)
{
	settings_.setValue(yRightReverseName, v);
}

bool FoamcutApp::activeLowSteps()
{
	return settings_.value(activeLowStepsName, false).toBool();
}

void FoamcutApp::activeLowSteps(bool v)
{
	settings_.setValue(activeLowStepsName, v);
}

foamcut::StepperInfo FoamcutApp::stepperInfo()
{
	foamcut::StepperInfo si;
	si.xStepSize_ = xStepSize();
	si.yStepSize_ = yStepSize();
	si.xLength_ = xLength();
	si.yLength_ = yLength();
	si.frameSeparation_ = frameSeparation();

	bool invertStep = activeLowSteps();
	si.invertMask_.xStep(invertStep);
	si.invertMask_.yStep(invertStep);
	si.invertMask_.zStep(invertStep);
	si.invertMask_.uStep(invertStep);
	si.invertMask_.xDir(xLeftReverse());
	si.invertMask_.yDir(yLeftReverse());
	si.invertMask_.zDir(xRightReverse());
	si.invertMask_.uDir(yRightReverse());

	return si;
}

double FoamcutApp::cutSpeed()
{
	return settings_.value(cutSpeedName, .05).toDouble();
}

void FoamcutApp::cutSpeed(double v)
{
	settings_.setValue(cutSpeedName, v);
}

double FoamcutApp::moveDistance()
{
	return settings_.value(moveDistanceName, .1).toDouble();
}

void FoamcutApp::moveDistance(double v)
{
	settings_.setValue(moveDistanceName, v);
}

bool FoamcutApp::moveFast()
{
	return settings_.value(moveFastName, true).toBool();
}

void FoamcutApp::moveFast(bool v)
{
	settings_.setValue(moveFastName, v);
}


