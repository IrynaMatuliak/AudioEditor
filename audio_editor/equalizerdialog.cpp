#include "equalizerdialog.h"
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSettings>
#include <QApplication>
#include <QDebug>

double EqualizerDialog::lastLowGain = 0.0;
double EqualizerDialog::lastMidGain = 0.0;
double EqualizerDialog::lastHighGain = 0.0;
bool EqualizerDialog::settingsLoaded = false;

EqualizerDialog::EqualizerDialog(QWidget *parent)
    : QDialog(parent)
    , lowSlider(nullptr)
    , midSlider(nullptr)
    , highSlider(nullptr)
    , lowLabel(nullptr)
    , midLabel(nullptr)
    , highLabel(nullptr)
    , okButton(nullptr)
    , cancelButton(nullptr)
    , resetButton(nullptr)
{
    setupUI();
    loadSettings();
}

EqualizerDialog::~EqualizerDialog()
{
    saveSettings();
}

void EqualizerDialog::setupUI()
{
    setWindowTitle("Equalizer Settings");
    setModal(true);
    setFixedSize(400, 300);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *eqGroup = new QGroupBox("Equalizer Bands");
    QFormLayout *formLayout = new QFormLayout(eqGroup);

    lowSlider = new QSlider(Qt::Horizontal);
    lowSlider->setRange(-12, 12);
    lowSlider->setValue(0);
    lowSlider->setTickInterval(3);
    lowSlider->setTickPosition(QSlider::TicksBelow);
    lowLabel = new QLabel("0 dB");
    lowLabel->setFixedWidth(40);

    QHBoxLayout *lowLayout = new QHBoxLayout();
    lowLayout->addWidget(new QLabel("Bass (60-250 Hz)"));
    lowLayout->addWidget(lowSlider);
    lowLayout->addWidget(lowLabel);
    formLayout->addRow(lowLayout);

    midSlider = new QSlider(Qt::Horizontal);
    midSlider->setRange(-12, 12);
    midSlider->setValue(0);
    midSlider->setTickInterval(3);
    midSlider->setTickPosition(QSlider::TicksBelow);
    midLabel = new QLabel("0 dB");
    midLabel->setFixedWidth(40);

    QHBoxLayout *midLayout = new QHBoxLayout();
    midLayout->addWidget(new QLabel("Mid (250-4000 Hz)"));
    midLayout->addWidget(midSlider);
    midLayout->addWidget(midLabel);
    formLayout->addRow(midLayout);

    highSlider = new QSlider(Qt::Horizontal);
    highSlider->setRange(-12, 12);
    highSlider->setValue(0);
    highSlider->setTickInterval(3);
    highSlider->setTickPosition(QSlider::TicksBelow);
    highLabel = new QLabel("0 dB");
    highLabel->setFixedWidth(40);

    QHBoxLayout *highLayout = new QHBoxLayout();
    highLayout->addWidget(new QLabel("Treble (4000-16000 Hz)"));
    highLayout->addWidget(highSlider);
    highLayout->addWidget(highLabel);
    formLayout->addRow(highLayout);

    mainLayout->addWidget(eqGroup);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    resetButton = new QPushButton("Reset");
    okButton = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");

    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(lowSlider, &QSlider::valueChanged, this, &EqualizerDialog::updateLabels);
    connect(midSlider, &QSlider::valueChanged, this, &EqualizerDialog::updateLabels);
    connect(highSlider, &QSlider::valueChanged, this, &EqualizerDialog::updateLabels);

    connect(resetButton, &QPushButton::clicked, this, [this]() {
        lowSlider->setValue(0);
        midSlider->setValue(0);
        highSlider->setValue(0);
    });

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    updateLabels();

    setStyleSheet(
        "QDialog { background-color: #333333; color: white; }"
        "QLabel { color: white; }"
        "QGroupBox { color: white; border: 1px solid #555555; margin-top: 10px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }"
        "QPushButton { "
        "   background-color: #555555; color: white; border: 1px solid #777777;"
        "   padding: 5px 15px; border-radius: 3px;"
        "}"
        "QPushButton:hover { background-color: #666666; }"
        "QSlider::groove:horizontal { "
        "   border: 1px solid #999999; height: 8px; background: #444444; border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal { "
        "   background: #0078d7; border: 1px solid #5c5c5c; width: 18px; margin: -5px 0; border-radius: 9px;"
        "}"
        "QSlider::sub-page:horizontal { background: #0078d7; border-radius: 4px; }"
        "QSlider::add-page:horizontal { background: #444444; border-radius: 4px; }"
        );
}

void EqualizerDialog::updateLabels()
{
    lowLabel->setText(QString("%1 dB").arg(lowSlider->value()));
    midLabel->setText(QString("%1 dB").arg(midSlider->value()));
    highLabel->setText(QString("%1 dB").arg(highSlider->value()));
}

void EqualizerDialog::saveSettings()
{
    QSettings settings("AudioEditor", "Equalizer");
    settings.setValue("lowGain", lowSlider->value());
    settings.setValue("midGain", midSlider->value());
    settings.setValue("highGain", highSlider->value());

    lastLowGain = lowSlider->value();
    lastMidGain = midSlider->value();
    lastHighGain = highSlider->value();

    qDebug() << "Equalizer settings saved:"
             << "Low:" << lastLowGain
             << "Mid:" << lastMidGain
             << "High:" << lastHighGain;
}

void EqualizerDialog::loadSettings()
{
    QSettings settings("AudioEditor", "Equalizer");

    double low = settings.value("lowGain", lastLowGain).toDouble();
    double mid = settings.value("midGain", lastMidGain).toDouble();
    double high = settings.value("highGain", lastHighGain).toDouble();

    lowSlider->setValue(static_cast<int>(low));
    midSlider->setValue(static_cast<int>(mid));
    highSlider->setValue(static_cast<int>(high));

    lastLowGain = low;
    lastMidGain = mid;
    lastHighGain = high;
    settingsLoaded = true;

    qDebug() << "Equalizer settings loaded:"
             << "Low:" << lastLowGain
             << "Mid:" << lastMidGain
             << "High:" << lastHighGain;
}

double EqualizerDialog::getLowGain() const
{
    return lowSlider->value();
}

double EqualizerDialog::getMidGain() const
{
    return midSlider->value();
}

double EqualizerDialog::getHighGain() const
{
    return highSlider->value();
}

void EqualizerDialog::setGains(double low, double mid, double high)
{
    lowSlider->setValue(static_cast<int>(low));
    midSlider->setValue(static_cast<int>(mid));
    highSlider->setValue(static_cast<int>(high));

    lastLowGain = low;
    lastMidGain = mid;
    lastHighGain = high;

    updateLabels();
}
