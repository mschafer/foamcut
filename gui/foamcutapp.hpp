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
#include <QSettings>
#include <memory>
#include <Host.hpp>
#include <stepper_info.hpp>

///\todo fold settings into this class

class FoamcutApp : public QApplication
{
public:
	explicit FoamcutApp(int &argc, char **argv);
	~FoamcutApp();

	static FoamcutApp *instance();

	stepper::Host &host() { return *host_.get(); }

	double xStepSize();
	void xStepSize(double v);

	double yStepSize();
	void yStepSize(double v);

	double xLength();
	void xLength(double v);

	double yLength();
	void yLength(double v);

	double frameSeparation();
	void frameSeparation(double v);

	int maxStepRate();
	void maxStepRate(int v);

	QString port();
	void port(const QString &v);

	bool xLeftReverse();
	void xLeftReverse(bool v);

	bool xRightReverse();
	void xRightReverse(bool v);

	bool yLeftReverse();
	void yLeftReverse(bool v);

	bool yRightReverse();
	void yRightReverse(bool v);

	bool activeLowSteps();
	void activeLowSteps(bool v);

	double cutSpeed();
	void cutSpeed(double v);

	double moveDistance();
	void moveDistance(double v);

	bool moveFast();
	void moveFast(bool v);

	foamcut::StepperInfo stepperInfo();

private:
	std::unique_ptr<stepper::Host> host_;
	QSettings settings_;

};

#endif
