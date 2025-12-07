#include "NoiseReductionDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QFrame>

NoiseReductionDialog::NoiseReductionDialog(QWidget *parent)
    : QDialog(parent)
    , methodComboBox(nullptr)
    , thresholdSpinBox(nullptr)
    , fftSizeComboBox(nullptr)
    , noiseReductionSpinBox(nullptr)
    , smoothingSpinBox(nullptr)
    , learnNoiseCheckBox(nullptr)
{
    setupUI();
    setupStyles();
    setWindowTitle("Noise Reduction Settings");
    setFixedSize(450, 500);
}

void NoiseReductionDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel("Noise Reduction Settings");
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    QGroupBox *basicGroup = new QGroupBox("Basic Parameters");
    basicGroup->setObjectName("basicGroup");
    QFormLayout *basicLayout = new QFormLayout(basicGroup);
    basicLayout->setSpacing(12);
    basicLayout->setContentsMargins(15, 20, 15, 20);

    QLabel *methodLabel = new QLabel("Method:");
    methodLabel->setObjectName("formLabel");
    methodComboBox = new QComboBox();
    methodComboBox->setObjectName("methodComboBox");
    methodComboBox->addItem("Spectral Subtraction", "spectral");
    // methodComboBox->addItem("Wiener Filter", "wiener");
    // methodComboBox->addItem("Adaptive Filter", "adaptive");
    basicLayout->addRow(methodLabel, methodComboBox);

    QLabel *thresholdLabel = new QLabel("Noise Threshold:");
    thresholdLabel->setObjectName("formLabel");
    thresholdSpinBox = new QDoubleSpinBox();
    thresholdSpinBox->setObjectName("thresholdSpinBox");
    thresholdSpinBox->setRange(0.01, 0.5);
    thresholdSpinBox->setValue(0.1);
    thresholdSpinBox->setSingleStep(0.01);
    thresholdSpinBox->setDecimals(2);
    thresholdSpinBox->setSuffix(" dB");
    basicLayout->addRow(thresholdLabel, thresholdSpinBox);

    QLabel *fftLabel = new QLabel("FFT Size:");
    fftLabel->setObjectName("formLabel");
    fftSizeComboBox = new QComboBox();
    fftSizeComboBox->setObjectName("fftSizeComboBox");
    fftSizeComboBox->addItem("512 points", 512);
    fftSizeComboBox->addItem("1024 points", 1024);
    fftSizeComboBox->addItem("2048 points", 2048);
    fftSizeComboBox->addItem("4096 points", 4096);
    fftSizeComboBox->setCurrentIndex(2);
    basicLayout->addRow(fftLabel, fftSizeComboBox);

    mainLayout->addWidget(basicGroup);

    QGroupBox *advancedGroup = new QGroupBox("Advanced Parameters");
    advancedGroup->setObjectName("advancedGroup");
    QFormLayout *advancedLayout = new QFormLayout(advancedGroup);
    advancedLayout->setSpacing(12);
    advancedLayout->setContentsMargins(15, 20, 15, 20);

    QLabel *strengthLabel = new QLabel("Reduction Strength:");
    strengthLabel->setObjectName("formLabel");
    noiseReductionSpinBox = new QDoubleSpinBox();
    noiseReductionSpinBox->setObjectName("noiseReductionSpinBox");
    noiseReductionSpinBox->setRange(0.1, 1.0);
    noiseReductionSpinBox->setValue(0.7);
    noiseReductionSpinBox->setSingleStep(0.1);
    noiseReductionSpinBox->setDecimals(1);
    advancedLayout->addRow(strengthLabel, noiseReductionSpinBox);

    QLabel *smoothingLabel = new QLabel("Smoothing:");
    smoothingLabel->setObjectName("formLabel");
    smoothingSpinBox = new QDoubleSpinBox();
    smoothingSpinBox->setObjectName("smoothingSpinBox");
    smoothingSpinBox->setRange(0.0, 1.0);
    smoothingSpinBox->setValue(0.3);
    smoothingSpinBox->setSingleStep(0.1);
    smoothingSpinBox->setDecimals(1);
    advancedLayout->addRow(smoothingLabel, smoothingSpinBox);

    learnNoiseCheckBox = new QCheckBox("Learn noise profile from selection");
    learnNoiseCheckBox->setObjectName("learnNoiseCheckBox");
    learnNoiseCheckBox->setChecked(true);
    advancedLayout->addRow(learnNoiseCheckBox);

    mainLayout->addWidget(advancedGroup);

    QFrame *infoFrame = new QFrame();
    infoFrame->setObjectName("infoFrame");
    QVBoxLayout *infoLayout = new QVBoxLayout(infoFrame);

    QLabel *infoLabel = new QLabel(
        "Noise reduction works best on constant background noise.\n"
        "Higher thresholds remove more noise but may affect audio quality.\n\n"
        "Tip: Select a noise-only section first for better results."
        );
    infoLabel->setObjectName("infoLabel");
    infoLabel->setWordWrap(true);
    infoLayout->addWidget(infoLabel);

    mainLayout->addWidget(infoFrame);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    QPushButton *cancelButton = new QPushButton("Cancel");
    cancelButton->setObjectName("cancelButton");
    cancelButton->setFixedSize(100, 35);

    QPushButton *okButton = new QPushButton("Apply");
    okButton->setObjectName("okButton");
    okButton->setFixedSize(100, 35);
    okButton->setDefault(true);

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);

    mainLayout->addLayout(buttonLayout);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void NoiseReductionDialog::setupStyles()
{
    setStyleSheet(R"(
        NoiseReductionDialog {
            background-color: #2b2b2b;
            color: #ffffff;
            font-family: Arial, sans-serif;
        }

        #titleLabel {
            color: #ffffff;
            font-size: 18px;
            font-weight: bold;
            padding: 10px 0px;
            border-bottom: 1px solid #444444;
            margin-bottom: 10px;
        }

        QGroupBox {
            color: #cccccc;
            font-weight: bold;
            font-size: 14px;
            border: 2px solid #444444;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 10px;
            background-color: #2b2b2b;
        }

        #formLabel {
            color: #cccccc;
            font-size: 12px;
            font-weight: bold;
            padding: 5px 0px;
        }

        QComboBox {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 8px 12px;
            font-size: 12px;
            min-width: 120px;
        }

        QComboBox:hover {
            border-color: #777777;
        }

        QComboBox::drop-down {
            subcontrol-origin: padding;
            subcontrol-position: top right;
            width: 20px;
            border-left: 1px solid #555555;
        }

        QComboBox::down-arrow {
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 4px solid #cccccc;
            width: 0px;
            height: 0px;
        }

        QComboBox QAbstractItemView {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            selection-background-color: #0078d7;
            outline: none;
        }

        QDoubleSpinBox {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 8px 12px;
            font-size: 12px;
            min-width: 100px;
        }

        QDoubleSpinBox:hover {
            border-color: #777777;
        }

        QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
            background-color: #4a4a4a;
            border: 1px solid #555555;
            width: 20px;
            border-radius: 2px;
        }

        QDoubleSpinBox::up-button:hover, QDoubleSpinBox::down-button:hover {
            background-color: #5a5a5a;
        }

        QDoubleSpinBox::up-arrow, QDoubleSpinBox::down-arrow {
            border: 1px solid #cccccc;
            width: 4px;
            height: 4px;
        }

        QCheckBox {
            color: #cccccc;
            font-size: 12px;
            spacing: 8px;
        }

        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border: 1px solid #555555;
            border-radius: 3px;
            background-color: #3a3a3a;
        }

        QCheckBox::indicator:checked {
            background-color: #0078d7;
            border-color: #0078d7;
        }

        QCheckBox::indicator:checked:hover {
            background-color: #108ee9;
            border-color: #108ee9;
        }

        #infoFrame {
            background-color: #1e1e1e;
            border: 1px solid #444444;
            border-radius: 6px;
            padding: 15px;
            margin: 10px 0px;
        }

        #infoLabel {
            color: #aaaaaa;
            font-size: 11px;
            line-height: 1.4;
        }

        #okButton {
            background-color: #0078d7;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            font-weight: bold;
            font-size: 12px;
        }

        #okButton:hover {
            background-color: #108ee9;
        }

        #okButton:pressed {
            background-color: #106ebe;
        }

        #cancelButton {
            background-color: #555555;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            font-weight: bold;
            font-size: 12px;
        }

        #cancelButton:hover {
            background-color: #666666;
        }

        #cancelButton:pressed {
            background-color: #444444;
        }
    )");
}

double NoiseReductionDialog::getNoiseThreshold() const
{
    return thresholdSpinBox->value();
}

int NoiseReductionDialog::getFftSize() const
{
    return fftSizeComboBox->currentData().toInt();
}

double NoiseReductionDialog::getReductionStrength() const
{
    return noiseReductionSpinBox->value();
}

double NoiseReductionDialog::getSmoothing() const
{
    return smoothingSpinBox->value();
}

QString NoiseReductionDialog::getMethod() const
{
    return methodComboBox->currentData().toString();
}

bool NoiseReductionDialog::getLearnNoiseProfile() const
{
    return learnNoiseCheckBox->isChecked();
}
