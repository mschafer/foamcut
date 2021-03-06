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
#ifndef foamcut_airfoil_hpp
#define foamcut_airfoil_hpp

#include <istream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "shape.hpp"

namespace foamcut {

/**
 * Treats the coordinates in a dat file as an airfoil.
 * Reflects coordinates so TE is at x = 0 and LE is at x > 0
 * Reverses coordinate order so path goes over the top first
 * Scales to desired chord.
 * Rotates to desired alfa (degrees).
 * Adds a sync point at the leading edge.
 */
class Airfoil {
public:
    typedef boost::shared_ptr<Airfoil> handle;

    Airfoil(DatFile::handle datfile, double chord, double alpha, bool leLoop=false);
    virtual ~Airfoil();

    const std::string &name() const { return datfile_->name(); }

    Shape::handle shape() const {
        Shape::handle h(new Shape(*shape_));
        return h;
    }

private:

    /**
    * Finds the s coordiate of the leading edge and sets the chord.
    * \return s coordinate of leading edge.
    */
    double findLeadingEdge();

    /** 
     * Builds coordinates for leading edge loop in {x|y}LELoop.
     * \param ple Leading edge point used for tangent.
     */
    void leadingEdgeLoop(const Shape::Point &ple);

    std::vector<double> xLELoop_;
    std::vector<double> yLELoop_;
    double chord_;
    double alpha_;
    double sle_;
    Shape::Point ple_;
    Shape::handle shape_;
    DatFile::handle datfile_;
    bool hasLELoop_;
};
}

#endif
