#include "logdoublespinbox.h"
#include <QDebug>
#include <cmath>

LogDoubleSpinBox::LogDoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
}

void LogDoubleSpinBox::stepBy(int steps) {
	double v = value();
	while (steps < 0 && v > minimum()) {
		v = prev125(v);
		++steps;
	}
	while (steps > 0 && v < maximum()) {
		v = next125(v);
		--steps;
	}
	setValue(v);
}

double LogDoubleSpinBox::next125(double v) {
	double logv = log10(v);
	double f = floor(logv);
	double nlogv = logv - f;
	double nv = powf(10., nlogv);
	double next;
	if (nv < 1.5) next = 2.;
	else if (nv < 2.5) next = 5.;
	else if (nv < 8) next = 10.;
	else next = 20.;
	next = next * powf(10., f);
	return next;
}

double LogDoubleSpinBox::prev125(double v) {
	double logv = log10(v);
	double f = floor(logv);
	double nlogv = logv - f;
	double nv = powf(10., nlogv);
	double next;
	if (nv > 8) next = 5.;
	else if (nv > 3) next = 2.;
	else if (nv > 1.5) next = 1.;
	else next = .5;
	next = next * powf(10., f);
	return next;
}
