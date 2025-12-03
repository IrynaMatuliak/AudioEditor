#include "fadedialog.h"
#include <QApplication>
#include <QStyle>

FadeDialog::FadeDialog(FadeType type, QWidget *parent)
    : QDialog(parent), fadeType(type)
{
    setupUI(type);
    setupStyles();
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void FadeDialog::setupUI(FadeType type)
{
    setWindowTitle(type == FadeIn ? "Fade In Settings" : "Fade Out Settings");
    setFixedSize(400, 280);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 15);

    QLabel *titleLabel = new QLabel(
        type == FadeIn ? "Fade In - Gradual Volume Increase" : "Fade Out - Gradual Volume Decrease",
        this
        );
    titleLabel->setObjectName("titleLabel");
    mainLayout->addWidget(titleLabel);

    QWidget *durationWidget = new QWidget(this);
    QHBoxLayout *durationLayout = new QHBoxLayout(durationWidget);
    durationLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *durationLabel = new QLabel("Fade Duration:", this);
    durationLabel->setObjectName("settingLabel");

    durationSpinBox = new QDoubleSpinBox(this);
    durationSpinBox->setObjectName("durationSpinBox");
    durationSpinBox->setRange(0.1, 30.0);
    durationSpinBox->setValue(2.0);
    durationSpinBox->setSingleStep(0.1);
    durationSpinBox->setDecimals(1);
    durationSpinBox->setSuffix(" seconds");

    durationLayout->addWidget(durationLabel);
    durationLayout->addStretch();
    durationLayout->addWidget(durationSpinBox);
    mainLayout->addWidget(durationWidget);

    QWidget *curveWidget = new QWidget(this);
    QHBoxLayout *curveLayout = new QHBoxLayout(curveWidget);
    curveLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *curveLabel = new QLabel("Fade Curve:", this);
    curveLabel->setObjectName("settingLabel");

    curveComboBox = new QComboBox(this);
    curveComboBox->setObjectName("curveComboBox");
    curveComboBox->addItem("Linear", "linear");
    curveComboBox->addItem("Exponential", "exponential");
    curveComboBox->addItem("Logarithmic", "logarithmic");
    curveComboBox->addItem("S-Curve", "scurve");
    curveComboBox->addItem("Cosine", "cosine");
    curveComboBox->setCurrentIndex(0);

    curveLayout->addWidget(curveLabel);
    curveLayout->addStretch();
    curveLayout->addWidget(curveComboBox);
    mainLayout->addWidget(curveWidget);

    QWidget *applyWidget = new QWidget(this);
    QHBoxLayout *applyLayout = new QHBoxLayout(applyWidget);
    applyLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *applyLabel = new QLabel("Apply to:", this);
    applyLabel->setObjectName("settingLabel");

    applyComboBox = new QComboBox(this);
    applyComboBox->setObjectName("applyComboBox");
    applyComboBox->addItem("Entire Track", "entire");
    applyComboBox->addItem("Selection Only", "selection");
    applyComboBox->setCurrentIndex(0);

    applyLayout->addWidget(applyLabel);
    applyLayout->addStretch();
    applyLayout->addWidget(applyComboBox);
    mainLayout->addWidget(applyWidget);

    QLabel *previewLabel = new QLabel(this);
    previewLabel->setObjectName("previewLabel");

    QString curveDescription;
    QString currentCurve = curveComboBox->currentText();
    if (currentCurve == "Linear") {
        curveDescription = "Linear: Constant rate of change";
    } else if (currentCurve == "Exponential") {
        curveDescription = "Exponential: Slow start, fast end";
    } else if (currentCurve == "Logarithmic") {
        curveDescription = "Logarithmic: Fast start, slow end";
    } else if (currentCurve == "S-Curve") {
        curveDescription = "S-Curve: Smooth acceleration and deceleration";
    } else if (currentCurve == "Cosine") {
        curveDescription = "Cosine: Natural smooth curve";
    }

    previewLabel->setText(QString("Preview: %1").arg(curveDescription));
    previewLabel->setWordWrap(true);
    mainLayout->addWidget(previewLabel);

    connect(curveComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, previewLabel]() {
        QString curveDescription;
        QString currentCurve = curveComboBox->currentText();

        if (currentCurve == "Linear") {
            curveDescription = "Linear: Constant rate of change";
        } else if (currentCurve == "Exponential") {
            curveDescription = "Exponential: Slow start, fast end";
        } else if (currentCurve == "Logarithmic") {
            curveDescription = "Logarithmic: Fast start, slow end";
        } else if (currentCurve == "S-Curve") {
            curveDescription = "S-Curve: Smooth acceleration and deceleration";
        } else if (currentCurve == "Cosine") {
            curveDescription = "Cosine: Natural smooth curve";
        }

        previewLabel->setText(QString("Preview: %1").arg(curveDescription));
    });

    mainLayout->addStretch();

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName("buttonBox");

    QPushButton *okButton = buttonBox->addButton(QDialogButtonBox::Ok);
    QPushButton *cancelButton = buttonBox->addButton(QDialogButtonBox::Cancel);

    okButton->setObjectName("okButton");
    cancelButton->setObjectName("cancelButton");

    okButton->setDefault(true);

    mainLayout->addWidget(buttonBox);

    connect(okButton, &QPushButton::clicked, this, &FadeDialog::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &FadeDialog::onCancelClicked);
}

