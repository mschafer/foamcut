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
#include <fstream>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/filesystem.hpp>

#include "airfoil.hpp"
#include "dat_file.hpp"

BOOST_AUTO_TEST_CASE( airfoil_xfoil_import )
{
    using namespace foamcut;
    
    std::string fname(FOAMCUT_TEST_DATA_DIR);
    fname += "/ag45c-03.dat";
    std::cout << fname << std::endl;
    std::ifstream xfs(fname.c_str(), std::ifstream::in);
    BOOST_CHECK(xfs.is_open());

    DatFile::handle dat = DatFile::read(xfs);
    Airfoil air(dat, 10, 0, true);
    Shape::handle shp = air.shape();

    BOOST_CHECK(shp->area() < 0);

    std::ofstream out("airfoil_test.txt");
    const std::vector<double> &x = shp->x();
    const std::vector<double> &y = shp->y();
    out << "x" << "\t" << "y" << std::endl;
    for (size_t i=0; i<x.size(); ++i) {
        out << x[i] << "\t" << y[i] << std::endl;
    }

    ///\todo add test for le insertion, winding, etc.

}

BOOST_AUTO_TEST_CASE( airfoil_offset_test )
{
    using namespace foamcut;
    std::string fname(FOAMCUT_TEST_DATA_DIR);
    fname += "/ag45c-03.dat";
    std::ifstream xfs(fname.c_str(), std::ifstream::in);
    BOOST_CHECK(xfs.is_open());

    DatFile::handle dat = DatFile::read(xfs);
    Airfoil air(dat, 10, 0, false);
    Shape::handle shp = air.shape();

    // use the last 4 points of the first spline and the first 4 of the second as offset test.
    auto xs0 = shp->xSpline(0);
    auto ys0 = shp->ySpline(0);
    auto xs1 = shp->xSpline(1);
    auto ys1 = shp->ySpline(1);
    std::vector<double> x(8), y(8);
    size_t n0 = xs0.x().size();
    for (size_t i=0; i<4; ++i) {
    	x[i] = xs0.y()[n0-4+i];
    	y[i] = ys0.y()[n0-4+i];
    	x[i+4] = xs1.y()[i];
    	y[i+4] = ys1.y()[i];
    }
    BOOST_CHECK(x[3] == x[4] && y[3] == y[4]);

    shp.reset(new Shape(x, y));
    for (size_t i=0; i<shp->x().size(); ++i) {
    	//std::cout << shp->x()[i] << '\t' << shp->y()[i] << std::endl;
    }

//    auto shp2 = shp->offset(.01);

}
