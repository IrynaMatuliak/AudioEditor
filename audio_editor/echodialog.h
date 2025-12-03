#ifndef ECHODIALOG_H
#define ECHODIALOG_H

#include <QDialog>

class QDoubleSpinBox;
class QSpinBox;
class QPushButton;

class EchoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EchoDialog(QWidget *parent = nullptr);

    double getDelay() const;
    double getDecay() const;
    int getRepetitions() const;

private:
    QDoubleSpinBox *delaySpinBox;
    QDoubleSpinBox *decaySpinBox;
    QSpinBox *repetitionsSpinBox;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // ECHODIALOG_H
