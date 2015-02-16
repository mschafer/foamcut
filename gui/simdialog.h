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
#ifndef SIMDIALOG_H
#define SIMDIALOG_H

#include <memory>
#include <QDialog>
#include "qcustomplot.h"

namespace Ui {
class SimDialog;
}

class SimDialog : public QDialog
{
    Q_OBJECT
    
public:
    SimDialog(QWidget *parent = 0);
    ~SimDialog();

    void done(int result);

private slots:
	void update();
	void on_clear_button_clicked();
	void my_limit_editingFinished();

private:
	enum {
		LEFT_CURVE = 0,
		RIGHT_CURVE = 1
	};

    std::unique_ptr<Ui::SimDialog> ui;
};

#endif
