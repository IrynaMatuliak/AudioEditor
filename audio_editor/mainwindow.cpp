// mainwindow.cpp
#include "mainwindow.h"
#include "createprojectwindow.h"
#include "projectworkwindow.h"
#include "helpwindow.h"
#include "./ui_mainwindow.h"

#include <QSettings>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QSpacerItem>
#include <sndfile.h>
#include <lame.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tmpDirectory()
{
    ui->setupUi(this);

    QString messageBoxStyle =
        "QMessageBox {"
        "   background-color: #333333;"
        "   color: white;"
        "   border: 1px solid #555555;"
        "}"
        "QMessageBox QLabel {"
        "   color: white;"
        "   padding: 5px;"
        "}"
        "QMessageBox QPushButton {"
        "   background-color: #555555;"
        "   color: white;"
        "   border: 1px solid #777777;"
        "   padding: 8px 15px;"
        "   min-width: 70px;"
        "   border-radius: 3px;"
        "   font-weight: bold;"
        "}"
        "QMessageBox QPushButton:hover {"
        "   background-color: #666666;"
        "   border-color: #888888;"
        "}"
        "QMessageBox QPushButton:pressed {"
        "   background-color: #444444;"
        "   border-color: #666666;"
        "}"
        "QMessageBox QPushButton:focus {"
        "   outline: none;"
        "   border: 2px solid #0078d7;"
        "}";
    qApp->setStyleSheet(messageBoxStyle);
    
    QCoreApplication::setOrganizationName("KPI");
    QCoreApplication::setOrganizationDomain("aeditor.kpi.edu.ua");
    QCoreApplication::setApplicationName("Audio Editor");

    QSettings appSettings;
    appSettings.setValue("project/tempFilePath", tmpDirectory.path());
    if (!tmpDirectory.isValid()) {
        //error?
        QMessageBox::critical(nullptr,
                              "Error",
                              "Failed to create temporary directory.");
        return;
    }

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(80, 40, 80, 40);
    mainLayout->setSpacing(0);

    mainLayout->addStretch(0);

    QVBoxLayout *headerLayout = new QVBoxLayout();
    headerLayout->setAlignment(Qt::AlignLeft);
    headerLayout->setSpacing(15);

    QLabel *titleLabel = new QLabel("Audio Editor Application");
    titleLabel->setObjectName("mainTitle");
    titleLabel->setStyleSheet("color: white; font-size: 36px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignLeft);

    QLabel *subtitleLabel = new QLabel("Original Audio Editing Software");
    subtitleLabel->setObjectName("mainSubtitle");
    subtitleLabel->setStyleSheet("color: #CCCCCC; font-size: 16px;");
    subtitleLabel->setAlignment(Qt::AlignLeft);
    subtitleLabel->setWordWrap(true);

    headerLayout->addWidget(titleLabel);
    headerLayout->addWidget(subtitleLabel);

    mainLayout->addLayout(headerLayout);
    mainLayout->addStretch(2);

    QVBoxLayout *welcomeLayout = new QVBoxLayout();
    welcomeLayout->setAlignment(Qt::AlignCenter);
    welcomeLayout->setSpacing(10);

    QLabel *welcomeTitle = new QLabel("Welcome to Audio Editor!");
    welcomeTitle->setObjectName("welcomeTitle");
    welcomeTitle->setStyleSheet("color: white; font-size: 24px; font-weight: bold;");
    welcomeTitle->setAlignment(Qt::AlignCenter);

    QLabel *welcomeText = new QLabel("Create, edit, and enhance your audio projects easily.\nStart by creating a new project or importing an existing one.");
    welcomeText->setObjectName("welcomeText");
    welcomeText->setStyleSheet("color: #CCCCCC; font-size: 14px;");
    welcomeText->setAlignment(Qt::AlignCenter);
    welcomeText->setWordWrap(true);

    welcomeLayout->addWidget(welcomeTitle);
    welcomeLayout->addWidget(welcomeText);

    mainLayout->addLayout(welcomeLayout);
    mainLayout->addStretch(1);

    QWidget *buttonsContainer = new QWidget(centralWidget);
    QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsContainer);
    buttonsLayout->setSpacing(40);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    buttonsLayout->addWidget(ui->createProjectButton);
    buttonsLayout->addWidget(ui->importProjectButton);
    buttonsLayout->addWidget(ui->helpMainButton);

    setupProjectButton(ui->createProjectButton, ":/icons/images/plus.png", "Create New Project", "Start a new audio project from scratch");
    setupProjectButton(ui->importProjectButton, ":/icons/images/import.png", "Import Project", "Open an existing audio project");
    setupProjectButton(ui->helpMainButton, ":/icons/images/help.png", "Help & Support", "Get help and documentation");

    mainLayout->addWidget(buttonsContainer);
    mainLayout->addStretch(3);

    QLabel *versionLabel = new QLabel("version 1.0.0");
    versionLabel->setObjectName("versionLabel");
    versionLabel->setStyleSheet("color: #666666; font-size: 12px;");
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setContentsMargins(0, 10, 0, 0);

    mainLayout->addWidget(versionLabel);

    setCentralWidget(centralWidget);

    setMinimumSize(1170, 685);
}

