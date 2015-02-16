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
#ifndef MOVEDIALOG_H
#define MOVEDIALOG_H

#include <QDialog>

namespace Ui {
class MoveDialog;
}

class MoveDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit MoveDialog(QWidget *parent = 0);
    ~MoveDialog();

	virtual void done(int r);
    
private slots:
	void on_up_button_clicked();
	void on_left_button_clicked();
	void on_right_button_clicked();
	void on_down_button_clicked();
	void on_home_button_clicked();

private:
    Ui::MoveDialog *ui;

	void moveX(int sign);
	void moveY(int sign);
};

#endif // MOVEDIALOG_H
