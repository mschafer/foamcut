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
#include <string>
#include <cmath>
#include <stdexcept>
#include <math.h>
#include "dat_file.hpp"

std::ostream &operator<<(std::ostream &os, const foamcut::DatFile &datfile) {
	size_t n = datfile.x().size();
	for (size_t i=0; i<n; i++) {
		os << datfile.x()[i] << "\t" << datfile.y()[i] << std::endl;
	}
	return os;
}

namespace foamcut {

DatFile::DatFile()
{

}

DatFile::~DatFile()
{

}

DatFile::handle
DatFile::read(std::istream &in)
{
	DatFile::handle ret(new DatFile());
	std::vector<double> xin;
	std::vector<double> yin;
	double x,y;

	// first line can be x y coords or a name
	auto start = in.tellg();
	in >> x;
	if (in.good()) in >> y;
	if (in.good()) {
		xin.push_back(x);
		yin.push_back(y);
	} else {
		in.clear();
		in.seekg(start);
		std::getline(in, ret->name_);
	}

	in >> x >> y;
	while (in.good() && !in.eof()) {
		xin.push_back(x);
		yin.push_back(y);
		in >> x >> y;
	}

	if (xin.size() != yin.size() || xin.size() < 2) {
		throw (std::range_error("DatFile::read wrong number of xy points"));
	}

	ret->x_.swap(xin);
	ret->y_.swap(yin);
	ret->origin();

	return ret;
}


void
DatFile::origin()
{
	double x0 = x_[0];
	double y0 = y_[0];
	for (size_t i=0; i<x_.size(); i++) {
		x_[i] = x_[i] - x0;
		y_[i] = y_[i] - y0;
	}
}


}
