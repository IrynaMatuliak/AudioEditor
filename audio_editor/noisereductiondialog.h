#ifndef NOISEREDUCTIONDIALOG_H
#define NOISEREDUCTIONDIALOG_H

#include <QDialog>

class QDoubleSpinBox;
class QComboBox;
class QLabel;
class QGroupBox;
class QCheckBox;

class NoiseReductionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoiseReductionDialog(QWidget *parent = nullptr);

    double getNoiseThreshold() const;
    int getFftSize() const;
    double getReductionStrength() const;
    double getSmoothing() const;
    QString getMethod() const;
    bool getLearnNoiseProfile() const;

private:
    void setupUI();
    void setupStyles();

    QComboBox *methodComboBox;
    QDoubleSpinBox *thresholdSpinBox;
    QComboBox *fftSizeComboBox;
    QDoubleSpinBox *noiseReductionSpinBox;
    QDoubleSpinBox *smoothingSpinBox;
    QCheckBox *learnNoiseCheckBox;
};

#endif // NOISEREDUCTIONDIALOG_H
