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

#include "settings.hpp"
#include <qapplication.h>
#include <StepDir.hpp>

namespace foamcut {

const QString Settings::xStepSizeName("stepper/xStepSize");
const QString Settings::yStepSizeName("stepper/yStepSize");
const QString Settings::xLengthName("stepper/xLength");
const QString Settings::yLengthName("stepper/yLength");
const QString Settings::frameSeparationName("stepper/frameSeparation");
const QString Settings::maxStepRateName("stepper/maxStepRate");
const QString Settings::portName("stepper/port");
const QString Settings::xLeftReverseName("stepper/xLeftReverse");
const QString Settings::xRightReverseName("stepper/xRightReverse");
const QString Settings::yLeftReverseName("stepper/yLeftReverse");
const QString Settings::yRightReverseName("stepper/yRightReverse");
const QString Settings::activeLowStepsName("stepper/activeLowSteps");

double Settings::xStepSize()
{
	return getQSettings().value(xStepSizeName, .001).toDouble();
}

void Settings::xStepSize(double v)
{
	getQSettings().setValue(xStepSizeName, v);
}

double Settings::yStepSize()
{
	return getQSettings().value(yStepSizeName, .001).toDouble();
}

void Settings::yStepSize(double v)
{
	getQSettings().setValue(yStepSizeName, v);
}

double Settings::xLength()
{
	return getQSettings().value(xLengthName, 1.e9).toDouble();
}

void Settings::xLength(double v)
{
	getQSettings().setValue(xLengthName, v);
}

double Settings::yLength()
{
	return getQSettings().value(yLengthName, 1.e9).toDouble();
}

void Settings::yLength(double v)
{
	getQSettings().setValue(yLengthName, v);
}

double Settings::frameSeparation()
{
	return getQSettings().value(frameSeparationName, 30.).toDouble();
}

void Settings::frameSeparation(double v)
{
	getQSettings().setValue(frameSeparationName, v);
}

int Settings::maxStepRate()
{
	return getQSettings().value(maxStepRateName, 30.).toInt();
}

void Settings::maxStepRate(int v)
{
	getQSettings().setValue(maxStepRateName, v);
}

QString Settings::port()
{
	return getQSettings().value(portName, "Simulator").toString();
}

void Settings::port(const QString &v)
{
	getQSettings().setValue(portName, v);
}


bool Settings::xLeftReverse()
{
	return getQSettings().value(xLeftReverseName, false).toBool();
}

void Settings::xLeftReverse(bool v)
{
	getQSettings().setValue(xLeftReverseName, v);
}

bool Settings::xRightReverse()
{
	return getQSettings().value(xRightReverseName, false).toBool();
}

void Settings::xRightReverse(bool v)
{
	getQSettings().setValue(xRightReverseName, v);
}

bool Settings::yLeftReverse()
{
	return getQSettings().value(yLeftReverseName, false).toBool();
}

void Settings::yLeftReverse(bool v)
{
	getQSettings().setValue(yLeftReverseName, v);
}

bool Settings::yRightReverse()
{
	return getQSettings().value(yRightReverseName, false).toBool();
}

void Settings::yRightReverse(bool v)
{
	getQSettings().setValue(yRightReverseName, v);
}

bool Settings::activeLowSteps()
{
	return getQSettings().value(activeLowStepsName, false).toBool();
}

void Settings::activeLowSteps(bool v)
{
	getQSettings().setValue(activeLowStepsName, v);
}

StepperInfo Settings::stepperInfo()
{
	StepperInfo si;
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

QSettings &Settings::getQSettings() {
	static QSettings s;
	return s;
}

}