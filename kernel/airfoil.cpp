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
    
    Airfoil::Airfoil(DatFile::handle datfile, double chord, double alfa, bool leLoop) :
    		datfile_(datfile), hasLELoop_(leLoop)
    {
        size_t n = datfile_->x().size();

        // reflect through y axis if TE is to the right
        if (datfile_->x()[0] > datfile_->x()[n/2]) {
        	datfile_ = datfile_->scale(-1., 1.);
        }

        // reverse coordinate order if bottom is first
        if (datfile_->y()[n/4] < datfile_->y()[3*n/4]) {
        	datfile_ = datfile_->reverse();
        }

        ///\ todo handle datfile not at 0 alfa
        
        // scale to correct chord
        typedef std::vector<double>::const_iterator iterator;
        std::pair<iterator, iterator> minmax = boost::minmax_element(datfile_->x().begin(), datfile_->x().end());
        datfile_ = datfile_->scale(chord / (*(minmax.second) - *(minmax.first)));

        // rotate to desired alfa (degrees)
        datfile_ = datfile_->rotate(alfa);

        shape_.reset(new Shape(*datfile_));
        sle_ = findLeadingEdge();

        // sync at leading edge
        shape_ = shape_->insertBreak(sle_);
        x_ = shape_->x();
        y_ = shape_->y();
    }
    
    Airfoil::~Airfoil()
    {
        
    }
    
    double
    Airfoil::area() const
    {
        double area = 0.;
        size_t npt = x_.size();
        for (size_t i=0; i<npt-1; i++) {
            area += x_[i] * y_[i+1] - x_[i+1] * y_[i];
        }
        return area / 2.;
    }
    
    void
    Airfoil::leadingEdgeLoop()
    {
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
