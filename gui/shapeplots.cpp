#include "shapeplots.h"
#include <boost/foreach.hpp>

QCPCurveDataMap *breakPoints(const foamcut::Shape &shape)
{
	QCPCurveDataMap *dataMap = new QCPCurveDataMap();
	size_t nseg = shape.segmentCount();
	for (size_t i=0; i<nseg; ++i) {
		std::pair<double, double> p = shape.breakPoint(i);
		double s = shape.breakS(i);
		QCPCurveData d(s, p.first, p.second);
		dataMap->insert(s, d);
	}
	return dataMap;
}

QCPCurveDataMap *lineFit(const foamcut::Shape &shape)
{
	QCPCurveDataMap *dataMap = new QCPCurveDataMap();
	double s = 0.;
	auto p0 = shape.evaluate(s);
	dataMap->insert(s, QCPCurveData(s, p0.x, p0.y));
	double sMax = shape.s().back();
	while (s < .99999 * sMax) {
		double s1 = shape.fitLineSegment(s, .001*sMax);
		auto p1 = shape.evaluate(s1);
		dataMap->insert(s1, QCPCurveData(s1, p1.x, p1.y));
		s = s1;
	}
	return dataMap;
}

std::pair<QCPCurveDataMap *, QCPCurveDataMap *> surfaceLines(const foamcut::RuledSurface &rs)
{
	QCPCurveDataMap *lDataMap = new QCPCurveDataMap();
	QCPCurveDataMap *rDataMap = new QCPCurveDataMap();
	size_t n = rs.leftX().size();
	for (size_t i=0; i<n; ++i) {
		lDataMap->insert((double)i, QCPCurveData((double)i, rs.leftX()[i], rs.leftY()[i]));
		rDataMap->insert((double)i, QCPCurveData((double)i, rs.rightX()[i], rs.rightY()[i]));
	}
	return std::make_pair(lDataMap, rDataMap);
}

BoundingBox bounds(const QCPCurveDataMap *dm)
{
	BoundingBox ret;
	QCPCurveData d = *(dm->begin());
	ret.xMin_ = ret.xMax_ = d.key;
	ret.yMin_ = ret.yMax_ = d.value;
	BOOST_FOREACH(QCPCurveData d, std::make_pair(dm->begin(), dm->end())) {
		ret.xMin_ = std::min(ret.xMin_, d.key);
		ret.xMax_ = std::max(ret.xMax_, d.key);
		ret.yMin_ = std::min(ret.yMin_, d.value);
		ret.yMax_ = std::max(ret.yMax_, d.value);
	}
	return ret;
}
