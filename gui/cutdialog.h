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
#ifndef CUTDIALOG_H
#define CUTDIALOG_H

#include <QDialog>

namespace Ui {
class CutDialog;
}

class CutDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CutDialog(QWidget *parent = 0);
    ~CutDialog();

private slots:
	void on_pause_button_clicked();
	void on_speed_spin_valueChanged(double val);
	void on_buttonBox_rejected();
	void update();

private:
    Ui::CutDialog *ui;
};

#endif // CUTDIALOG_H
