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
#include <stdexcept>
#include <boost/optional.hpp>
#include <boost/algorithm/minmax.hpp>
#include <boost/algorithm/minmax_element.hpp>
#include <cmath>
#include "airfoil.hpp"

namespace foamcut {

Airfoil::Airfoil(DatFile::handle datfile, double chord, double alpha, bool leLoop) :
datfile_(datfile), chord_(chord), alpha_(alpha), hasLELoop_(leLoop)
{
    shape_.reset(new Shape(*datfile_));
    size_t n = shape_->x().size();

    // reflect through y axis if TE is to the right
    if (shape_->x()[0] > shape_->x()[n/2]) {
        shape_ = shape_->scale(-1., 1.);
    }

    // reverse coordinate order if bottom is first
    if (shape_->y()[n/4] < shape_->y()[3*n/4]) {
        shape_ = shape_->reverse();
    }

    // scale to correct chord
    typedef std::vector<double>::const_iterator iterator;
    std::pair<iterator, iterator> minmax = boost::minmax_element(datfile_->x().begin(), datfile_->x().end());
    shape_ = shape_->scale(chord_ / (*(minmax.second) - *(minmax.first)));

    sle_ = findLeadingEdge();
    
    // calculate le loop before inserting break to get tangent right 
    if (leLoop) {
        leadingEdgeLoop(shape_->evaluate(sle_));
    }

    // sync at leading edge
    shape_ = shape_->insertBreak(sle_);

    // now splice in le loop
    if (leLoop) {
        Shape::Point ple = shape_->evaluate(sle_);
        size_t iseg = shape_->nearestBreak(ple.x, ple.y);
        Shape loop(xLELoop_, yLELoop_);
        shape_ = shape_->insertShape(iseg, loop);
    }

    // rotate to desired alfa (degrees)
    shape_ = shape_->rotate(alpha_);
}

Airfoil::~Airfoil()
{
}

void
Airfoil::leadingEdgeLoop(const Shape::Point &ple)
{
    double r = fabs(shape_->area()) / chord_;
    double xtan = ple.x_s;
    double ytan = ple.y_s;
    double l = r / hypot(xtan, ytan);
    xtan *= l;
    ytan *= l;

    double theta = atan2(ple.y_s, ple.x_s) + 2. * atan(1.);

    xLELoop_.clear();
    yLELoop_.clear();
    xLELoop_.push_back(0.);    yLELoop_.push_back(0.);
    xLELoop_.push_back(xtan);  yLELoop_.push_back(ytan);
    xLELoop_.push_back(xtan);  yLELoop_.push_back(ytan);
    xLELoop_.push_back(-ytan); yLELoop_.push_back(xtan);
    xLELoop_.push_back(-xtan); yLELoop_.push_back(-ytan);
    xLELoop_.push_back(-xtan); yLELoop_.push_back(-ytan);
    xLELoop_.push_back(0.);    yLELoop_.push_back(0.);
}


double
Airfoil::findLeadingEdge()
{
    const std::vector<double> &x = shape_->x();
    const std::vector<double> &y = shape_->y();
    const std::vector<double> &s = shape_->s();
    double xte = .5 * (x.front() + x.back());
    double yte = .5 * (y.front() + y.back());

    // initial guess is the point furthest from the te
    boost::optional<int> lastSign;
    size_t ile, ileGuess;
    double dmax = -1.;
    for (ile=2; ile<x.size()-1; ile++) {
        double dxte = x[ile] - xte;
        double dyte = y[ile] - yte;
        double d = hypot(dxte, dyte);
        if (d > dmax) {
            dmax = d;
            ileGuess = ile;
        }
    }

    double sle = s[ileGuess];
    double stol = sle * 1.e-8;
    double dsle;
    int iter = 10;
    do {
        Shape::Point pt = shape_->evaluate(sle);
        double xchord = pt.x - xte;
        double ychord = pt.y - yte;
        double res = pt.x_s * xchord + pt.y_s * ychord;
        double res_s = pt.x_s  * pt.x_s + pt.y_s  * pt.y_s +
            pt.x_ss * xchord  + pt.y_ss * ychord;

        dsle = -res / res_s;

        double chord = hypot(xchord, ychord);
        double curv = (pt.x_s * pt.y_ss - pt.y_s * pt.x_ss) /
            sqrt(pow(pt.x_s*pt.x_s + pt.y_s*pt.y_s, 3));

        double dslim = 0.01 * chord;
        if (curv != 0.0) {
            dslim = std::min(dslim, .1 / abs(curv));
        }

        dsle = std::min(dsle, dslim);
        dsle = std::max(dsle, -dslim);

        sle += dsle;

        iter--;
        if (iter == 0) {
            throw(std::range_error("Airfoil::FindLeadingEdge failed to converge"));
        }

    } while(abs(dsle) > stol);        

    Shape::Point lePoint =  shape_->evaluate(sle);
    chord_ = hypot(lePoint.x - xte, lePoint.y - yte);
    sle_ = sle;
    return sle;
}

}
