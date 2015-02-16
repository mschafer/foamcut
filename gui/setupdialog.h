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
#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <QSerialPortInfo>
#include <memory>

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SetupDialog(QWidget *parent = 0);
    ~SetupDialog();

private slots:
	void accept();
	void on_port_combo_currentIndexChanged(int index);

private:
    std::unique_ptr<Ui::SetupDialog> ui;
	QList<QSerialPortInfo> serialPorts_;
};

#endif // SETUPDIALOG_H
