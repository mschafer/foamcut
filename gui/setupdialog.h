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
