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
#ifndef foamcut_settings_hpp
#define foamcut_settings_hpp

#include <qsettings.h>
#include <qstring.h>
#include <stepper_info.hpp>

namespace foamcut {

/**
 * Class to manage persistent storage of configuration settings for the 
 * foamcutter hardware and application.
 * Settings in the "stepper" group are specific to the foamcutter hardware.
 */
class Settings
{
public:
	static const QString applicationName;
	static const QString organizationName;
	static const QString xStepSizeName;
	static const QString yStepSizeName;
	static const QString xLengthName;
	static const QString yLengthName;
	static const QString frameSeparationName;
	static const QString maxStepRateName;
	static const QString portName;
	static const QString xLeftReverseName;
	static const QString xRightReverseName;
	static const QString yLeftReverseName;
	static const QString yRightReverseName;
	static const QString activeLowStepsName;

	static double xStepSize();
	static void xStepSize(double v);

	static double yStepSize();
	static void yStepSize(double v);

	static double xLength();
	static void xLength(double v);

	static double yLength();
	static void yLength(double v);

	static double frameSeparation();
	static void frameSeparation(double v);

	static int maxStepRate();
	static void maxStepRate(int v);

	static QString port();
	static void port(const QString &v);

	static bool xLeftReverse();
	static void xLeftReverse(bool v);

	static bool xRightReverse();
	static void xRightReverse(bool v);

	static bool yLeftReverse();
	static void yLeftReverse(bool v);

	static bool yRightReverse();
	static void yRightReverse(bool v);

	static bool activeLowSteps();
	static void activeLowSteps(bool v);

	static StepperInfo stepperInfo();

	static QSettings &getQSettings();

};

}

#endif
