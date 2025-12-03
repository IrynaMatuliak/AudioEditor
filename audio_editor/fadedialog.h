#ifndef FADEDIALOG_H
#define FADEDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>

class FadeDialog : public QDialog
{
    Q_OBJECT

public:
    enum FadeType {
        FadeIn,
        FadeOut
    };

    explicit FadeDialog(FadeType type, QWidget *parent = nullptr);

    double getFadeDuration() const;
    QString getCurveType() const;
    bool applyToSelection() const;

private slots:
    void onOkClicked();
    void onCancelClicked();

private:
    void setupUI(FadeType type);
    void setupStyles();

    QDoubleSpinBox *durationSpinBox;
    QComboBox *curveComboBox;
    QComboBox *applyComboBox;
    FadeType fadeType;
};

#endif // FADEDIALOG_H
