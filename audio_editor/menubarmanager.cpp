// menubarmanager.cpp
#include "menubarmanager.h"
#include "projectworkwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QApplication>

MenuBarManager::MenuBarManager(ProjectWorkWindow *parent)
    : QObject(parent)
    , mainWindow(parent)
{
    menuBar = new QMenuBar();
    menuBar->setStyleSheet(
        "QMenuBar {"
        "   background-color: #0F0F0F;"
        "   color: white;"
        "   border: 0.5px solid #242323;"
        "}"
        "QMenuBar::item {"
        "   background-color: transparent;"
        "   color: white;"
        "   padding: 4px 10px;"
        "}"
        "QMenuBar::item:selected {"
        "   background-color: #1C5CBA;"
        "}"
        "QMenuBar::item:pressed {"
        "   background-color: #114999;"
        "}"
        "QMenu {"
        "   background-color: #0F0F0F;"
        "   color: white;"
        "   border: 1px solid #242323;"
        "}"
        "QMenu::item {"
        "   background-color: transparent;"
        "   color: white;"
        "   padding: 4px 20px;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #1C5CBA;"
        "}"
        "QMenu::item:disabled {"
        "   color: #888;"
        "}"
        "QMenu::separator {"
        "   height: 1px;"
        "   background-color: #555;"
        "   margin: 4px 0;"
        "}"
        );

    createFileMenu();
    createEditMenu();
    createEffectMenu();
    createViewMenu();
    createControlMenu();
}

MenuBarManager::~MenuBarManager()
{
    delete menuBar;
}

void MenuBarManager::createFileMenu()
{
    fileMenu = menuBar->addMenu("&File");

    // New
    QAction *newAction = fileMenu->addAction("&New...");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MenuBarManager::onNewFile);

    // Open
    QAction *openAction = fileMenu->addAction("&Open...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MenuBarManager::onOpenFile);

    fileMenu->addSeparator();

    // Close
    QAction *closeAction = fileMenu->addAction("&Close");
    closeAction->setShortcut(QKeySequence::Close);
    connect(closeAction, &QAction::triggered, this, &MenuBarManager::onClose);

    fileMenu->addSeparator();

    // Information
    QAction *infoAction = fileMenu->addAction("&Information...");
    connect(infoAction, &QAction::triggered, this, &MenuBarManager::onShowInfo);

    fileMenu->addSeparator();

    // Save
    QAction *saveAction = fileMenu->addAction("&Save");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MenuBarManager::onSave);

    // Save As
    QAction *saveAsAction = fileMenu->addAction("Save &As...");
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAction, &QAction::triggered, this, &MenuBarManager::onSaveAs);

    // Save Selection As
    QAction *saveSelectionAction = fileMenu->addAction("Save Selection &As...");
    connect(saveSelectionAction, &QAction::triggered, this, &MenuBarManager::onSaveSelectionAs);

    fileMenu->addSeparator();

    // Exit
    QAction *exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
}

void MenuBarManager::onNewFile()
{
    if (mainWindow) {
        mainWindow->newFile();
    }
}

void MenuBarManager::onOpenFile()
{
    if (mainWindow) {
        mainWindow->openFile();
    }
}

void MenuBarManager::onClose()
{
    if (mainWindow) {
        mainWindow->closeFile();
    }
}

void MenuBarManager::onShowInfo()
{
    if (mainWindow) {
        mainWindow->showInfo();
    }
}

void MenuBarManager::onSave()
{
    if (mainWindow) {
        mainWindow->save();
    }
}

void MenuBarManager::onSaveAs()
{
    if (mainWindow) {
        mainWindow->saveAs();
    }
}

void MenuBarManager::onSaveSelectionAs()
{
    if (mainWindow) {
        mainWindow->saveSelectionAs();
    }
}

void MenuBarManager::createEditMenu()
{
    editMenu = menuBar->addMenu("&Edit");

    // Undo
    QAction *undoAction = editMenu->addAction("&Undo");
    undoAction->setShortcut(QKeySequence::Undo);
    connect(undoAction, &QAction::triggered, this, &MenuBarManager::onUndo);

    // Redo
    QAction *redoAction = editMenu->addAction("&Redo");
    redoAction->setShortcut(QKeySequence::Redo);
    connect(redoAction, &QAction::triggered, this, &MenuBarManager::onRedo);

    editMenu->addSeparator();

    // Cut
    QAction *cutAction = editMenu->addAction("&Cut");
    cutAction->setShortcut(QKeySequence::Cut);
    connect(cutAction, &QAction::triggered, this, &MenuBarManager::onCut);

    // Copy
    QAction *copyAction = editMenu->addAction("&Copy");
    copyAction->setShortcut(QKeySequence::Copy);
    connect(copyAction, &QAction::triggered, this, &MenuBarManager::onCopy);

    // Paste
    QAction *pasteAction = editMenu->addAction("&Paste");
    pasteAction->setShortcut(QKeySequence::Paste);
    connect(pasteAction, &QAction::triggered, this, &MenuBarManager::onPaste);

    editMenu->addSeparator();

    // Delete
    QAction *deleteAction = editMenu->addAction("&Delete");
    deleteAction->setShortcut(QKeySequence::Delete);
    connect(deleteAction, &QAction::triggered, this, &MenuBarManager::onDelete);
}

