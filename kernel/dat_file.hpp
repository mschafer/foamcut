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
#ifndef foamcut_dat_file_hpp
#define foamcut_dat_file_hpp

#include "foamcut_kernel_dll.h"
#include <istream>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace foamcut {

/**
 * In memory representation of a dat file which specifies one side of the
 * geometry to cut in the format used by XFoil.
 * Each line of the file should contain the two floating point numbers delimited
 * by whitespace--the x coordinate followed by the y coordinate.
 * Two successive lines with identical x and y values will be treated as a break
 * point or slope discontinuity.
 * The first line of the file may contain a string name or the first pair of numbers.
 */
class foamcut_kernel_API DatFile {
public:
	typedef boost::shared_ptr<DatFile> handle;

	virtual ~DatFile();

	const std::string &name() const  { return name_; }
	const std::vector<double> &x() const { return x_; }
	const std::vector<double> &y() const { return y_; }

	static handle read(std::istream &in);

	/// Reverse the order of points.
	handle reverse() const;

	handle rotate(double theta) const;
	handle scale(double s) const;
	handle scale(double sx, double sy) const;

private:
	DatFile();

	/// Translate the DatFile so the first point is 0,0
	void origin();

	std::vector<double> x_;
	std::vector<double> y_;
	std::string name_;
};

}

std::ostream &operator<<(std::ostream &os, const foamcut::DatFile &datfile);

#endif
