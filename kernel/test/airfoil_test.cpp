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
    Airfoil air(dat, 10, 0);
    Shape::handle shp = air.shape();

    BOOST_CHECK(air.area() < 0);

    ///\todo add test for le insertion, winding, etc.

}
