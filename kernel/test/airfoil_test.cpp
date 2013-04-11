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
#include "ruled_surface.hpp"

BOOST_AUTO_TEST_CASE( airfoil_xfoil_import )
{
    using namespace foamcut;
    
    std::string fname(FOAMCUT_TEST_DATA_DIR);
    fname += "/ag45c-03.dat";
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

    double a = shp->area();
    auto kshp = shp->offset(.01);
    BOOST_CHECK_CLOSE((a-kshp->area()), -.01*shp->s().back(), .1);

    kshp = shp->offset(-.01);
    BOOST_CHECK_CLOSE((a-kshp->area()), .01*shp->s().back(), .1);
}

BOOST_AUTO_TEST_CASE( airfoil_make_wing )
{
    using namespace foamcut;
    std::string datDir(FOAMCUT_TEST_DATA_DIR);
    std::string fname = datDir + "/ag45c-03.dat";
    std::ifstream xfs(fname.c_str(), std::ifstream::in);
    BOOST_CHECK(xfs.is_open());
    DatFile::handle dat = DatFile::read(xfs);
    xfs.close();
    Airfoil root(dat, 6., 0., false);

    fname = datDir + "/ag47c-03.dat";
    xfs.open(fname.c_str(), std::ifstream::in);
    dat = DatFile::read(xfs);
    xfs.close();
    Airfoil tip(dat, 3., 3., false);

    auto rootShp = root.shape();
    auto tipShp = tip.shape();

    auto rootKShp = rootShp->offset(-.03);
    auto tipKShp = tipShp->offset(-.03);

 #if 0
    size_t n = tipKShp->x().size();
    for (size_t i=0; i<n; ++i) {
		std::cout << tipKShp->x()[i] << '\t' << tipKShp->y()[i] << std::endl;
	}
#endif

	RuledSurface::handle partRS(new foamcut::RuledSurface(*rootKShp, *tipKShp, 1., 24., .001));
#if 0
	size_t n = partRS->leftX().size();
	for (size_t i=0; i<n; ++i) {
		std::cout << partRS->leftX()[i] << '\t' << partRS->leftY()[i] <<
			  '\t'<< partRS->rightX()[i] << '\t'<< partRS->rightY()[i] << std::endl;
	}
#endif

	auto frameRS = partRS->interpolateZ(0., 30.);
}
