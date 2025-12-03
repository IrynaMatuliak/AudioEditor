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
    void on_OKCreateProjectButton_clicked();
    void on_cancelButton_clicked();
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
    void createAudioFile(const QString &filePath, int durationSec, int sampleRate, int format);
<<<<<<< HEAD
=======
    void createMp3File(const QString &filePath, int durationSec, int sampleRate);
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
};

#endif // CREATEPROJECTWINDOW_H
