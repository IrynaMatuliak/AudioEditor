#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QListWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QMap>

class HelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HelpWindow(QWidget *parent = nullptr, QWidget *previousWindow = nullptr);
    ~HelpWindow();

private slots:
    void onTopicSelected(QListWidgetItem *item);
    void onBackButtonClicked();

private:
    void setupUI();
    void setupContent();
    void createContentSection(const QString &title, const QString &content, QVBoxLayout *layout);

    QSplitter *mainSplitter;
    QListWidget *topicsList;
    QScrollArea *contentScrollArea;
    QWidget *contentWidget;
    QVBoxLayout *contentLayout;
    QPushButton *backButton;

    QMap<QString, QString> contentMap;
    QWidget *m_previousWindow;
};

#endif // HELPWINDOW_H
