#include "importdat.hpp"
#include <fstream>
#include <dat_file.hpp>
#include <airfoil.hpp>
#include <QDebug>

ImportDat::ImportDat(const QString &name, QObject *parent) : QObject(parent)
{
	setObjectName(name);
}

ImportDat::~ImportDat()
{

}

void ImportDat::setFileName(const QString &n)
{
	fileName_ = n;
	shape_.reset();
}

void ImportDat::setDatName(const QString &n)
{
	datName_ = n;
	shape_.reset();
}

void ImportDat::setIsAirfoil(bool v) 
{
	isAirfoil_ = v; 
	shape_.reset();
}

void ImportDat::setLeLoop(bool v)
{
	leLoop_ = v; 
	shape_.reset();
}

void ImportDat::setChord(double v)
{
	chord_ = v;
	shape_.reset();
}

void ImportDat::setAlpha(double v)
{
	alpha_ = v;
	shape_.reset();
}

void ImportDat::import()
{
	std::ifstream in(fileName_.toLatin1(), std::ifstream::in);
	auto datFile = foamcut::DatFile::read(in);
	if (isAirfoil_) {
		foamcut::Airfoil::handle airfoil(new foamcut::Airfoil(datFile, chord_, alpha_, leLoop_));
		shape_ = airfoil->shape();
	} else {
		shape_.reset(new foamcut::Shape(*datFile));
	}
}
