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
#include "importdat.hpp"
#include <fstream>
#include <dat_file.hpp>
#include <airfoil.hpp>
#include <QDebug>

ImportDat::ImportDat(const QString &name, QObject *parent) : QObject(parent)
{
	setObjectName(name);
}

ImportDat::~ImportDat()
{

}

void ImportDat::setFileName(const QString &n)
{
	fileName_ = n;
	shape_.reset();
}

void ImportDat::setDatName(const QString &n)
{
	datName_ = n;
	shape_.reset();
}

void ImportDat::setIsAirfoil(bool v) 
{
	isAirfoil_ = v; 
	shape_.reset();
}

void ImportDat::setLeLoop(bool v)
{
	leLoop_ = v; 
	shape_.reset();
}

void ImportDat::setChord(double v)
{
	chord_ = v;
	shape_.reset();
}

void ImportDat::setAlpha(double v)
{
	alpha_ = v;
	shape_.reset();
}

void ImportDat::import()
{
	std::ifstream in(fileName_.toLatin1(), std::ifstream::in);
	auto datFile = foamcut::DatFile::read(in);
	if (isAirfoil_) {
		foamcut::Airfoil::handle airfoil(new foamcut::Airfoil(datFile, chord_, alpha_, leLoop_));
		shape_ = airfoil->shape();
	} else {
		shape_.reset(new foamcut::Shape(*datFile));
	}
}