void MainWindow::setupProjectButton(QToolButton *button, const QString &iconPath, const QString &title, const QString &subtitle)
{
    button->setMinimumSize(280, 120);
    button->setMaximumSize(500, 250);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (button->layout()) {
        QLayoutItem* item;
        while ((item = button->layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete button->layout();
    }

    QWidget *content = new QWidget(button);
    content->setAttribute(Qt::WA_TransparentForMouseEvents);
    content->setStyleSheet("background: transparent;");

    QVBoxLayout *contentLayout = new QVBoxLayout(content);
    contentLayout->setAlignment(Qt::AlignLeft);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(15);

    QLabel *iconLabel = new QLabel;
    iconLabel->setAlignment(Qt::AlignLeft);
    iconLabel->setMinimumSize(60, 60);
    iconLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QPixmap pixmap(iconPath);
    if (pixmap.isNull()) {
        iconLabel->setText("File not found");
        iconLabel->setStyleSheet("color: white; font-size: 40px;");
    } else {
        QPixmap scaledPixmap = pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        iconLabel->setPixmap(scaledPixmap);
        iconLabel->setFixedSize(scaledPixmap.size());
    }

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("buttonTitle");
    titleLabel->setStyleSheet("color: white; font-weight: bold; font-size: 18px;");
    titleLabel->setAlignment(Qt::AlignLeft);
    titleLabel->setWordWrap(true);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QLabel *subtitleLabel = new QLabel(subtitle);
    subtitleLabel->setObjectName("buttonSubtitle");
    subtitleLabel->setStyleSheet("color: #999999; font-size: 13px;");
    subtitleLabel->setAlignment(Qt::AlignLeft);
    subtitleLabel->setWordWrap(true);
    subtitleLabel->setMinimumHeight(35);
    subtitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(titleLabel);
    contentLayout->addWidget(subtitleLabel);

    contentLayout->setStretchFactor(iconLabel, 1);
    contentLayout->setStretchFactor(titleLabel, 1);
    contentLayout->setStretchFactor(subtitleLabel, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout(button);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addWidget(content);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    int width = this->width();

    QLabel *titleLabel = findChild<QLabel*>("mainTitle");
    if (titleLabel) {
        int titleFontSize = qMax(24, qMin(48, width / 30));
        titleLabel->setStyleSheet(QString("color: white; font-size: %1px; font-weight: bold;").arg(titleFontSize));
    }

    QLabel *subtitleLabel = findChild<QLabel*>("mainSubtitle");
    if (subtitleLabel) {
        int subtitleFontSize = qMax(12, qMin(20, width / 80));
        subtitleLabel->setStyleSheet(QString("color: #CCCCCC; font-size: %1px;").arg(subtitleFontSize));
    }

    QLabel *welcomeTitle = findChild<QLabel*>("welcomeTitle");
    if (welcomeTitle) {
        int welcomeTitleFontSize = qMax(18, qMin(32, width / 50));
        welcomeTitle->setStyleSheet(QString("color: white; font-size: %1px; font-weight: bold;").arg(welcomeTitleFontSize));
    }

    QLabel *welcomeText = findChild<QLabel*>("welcomeText");
    if (welcomeText) {
        int welcomeTextFontSize = qMax(12, qMin(18, width / 90));
        welcomeText->setStyleSheet(QString("color: #CCCCCC; font-size: %1px;").arg(welcomeTextFontSize));
    }

    QList<QLabel*> buttonTitles = findChildren<QLabel*>("buttonTitle");
    for (QLabel *title : buttonTitles) {
        int buttonTitleSize = qMax(14, qMin(22, width / 70));
        title->setStyleSheet(QString("color: white; font-weight: bold; font-size: %1px;").arg(buttonTitleSize));
    }

    QList<QLabel*> buttonSubtitles = findChildren<QLabel*>("buttonSubtitle");
    for (QLabel *subtitle : buttonSubtitles) {
        int buttonSubtitleSize = qMax(11, qMin(16, width / 100));
        subtitle->setStyleSheet(QString("color: #999999; font-size: %1px;").arg(buttonSubtitleSize));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_createProjectButton_clicked()
{
    this->close();
    CreateProjectWindow *dlg = new CreateProjectWindow(this);
    dlg->show();
}

void MainWindow::on_importProjectButton_clicked()
{
    QSettings appSettings;

    QString homePathLast = appSettings.value("project/homePathLast", "").toString();
    if (homePathLast=="") homePathLast = QDir::homePath();

    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Open Audio Project"),
                                                    homePathLast,
                                                    tr("Audio Files (*.wav *.mp3 *.flac);;All Files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(filePath);
    homePathLast = fileInfo.absolutePath();

    if (homePathLast != "") {
        appSettings.setValue("project/homePathLast", homePathLast);
    }

    SF_INFO sfinfo;
    SNDFILE* file = sf_open(filePath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!file) {
        QMessageBox::critical(this, "Error", "Failed to open audio file");
        return;
    }

    QString samplingRate = QString::number(sfinfo.samplerate / 1000.0, 'f', 1) + " kHz";
    QString format;

    switch (sfinfo.format & SF_FORMAT_TYPEMASK) {
    case SF_FORMAT_WAV: format = "WAV"; break;
    case SF_FORMAT_FLAC: format = "FLAC"; break;
    case SF_FORMAT_MPEG: format = "MP3"; break;
    default: format = "Unknown"; break;
    }

    sf_close(file);

    this->close();
    ProjectWorkWindow *workWindow = new ProjectWorkWindow();
    workWindow->initializeProject(QFileInfo(filePath).baseName(),
                                  sfinfo.frames / double(sfinfo.samplerate),
                                  sfinfo.samplerate,
                                  sfinfo.channels,
                                  format,
                                  filePath);
    workWindow->show();
}

void MainWindow::on_helpMainButton_clicked()
{
    hide();
    HelpWindow *helpWindow = new HelpWindow(this, this);
    helpWindow->setAttribute(Qt::WA_DeleteOnClose);
    helpWindow->show();
    helpWindow->raise();
    helpWindow->activateWindow();
}
