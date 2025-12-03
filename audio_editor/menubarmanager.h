#ifndef MENUBARMANAGER_H
#define MENUBARMANAGER_H

#include <QObject>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <sndfile.h>
#include <lame.h>

class ProjectWorkWindow;

class MenuBarManager : public QObject
{
    Q_OBJECT

public:
    explicit MenuBarManager(ProjectWorkWindow *parent = nullptr);
    ~MenuBarManager();

    QMenuBar* getMenuBar() const { return menuBar; }
    QString getProjectPath() const { return projectPath; }

private slots:
    void onNewFile();
    void onOpenFile();
    void onClose();
    void onShowInfo();
    void onSave();
    void onSaveAs();
    void onSaveSelectionAs();
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onDelete();
    void onEqualizer();
    void onFadeOut();
    void onFadeIn();
    void onReverse();
    void onNoiseReduction();
    void onEcho();
    void onPlayPause();
    void onRewind();
    void onFastForward();
    void onReset();
    void onToggleLoop();
    void onZoomIn();
    void onZoomOut();

private:
    void createFileMenu();
    void createEditMenu();
    void createEffectMenu();
    void createViewMenu();
    void createControlMenu();
    bool saveWithTrim(const QString &inputPath, const QString &outputPath, qint64 startMs, qint64 endMs);
    QString getFormatFromSFInfo(const SF_INFO& info);

    QMenuBar *menuBar;
    ProjectWorkWindow *mainWindow;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *effectMenu;
    QMenu *viewMenu;
    QMenu *controlMenu;
    QString projectName;
    int projectDuration;
    QString projectSamplingRate;
    QString projectFormat;
    QString projectPath;
};

#endif // MENUBARMANAGER_H