void FadeDialog::setupStyles()
{
    setStyleSheet(R"(
        FadeDialog {
            background-color: #2b2b2b;
            color: #ffffff;
            font-family: "Segoe UI", Arial, sans-serif;
        }

        #titleLabel {
            color: #ffffff;
            font-size: 16px;
            font-weight: bold;
            padding: 5px 0px;
            border-bottom: 1px solid #444444;
            margin-bottom: 10px;
        }

        #settingLabel {
            color: #cccccc;
            font-size: 14px;
            font-weight: normal;
            min-width: 100px;
        }

        #durationSpinBox, #curveComboBox, #applyComboBox {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 8px 12px;
            font-size: 13px;
            min-width: 150px;
        }

        #durationSpinBox:focus, #curveComboBox:focus, #applyComboBox:focus {
            border-color: #0078d7;
            outline: none;
        }

        /* Стилі для кнопок QDoubleSpinBox */
        #durationSpinBox::up-button, #durationSpinBox::down-button {
            background-color: #555555;
            border: 1px solid #666666;
            width: 20px;
            border-radius: 2px;
            margin: 1px;
        }

        #durationSpinBox::up-button {
            subcontrol-position: top right;
            border-bottom: none;
            border-bottom-left-radius: 0px;
            border-bottom-right-radius: 0px;
        }

        #durationSpinBox::down-button {
            subcontrol-position: bottom right;
            border-top: none;
            border-top-left-radius: 0px;
            border-top-right-radius: 0px;
        }

        #durationSpinBox::up-button:hover, #durationSpinBox::down-button:hover {
            background-color: #666666;
        }

        #durationSpinBox::up-button:pressed, #durationSpinBox::down-button:pressed {
            background-color: #444444;
        }

        #durationSpinBox::up-arrow {
            width: 4px;
            height: 2px;
            background: none;
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-bottom: 6px solid #cccccc;
        }

        #durationSpinBox::down-arrow {
            width: 4px;
            height: 2px;
            background: none;
            image: none;
            border-left: 4px solid transparent;
            border-right: 4px solid transparent;
            border-top: 6px solid #cccccc;
        }

        #durationSpinBox::up-arrow:hover, #durationSpinBox::down-arrow:hover {
            border-bottom-color: #ffffff;
            border-top-color: #ffffff;
        }

        #durationSpinBox::up-arrow:pressed, #durationSpinBox::down-arrow:pressed {
            border-bottom-color: #aaaaaa;
            border-top-color: #aaaaaa;
        }

        #curveComboBox::drop-down, #applyComboBox::drop-down {
            border: none;
            width: 25px;
        }

        #curveComboBox::down-arrow, #applyComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #cccccc;
            width: 0px;
            height: 0px;
        }

        #curveComboBox::down-arrow:hover, #applyComboBox::down-arrow:hover {
            border-top-color: #ffffff;
        }

        #curveComboBox QListView, #applyComboBox QListView {
            background-color: #3a3a3a;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            padding: 4px;
            font-size: 13px;
        }

        #curveComboBox QListView::item, #applyComboBox QListView::item {
            padding: 8px 12px;
            border-radius: 2px;
        }

        #curveComboBox QListView::item:hover, #applyComboBox QListView::item:hover {
            background-color: #0078d7;
        }

        #curveComboBox QListView::item:selected, #applyComboBox QListView::item:selected {
            background-color: #005a9e;
        }

        #previewLabel {
            color: #888888;
            font-size: 12px;
            font-style: italic;
            background-color: #333333;
            padding: 10px;
            border-radius: 4px;
            border-left: 3px solid #0078d7;
            margin: 10px 0px;
        }

        #buttonBox {
            margin-top: 10px;
        }

        #okButton, #cancelButton {
            background-color: #555555;
            color: #ffffff;
            border: 1px solid #666666;
            border-radius: 4px;
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
            min-width: 80px;
            margin: 0px 5px;
        }

        #okButton {
            background-color: #0078d7;
            border-color: #0078d7;
        }

        #okButton:hover {
            background-color: #106ebe;
            border-color: #106ebe;
        }

        #okButton:pressed {
            background-color: #005a9e;
            border-color: #005a9e;
        }

        #cancelButton:hover {
            background-color: #666666;
            border-color: #777777;
        }

        #cancelButton:pressed {
            background-color: #444444;
            border-color: #555555;
        }
    )");

    if (qApp->style()->objectName().contains("dark", Qt::CaseInsensitive)) {
        setStyleSheet(styleSheet() + R"(
            #durationSpinBox, #curveComboBox, #applyComboBox {
                background-color: #2d2d2d;
                border-color: #404040;
            }
        )");
    }
}

double FadeDialog::getFadeDuration() const
{
    return durationSpinBox->value();
}

QString FadeDialog::getCurveType() const
{
    return curveComboBox->currentData().toString();
}

bool FadeDialog::applyToSelection() const
{
    return applyComboBox->currentData().toString() == "selection";
}

void FadeDialog::onOkClicked()
{
    if (durationSpinBox->value() <= 0) {
        durationSpinBox->setFocus();
        return;
    }
    accept();
}

void FadeDialog::onCancelClicked()
{
    reject();
}
