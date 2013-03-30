#ifndef shapeplots_h
#define shapeplots_h

#include "shape.hpp"
#include "ruled_surface.hpp"
#include "qcustomplot.h"

	struct BoundingBox {
		double xMin_;
		double xMax_;
		double yMin_;
		double yMax_;
	};

	QCPCurveDataMap *breakPoints(const foamcut::Shape &shape);

	QCPCurveDataMap *lineFit(const foamcut::Shape &shape);

	std::pair<QCPCurveDataMap *, QCPCurveDataMap *> surfaceLines(const foamcut::RuledSurface &rs);

	BoundingBox bounds(const QCPCurveDataMap *dm);

#endif
