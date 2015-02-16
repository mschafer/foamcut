/*
 * (C) Copyright 2015 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#ifndef IMPORTDAT_H
#define IMPORTDAT_H

#include <QObject>
#include <QString>
#include <Shape.hpp>

class ImportDat : public QObject
{
    Q_OBJECT
    
public:
    explicit ImportDat(const QString &name, QObject *parent = nullptr);
    ~ImportDat();

	Q_PROPERTY(QString fileName READ fileName WRITE setFileName);
	Q_PROPERTY(QString datName READ datName WRITE setDatName);
	Q_PROPERTY(bool isAirfoil READ isAirfoil WRITE setIsAirfoil);
	Q_PROPERTY(bool leLoop READ leLoop WRITE setLeLoop);
	Q_PROPERTY(double chord READ chord WRITE setChord);
	Q_PROPERTY(double alpha READ alpha WRITE setAlpha);

	const QString &fileName() const { return fileName_; }
	void setFileName(const QString &n);

	const QString &datName() const { return datName_; }
	void setDatName(const QString &n);

	bool isAirfoil() const { return isAirfoil_; }
	void setIsAirfoil(bool v);

	bool leLoop() const { return leLoop_; }
	void setLeLoop(bool v);

	double chord() const { return chord_; }
	void setChord(double v);

	double alpha() const { return alpha_; }
	void setAlpha(double v);

	/**
	 * Attempts to open the .dat file and generate a shape.
	 */
	void import();

	/**
	 * \return a handle to the shape imported from the .dat file.
	 * The handle will be empty if the last import failed or if any of
	 * the properties have been modified since the last import.
	 */
	foamcut::Shape::handle shape() { return shape_; }

signals:
	void shapeChanged();

private:
	QString datName_;
	QString fileName_;
	bool isAirfoil_;
	bool leLoop_;
	double chord_;
	double alpha_;
	foamcut::Shape::handle shape_;
};

#endif // MOVEDIALOG_H
