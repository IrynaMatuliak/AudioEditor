#ifndef EQUALIZERDIALOG_H
#define EQUALIZERDIALOG_H

#include <QDialog>

class QSlider;
class QLabel;
class QPushButton;

class EqualizerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EqualizerDialog(QWidget *parent = nullptr);
    ~EqualizerDialog();

    double getLowGain() const;
    double getMidGain() const;
    double getHighGain() const;

    void setGains(double low, double mid, double high);

private slots:
    void updateLabels();
    void saveSettings();
    void loadSettings();

private:
    void setupUI();

    QSlider *lowSlider;
    QSlider *midSlider;
    QSlider *highSlider;
    QLabel *lowLabel;
    QLabel *midLabel;
    QLabel *highLabel;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *resetButton;

    static double lastLowGain;
    static double lastMidGain;
    static double lastHighGain;
    static bool settingsLoaded;
};

#endif // EQUALIZERDIALOG_H