void MenuBarManager::onUndo()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->undo();
}

void MenuBarManager::onRedo()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->redo();
}

void MenuBarManager::onCut()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->cut();
}

void MenuBarManager::onCopy()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->copy();
}

void MenuBarManager::onPaste()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->paste();
}

void MenuBarManager::onDelete()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->deleteAction();
}

void MenuBarManager::createEffectMenu()
{
    effectMenu = menuBar->addMenu("&Effect");

    QAction *equalizerAction = effectMenu->addAction("&Equalizer");
    connect(equalizerAction, &QAction::triggered, this, &MenuBarManager::onEqualizer);

    QAction *fadeOutAction = effectMenu->addAction("&Fade Out");
    connect(fadeOutAction, &QAction::triggered, this, &MenuBarManager::onFadeOut);

    QAction *fadeInAction = effectMenu->addAction("&Fade In");
    connect(fadeInAction, &QAction::triggered, this, &MenuBarManager::onFadeIn);

    QAction *reverseAction = effectMenu->addAction("&Reverse");
    connect(reverseAction, &QAction::triggered, this, &MenuBarManager::onReverse);

    QAction *noiseReductionAction = effectMenu->addAction("&Noise Reduction");
    connect(noiseReductionAction, &QAction::triggered, this, &MenuBarManager::onNoiseReduction);

    QAction *echoAction = effectMenu->addAction("&Echo");
    connect(echoAction, &QAction::triggered, this, &MenuBarManager::onEcho);
}

void MenuBarManager::onEqualizer()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->applyEqualizerEffect();
}

void MenuBarManager::onFadeOut()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->applyFadeOut();
}

void MenuBarManager::onFadeIn()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->applyFadeIn();
}

void MenuBarManager::onReverse()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->reverse();
}

void MenuBarManager::onNoiseReduction()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->applyNoiseReduction();
}

void MenuBarManager::onEcho()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->applyEchoEffect();
}

void MenuBarManager::createViewMenu()
{
    viewMenu = menuBar->addMenu("&View");
    // Zoom In
    QAction *zoomInAction = viewMenu->addAction("&Zoom In");
    zoomInAction->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAction, &QAction::triggered, this, &MenuBarManager::onZoomIn);

    // Zoom Out
    QAction *zoomOutAction = viewMenu->addAction("Zoom &Out");
    zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAction, &QAction::triggered, this, &MenuBarManager::onZoomOut);

    viewMenu->addSeparator();
}

void MenuBarManager::onZoomIn()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
<<<<<<< HEAD
    mainWindow->zoomIn();
=======
    //mainWindow->zoomIn();
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
}

void MenuBarManager::onZoomOut()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
<<<<<<< HEAD
    mainWindow->zoomOut();
=======
    //mainWindow->zoomOut();
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
}

void MenuBarManager::createControlMenu()
{
    controlMenu = menuBar->addMenu("&Control");

    // Play/Pause
    QAction *playPauseAction = controlMenu->addAction("Play/Pause");
    playPauseAction->setShortcut(QKeySequence("Space"));
    connect(playPauseAction, &QAction::triggered, this, &MenuBarManager::onPlayPause);

    controlMenu->addSeparator();

    // Rewind
    QAction *rewindAction = controlMenu->addAction("&Rewind");
    rewindAction->setShortcut(QKeySequence("Left"));
    connect(rewindAction, &QAction::triggered, this, &MenuBarManager::onRewind);

    // Fast Forward
    QAction *fastForwardAction = controlMenu->addAction("&Fast Forward");
    fastForwardAction->setShortcut(QKeySequence("Right"));
    connect(fastForwardAction, &QAction::triggered, this, &MenuBarManager::onFastForward);

    controlMenu->addSeparator();

    // Reset
    QAction *resetAction = controlMenu->addAction("&Reset");
    resetAction->setShortcut(QKeySequence("R"));
    connect(resetAction, &QAction::triggered, this, &MenuBarManager::onReset);

    // Loop/Repeat
    QAction *loopAction = controlMenu->addAction("&Loop");
    loopAction->setCheckable(true);
    loopAction->setShortcut(QKeySequence("L"));
    connect(loopAction, &QAction::toggled, this, &MenuBarManager::onToggleLoop);

    controlMenu->addSeparator();
}

void MenuBarManager::onPlayPause()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->togglePlayPause();
}

void MenuBarManager::onRewind()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->rewind();
}

void MenuBarManager::onFastForward()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->fastForward();
}

void MenuBarManager::onReset()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->reset();
}

void MenuBarManager::onToggleLoop()
{
    if (!mainWindow) {
        QMessageBox::warning(nullptr, "Error", "No main window available");
        return;
    }
    mainWindow->repeat();
}
