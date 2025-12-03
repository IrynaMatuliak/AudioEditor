#include "echodialog.h"
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>

EchoDialog::EchoDialog(QWidget *parent)
    : QDialog(parent)
    , delaySpinBox(nullptr)
    , decaySpinBox(nullptr)
    , repetitionsSpinBox(nullptr)
    , okButton(nullptr)
    , cancelButton(nullptr)
{
    setWindowTitle("Echo Effect Settings");
    setModal(true);
    setFixedSize(300, 200);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QFormLayout *formLayout = new QFormLayout();

    delaySpinBox = new QDoubleSpinBox(this);
    delaySpinBox->setRange(10, 2000);
    delaySpinBox->setValue(300);
    delaySpinBox->setSuffix(" ms");
    delaySpinBox->setSingleStep(50);
    formLayout->addRow("Echo Delay:", delaySpinBox);

    decaySpinBox = new QDoubleSpinBox(this);
    decaySpinBox->setRange(0.1, 0.9);
    decaySpinBox->setValue(0.5);
    decaySpinBox->setSingleStep(0.1);
    decaySpinBox->setDecimals(2);
    formLayout->addRow("Echo Decay:", decaySpinBox);

    repetitionsSpinBox = new QSpinBox(this);
    repetitionsSpinBox->setRange(1, 10);
    repetitionsSpinBox->setValue(3);
    formLayout->addRow("Repetitions:", repetitionsSpinBox);

    mainLayout->addLayout(formLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    setStyleSheet(
        "QDialog { background-color: #333333; color: white; }"
        "QLabel { color: white; }"
        "QPushButton { "
        "   background-color: #555555; color: white; border: 1px solid #777777;"
        "   padding: 5px 15px; border-radius: 3px;"
        "}"
        "QPushButton:hover { background-color: #666666; }"
        "QDoubleSpinBox, QSpinBox { "
        "   background-color: #444444; color: white; border: 1px solid #666666;"
        "   padding: 3px; border-radius: 2px;"
        "}"
        );
}

double EchoDialog::getDelay() const
{
    return delaySpinBox ? delaySpinBox->value() : 300.0;
}

double EchoDialog::getDecay() const
{
    return decaySpinBox ? decaySpinBox->value() : 0.5;
}

int EchoDialog::getRepetitions() const
{
    return repetitionsSpinBox ? repetitionsSpinBox->value() : 3;
}
