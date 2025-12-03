#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTemporaryDir>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_createProjectButton_clicked();
    void on_importProjectButton_clicked();

    void on_helpMainButton_clicked();

private:
    Ui::MainWindow *ui;
    QTemporaryDir tmpDirectory;
    
    void setupHeaderSpacing();
    void setupProjectButton(QToolButton *button, const QString &iconPath, const QString &title, const QString &subtitle);
};
#endif // MAINWINDOW_H
