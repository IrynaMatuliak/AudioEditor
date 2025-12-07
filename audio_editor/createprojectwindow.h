#ifndef CREATEPROJECTWINDOW_H
#define CREATEPROJECTWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>

namespace Ui {
class CreateProjectWindow;
}

class CreateProjectWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CreateProjectWindow(QWidget *parent = nullptr);
    ~CreateProjectWindow();

private slots:
    void validateProjectName();
    void validateDuration();
    void browseForProjectFolder();

private:
    Ui::CreateProjectWindow *ui;
    QLabel *projectNameError;
    QLabel *durationError;
    QString projectFolderPath;

    void setupMainLayout();
    void setupSizePolicies();
    bool createAudioFile(const QString &filePath, int durationSec, int sampleRate, int format);

    void on_OKButton_clicked();
    void on_cancelButton_clicked();
};

#endif // CREATEPROJECTWINDOW_H
