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
#include <QDoubleSpinBox>

class LogDoubleSpinBox : public QDoubleSpinBox
{
public:
	LogDoubleSpinBox(QWidget *parent);
	void stepBy(int steps);

private:
	static double next125(double v);
	static double prev125(double v);
};
