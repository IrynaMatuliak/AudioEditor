// projectworkwindow.cpp
#include "projectworkwindow.h"
#include "createprojectwindow.h"
#include "mainwindow.h"
#include "helpwindow.h"
#include "echodialog.h"
#include "equalizerdialog.h"
#include "noisereductiondialog.h"
#include "fadedialog.h"
#include "ui_projectworkwindow.h"

#include "AudioEffects/cechoeffect.h"
#include "AudioEffects/cequalizereffect.h"
#include "AudioEffects/cnoisereductioneffect.h"
#include "AudioEffects/cfadeineffect.h"
#include "AudioEffects/cfadeouteffect.h"

#include <sndfile.h>
#include <lame.h>
#include <QSettings>
#include <QFileInfo>
#include <QTime>
#include <QDebug>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QMessageBox>
#include <QAudioDecoder>
#include <QAudioBuffer>
#include <QProcess>
#include <QDir>
#include <QBuffer>
#include <QEventLoop>
#include <QTimer>
#include <QMediaDevices>
#include <QAudioInput>
#include <QMediaCaptureSession>
#include <QMediaRecorder>
#include <QMediaFormat>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QtWidgets>

namespace {
QString getPositionLabel(double position, double duration)
{
    QString posStr;
    if (duration < 60.0*60.0) {
        posStr = QTime::fromMSecsSinceStartOfDay(position * 1000).toString("mm:ss");
    } else {
        posStr = QTime::fromMSecsSinceStartOfDay(position * 1000).toString("h:mm:ss");
    }
    return posStr;
}
}

ProjectWorkWindow::ProjectWorkWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProjectWorkWindow)
    , player(new QMediaPlayer(this))
    , audioOutput(new QAudioOutput(this))
    , waveformWidget(new WaveformWidget(this))
    , menuManager(new MenuBarManager(this))
    , captureSession(new QMediaCaptureSession(this))
    , audioInput(new QAudioInput(this))
    , mediaRecorder(new QMediaRecorder(this))
    , isPlaying(false)
    , isRepeatOn(false)
    , isTrimmed(false)
    , trimStartMs(0)
    , trimEndMs(0)
    , currentHistoryIndex(-1)
    , hasCopiedData(false)
    , hasCutData(false)
    , lastActionWasCut(false)
    , isRecording(false)
    , recordStartPosition(0)
    , recordEndPosition(0)
{
    ui->setupUi(this);
    initializeComponents();
    setupUI();
    setupMainLayout();
    connectSignals();
    setupStyles();
}

void ProjectWorkWindow::setupMainLayout()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 10, 15, 10);

    // Edit buttons
    setupTopButtonsWithLabels(mainLayout);

    // Track title
    QHBoxLayout *titleLayout = new QHBoxLayout();
    ui->projectNameLabel->setAlignment(Qt::AlignLeft);
    ui->projectNameLabel->setStyleSheet("QLabel { color: white; font-size: 20px; font-weight: bold; padding: 10px; }");

    ui->durationLabel->setEnabled(true);
    ui->durationLabel->setAlignment(Qt::AlignRight);
    ui->durationLabel->setStyleSheet("QLabel { color: white; font-size: 20px; font-weight: bold; padding: 10px; }");

    titleLayout->addWidget(ui->projectNameLabel);
    titleLayout->addWidget(ui->durationLabel);

    mainLayout->addLayout(titleLayout);

    setupWaveformArea(mainLayout);

    setupTimelineSlider(mainLayout);

    setupBottomControls(mainLayout);
}

void ProjectWorkWindow::setupTopButtonsWithLabels(QVBoxLayout *mainLayout)
{
    QVBoxLayout *topButtonsMainLayout = new QVBoxLayout();
    topButtonsMainLayout->setSpacing(5);

    QHBoxLayout *topButtonsLayout = new QHBoxLayout();
    topButtonsLayout->setSpacing(15);

    // topButtonsLayout->addStretch(1);

    createButtonWithLabel(topButtonsLayout, ui->undoButton, "Undo");
    createButtonWithLabel(topButtonsLayout, ui->redoButton, "Redo");
    createButtonWithLabel(topButtonsLayout, ui->trimButton, "Trim");
    createButtonWithLabel(topButtonsLayout, ui->cutButton, "Cut");
    createButtonWithLabel(topButtonsLayout, ui->copyButton, "Copy");
    createButtonWithLabel(topButtonsLayout, ui->pasteButton, "Paste");
    createButtonWithLabel(topButtonsLayout, ui->deleteButton, "Delete");
    createButtonWithLabel(topButtonsLayout, ui->recordButton, "Record");
    createButtonWithLabel(topButtonsLayout, ui->zoomInButton, "Zoom In");
    createButtonWithLabel(topButtonsLayout, ui->zoomOutButton, "Zoom Out");
    createButtonWithLabel(topButtonsLayout, ui->helpButton, "Help");

    topButtonsLayout->addStretch(1);

    topButtonsMainLayout->addLayout(topButtonsLayout);
    mainLayout->addLayout(topButtonsMainLayout);
}

void ProjectWorkWindow::createButtonWithLabel(QHBoxLayout *layout, QPushButton *button, const QString &labelText)
{
    QWidget *container = new QWidget();
    container->setFixedSize(60, 65);

    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setSpacing(3);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setAlignment(Qt::AlignCenter);
    button->setFixedSize(45, 45);
    containerLayout->addWidget(button, 0, Qt::AlignHCenter);

    QLabel *label = new QLabel(labelText);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("QLabel { "
                         "color: #cccccc; "
                         "font-size: 11px; "
                         "font-weight: none; "
                         "background: transparent; "
                         "padding: 0px; "
                         "margin: 0px; "
                         "}");
    label->setFixedSize(55, 15);
    containerLayout->addWidget(label, 0, Qt::AlignHCenter);
    layout->addWidget(container);
}

void ProjectWorkWindow::setupTimelineSlider(QVBoxLayout *mainLayout)
{
    QHBoxLayout *timelineLayout = new QHBoxLayout();
    timelineLayout->setSpacing(10);

    timelineLayout->addWidget(ui->timelineSlider, 1);
    ui->currentTimeLabel->setVisible(true);
    ui->currentTimeLabel->setStyleSheet("QLabel { color: white; font-size: 12px; font-weight: bold; min-width: 40px; }");
    timelineLayout->addWidget(ui->currentTimeLabel);

    mainLayout->addLayout(timelineLayout);
}

void ProjectWorkWindow::setupWaveformArea(QVBoxLayout *mainLayout)
{
    waveformLayout = new QHBoxLayout();
    waveformLayout->setSpacing(5);
    waveformWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    waveformWidget->setMinimumHeight(250);
    waveformLayout->addWidget(waveformWidget, 1);
    mainLayout->addLayout(waveformLayout, 1);
}

void ProjectWorkWindow::setupBottomControls(QVBoxLayout *mainLayout)
{
    QHBoxLayout *bottomControlsLayout = new QHBoxLayout();
    bottomControlsLayout->setSpacing(15);
    bottomControlsLayout->setContentsMargins(0, 10, 0, 0);
    bottomControlsLayout->addStretch(1);

    QHBoxLayout *speedLayout = new QHBoxLayout();
    speedLayout->setSpacing(10);

    QIcon speedIcon(":/icons/images/speed.png");
    ui->speedImageLabel->setPixmap(speedIcon.pixmap(30, 30));
    speedLayout->addWidget(ui->speedImageLabel);

    ui->speedSlider->setRange(0, 3);
    ui->speedSlider->setValue(1);
    ui->speedSlider->setFixedWidth(150);
    speedLayout->addWidget(ui->speedSlider);

    ui->speedLabel->setText("1x");
    ui->speedLabel->setStyleSheet("QLabel { color: white; font-size: 14px; min-width: 25px; }");
    speedLayout->addWidget(ui->speedLabel);

    bottomControlsLayout->addLayout(speedLayout);
    bottomControlsLayout->addSpacing(20);

    bottomControlsLayout->addWidget(ui->resetButton);
    bottomControlsLayout->addWidget(ui->rewindButton);
    bottomControlsLayout->addWidget(ui->playPauseButton);
    bottomControlsLayout->addWidget(ui->fastForwardButton);
    bottomControlsLayout->addWidget(ui->repeatButton);
    bottomControlsLayout->addSpacing(20);

    QHBoxLayout *volumeLayout = new QHBoxLayout();
    volumeLayout->setSpacing(5);
    updateVolumeIcon(50);
    ui->volumeImageLabel->setFixedSize(30, 30);
    volumeLayout->addWidget(ui->volumeImageLabel);
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(50);
    ui->volumeSlider->setFixedWidth(150);
    volumeLayout->addWidget(ui->volumeSlider);
    ui->volumeLabel->setText("50%");
    ui->volumeLabel->setStyleSheet("QLabel { color: white; font-size: 14px; min-width: 35px; }");
    volumeLayout->addWidget(ui->volumeLabel);
    bottomControlsLayout->addLayout(volumeLayout);
    bottomControlsLayout->addStretch(1);

    mainLayout->addLayout(bottomControlsLayout);
}

void ProjectWorkWindow::initializeComponents()
{
    setMenuBar(menuManager->getMenuBar());
    player->setAudioOutput(audioOutput);

    captureSession->setAudioInput(audioInput);
    captureSession->setRecorder(mediaRecorder);

    ui->currentTimeLabel->setVisible(false);
}

void ProjectWorkWindow::setupUI()
{
    setupButton(ui->resetButton, ":/icons/images/reset.png", QSize(38, 38));
    setupButton(ui->rewindButton, ":/icons/images/rewind.png", QSize(43, 43));
    setupButton(ui->fastForwardButton, ":/icons/images/fast_forward.png", QSize(43, 43));
    setupButton(ui->repeatButton, ":/icons/images/repeat.png", QSize(48, 48));
    setupButton(ui->playPauseButton, ":/icons/images/pause.png", QSize(40, 40));
    setupButton(ui->undoButton, ":/icons/images/undo.png", QSize(30, 30));
    setupButton(ui->redoButton, ":/icons/images/redo.png", QSize(30, 30));
    setupButton(ui->trimButton, ":/icons/images/trim.png", QSize(40, 40));
    setupButton(ui->cutButton, ":/icons/images/cut.png", QSize(40, 40));
    setupButton(ui->copyButton, ":/icons/images/copy.png", QSize(40, 40));
    setupButton(ui->deleteButton, ":/icons/images/delete.png", QSize(55, 55));
    setupButton(ui->pasteButton, ":/icons/images/paste.png", QSize(70, 70));
    setupButton(ui->recordButton, ":/icons/images/record.png", QSize(50, 50));
    setupButton(ui->zoomInButton, ":/icons/images/zoom_in.png", QSize(40, 40));
    setupButton(ui->zoomOutButton, ":/icons/images/zoom_out.png", QSize(40, 40));
    setupButton(ui->helpButton, ":/icons/images/help_button.png", QSize(40, 40));

    ui->undoButton->setEnabled(false);
    ui->redoButton->setEnabled(false);

    setupSpeedControls();
    setupVolumeControls();

    ui->currentTimeLabel->setText("00:00");
    ui->durationLabel->setVisible(true);
}

void ProjectWorkWindow::setupButton(QPushButton* button, const QString& iconPath, const QSize& iconSize)
{
    QIcon icon(iconPath);
    button->setIcon(icon);
    button->setIconSize(iconSize);
    button->setText("");
    button->setStyleSheet("QPushButton { border: none; }");
}

void ProjectWorkWindow::setupSpeedControls()
{
    QIcon speedIcon(":/icons/images/speed.png");
    ui->speedImageLabel->setPixmap(speedIcon.pixmap(45, 45));
    ui->speedSlider->setRange(0, 3);
    ui->speedSlider->setValue(1);
    ui->speedSlider->setSingleStep(1);
    ui->speedSlider->setPageStep(1);
    ui->speedSlider->setTickInterval(1);
    ui->speedLabel->setText("1x");
    player->setPlaybackRate(1.0);
}

void ProjectWorkWindow::setupVolumeControls()
{
    updateVolumeIcon(50);
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(50);
    ui->volumeLabel->setText("50%");
    audioOutput->setVolume(0.5);
}

void ProjectWorkWindow::connectSignals()
{
    connect(player, &QMediaPlayer::positionChanged, this, &ProjectWorkWindow::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &ProjectWorkWindow::updateDuration);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &ProjectWorkWindow::on_playbackStateChanged);
    connect(player, &QMediaPlayer::positionChanged, waveformWidget, &WaveformWidget::setPosition);
    connect(ui->resetButton, &QPushButton::clicked, this, &ProjectWorkWindow::on_resetButton_clicked);
    connect(ui->timelineSlider, &QSlider::sliderMoved, player, &QMediaPlayer::setPosition);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &ProjectWorkWindow::on_volumeSlider_valueChanged);
    connect(waveformWidget, &WaveformWidget::userPositionChanged, player, &QMediaPlayer::setPosition);
    connect(mediaRecorder, &QMediaRecorder::recorderStateChanged, this, &ProjectWorkWindow::onRecordingStateChanged);
    connect(mediaRecorder, &QMediaRecorder::errorOccurred, this, &ProjectWorkWindow::onRecordingError);
}

void ProjectWorkWindow::setupStyles()
{
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
}

void ProjectWorkWindow::updateVolumeIcon(int volume)
{
    QString iconName;
    if (volume == 0) {
        iconName = ":/icons/images/volume_level0.png";
    } else if (volume <= 33) {
        iconName = ":/icons/images/volume_level1.png";
    } else if (volume <= 66) {
        iconName = ":/icons/images/volume_level2.png";
    } else {
        iconName = ":/icons/images/volume_level3.png";
    }

    QPixmap pixmap(iconName);
    if (!pixmap.isNull()) {
        ui->volumeImageLabel->setPixmap(pixmap.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        qWarning() << "Failed to load volume icon:" << iconName;
    }
}

void ProjectWorkWindow::closeEvent(QCloseEvent *event)
{
    if (player != nullptr) {
        player->stop();
        player->setSource(QUrl());
    }
    if (event != nullptr) {
        event->accept();
    }
}

ProjectWorkWindow::~ProjectWorkWindow()
{
    player->stop();
    player->setSource(QUrl());

    if (isRecording) {
        mediaRecorder->stop();
    }
    if (QFile::exists(recordedFilePath)) {
        QFile::remove(recordedFilePath);
    }

    delete mediaRecorder;
    delete audioInput;
    delete captureSession;

    cutAudioData.clear();
    hasCutData = false;

    delete player;
    delete audioOutput;
    delete ui;
}

void ProjectWorkWindow::initializeProject(const QString& name, double duration,
                                          int samplingRate, int channels, const QString& format,
                                          const QString& path)
{
    player->stop();
    player->setSource(QUrl());

    // original file is copied to tmp folder
    // to avoid extra logic on 'save'
    //---
    QSettings appSettings;
    QString tmpOrigFileName = createTempFilePath("orig");
    QFile(path).copy(tmpOrigFileName);
    //---

    projectName = name;
    projectDuration = duration;
    projectSamplingRate = samplingRate;
    projectChannelsCount = channels;
    projectFormat = format;
    projectPath = tmpOrigFileName;
    originalFilePath = path;

    undoHistory.clear();
    currentHistoryIndex = -1;

    AudioState initialState;
    initialState.filePath = tmpOrigFileName;
    initialState.startMs = 0;
    initialState.endMs = duration * 1000;
    initialState.timestamp = QDateTime::currentDateTime();

    undoHistory.append(initialState);
    currentHistoryIndex = 0;

    ui->undoButton->setEnabled(false);
    ui->redoButton->setEnabled(false);

    loadAndSetAudioFile(projectPath);

    ui->projectNameLabel->setText(projectName);

    ui->durationLabel->setVisible(true);
    ui->durationLabel->setText(getPositionLabel(projectDuration, projectDuration));
    ui->currentTimeLabel->setText(getPositionLabel(0.0, projectDuration));
    ui->timelineSlider->setRange(0, projectDuration * 1000);

    waveformWidget->setDuration(projectDuration * 1000);
    loadAudioForVisualization(projectPath);
}

void ProjectWorkWindow::loadAndSetAudioFile(const QString& filePath)
{
    player->setSource(QUrl::fromLocalFile(filePath));
    audioOutput->setVolume(0.5);

    if (!waitForMediaLoad(5000)) {
        qWarning() << "Failed to load media file:" << filePath;
        QMessageBox::warning(this, "Error", "Failed to load audio file: " + filePath);
    }
}

void ProjectWorkWindow::clearFutureHistory()
{
    // Delete all states after the current index
    while (undoHistory.size() > currentHistoryIndex + 1) {
        AudioState stateToRemove = undoHistory.last();
        // Delete temporary file if it is not original
        if (stateToRemove.filePath != originalFilePath && QFile::exists(stateToRemove.filePath)) {
            QFile::remove(stateToRemove.filePath);
        }
        undoHistory.removeLast();
    }

    ui->undoButton->setEnabled(currentHistoryIndex > 0);
    ui->redoButton->setEnabled(false);
}

QString ProjectWorkWindow::createTempFilePath(const QString& prefix)
{
    QSettings appSettings;

    QString tempDir = appSettings.value("project/tempFilePath", "").toString();
    QDir folder = QDir(tempDir);
    if (!folder.exists()) {
        return "";
    }

    return tempDir + "/" + prefix + "_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".wav";
}

void ProjectWorkWindow::updateProjectWithNewFile(const QString& newFilePath, qint64 framesCount, int sampleRate)
{
    QString oldProjectPath = projectPath;
    projectPath = newFilePath;
    projectDuration = double(framesCount) / sampleRate;

    AudioState newState;
    newState.filePath = projectPath;
    newState.startMs = 0;
    newState.endMs = projectDuration * 1000;
    newState.timestamp = QDateTime::currentDateTime();

    undoHistory.append(newState);
    currentHistoryIndex++;
    ui->undoButton->setEnabled(true);
    ui->redoButton->setEnabled(false);

    // Update UI
    waveformWidget->setDuration(projectDuration * 1000);
    ui->timelineSlider->setRange(0, projectDuration * 1000);
    ui->durationLabel->setText(getPositionLabel(projectDuration, projectDuration));
    ui->currentTimeLabel->setText(getPositionLabel(0.0, projectDuration));

    loadAndSetAudioFile(projectPath);
    reloadAudioVisualization();
    player->setPosition(0);
}

void ProjectWorkWindow::stopPlayerAndResetUI()
{
    player->stop();
    QIcon pauseIcon(":/icons/images/pause.png");
    ui->playPauseButton->setIcon(pauseIcon);
    isPlaying = false;
}

bool ProjectWorkWindow::validateAudioSelection(qint64 startMs, qint64 endMs)
{
    if (startMs >= endMs) {
        QMessageBox::warning(this, "Error", "Please select a valid audio range");
        return false;
    }
    return true;
}

void ProjectWorkWindow::showSuccessMessage(const QString& message)
{
    QMessageBox::information(this, "Success", message);
}

void ProjectWorkWindow::showErrorMessage(const QString& message)
{
    QMessageBox::critical(this, "Error", message);
}

bool ProjectWorkWindow::waitForMediaLoad(int timeoutMs)
{
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    bool mediaLoaded = false;
    auto connection = connect(player, &QMediaPlayer::mediaStatusChanged, [&](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia || status == QMediaPlayer::BufferedMedia) {
            mediaLoaded = true;
            loop.quit();
        } else if (status == QMediaPlayer::InvalidMedia) {
            qWarning() << "Invalid media file:" << projectPath;
            loop.quit();
        }
    });

    timer.start(timeoutMs);
    loop.exec();
    disconnect(connection);

    return mediaLoaded;
}

void ProjectWorkWindow::reloadAudioVisualization()
{
    loadAudioForVisualization(projectPath);
}

void ProjectWorkWindow::togglePlayPause()
{
    if (isPlaying) {
        player->pause();
        QIcon pauseIcon(":/icons/images/pause.png");
        ui->playPauseButton->setIcon(pauseIcon);
        isPlaying = false;
    } else {
        player->play();
        QIcon playIcon(":/icons/images/play.png");
        ui->playPauseButton->setIcon(playIcon);
        isPlaying = true;
    }
}

void ProjectWorkWindow::on_playPauseButton_clicked()
{
    togglePlayPause();
}

void ProjectWorkWindow::reset() {
    player->stop();
    player->setPosition(0);

    if (isRepeatOn) {
        player->play();
    }
    QIcon pauseIcon(":/icons/images/pause.png");
    ui->playPauseButton->setIcon(pauseIcon);
    isPlaying = false;
}

void ProjectWorkWindow::on_resetButton_clicked()
{
    reset();
}

void ProjectWorkWindow::updatePosition(qint64 position)
{
    if (!ui->timelineSlider->isSliderDown()) {
        ui->timelineSlider->setValue(position);
    }
    ui->currentTimeLabel->setText(getPositionLabel(double(position)/1000.0, projectDuration));
}

void ProjectWorkWindow::updateDuration(qint64 duration)
{
    ui->timelineSlider->setRange(0, duration);
    ui->durationLabel->setText(getPositionLabel(double(duration)/1000.0, projectDuration));
}

void ProjectWorkWindow::on_speedSlider_valueChanged(int value)
{
    double speed = 1.0;
    QString speedText;

    switch(value) {
    case 0: speed = 0.5; speedText = "0.5x"; break;
    case 1: speed = 1.0; speedText = "1x"; break;
    case 2: speed = 1.5; speedText = "1.5x"; break;
    case 3: speed = 2.0; speedText = "2x"; break;
    default: speed = 1.0; speedText = "1x"; break;
    }

    player->setPlaybackRate(speed);
    ui->speedLabel->setText(speedText);
}

void ProjectWorkWindow::on_volumeSlider_valueChanged(int value)
{
    float volume = value / 100.0f;
    audioOutput->setVolume(volume);
    ui->volumeLabel->setText(QString("%1%").arg(value));
    updateVolumeIcon(value);
}

void ProjectWorkWindow::rewind()
{
    qint64 newPos = player->position() - REWIND_STEP_MS;
    newPos = qMax(0LL, newPos);

    player->setPosition(newPos);
    ui->timelineSlider->setValue(newPos);
}

void ProjectWorkWindow::on_rewindButton_clicked()
{
    rewind();
}

void ProjectWorkWindow::fastForward()
{
    qint64 newPos = player->position() + REWIND_STEP_MS;
    newPos = qMin(player->duration(), newPos);

    player->setPosition(newPos);
    ui->timelineSlider->setValue(newPos);
}

void ProjectWorkWindow::on_fastForwardButton_clicked()
{
    fastForward();
}

void ProjectWorkWindow::repeat()
{
    isRepeatOn = !isRepeatOn;

    if (isRepeatOn) {
        ui->repeatButton->setStyleSheet("QPushButton { border: none; background-color: #0078d7; border-radius: 5px; }");
    } else {
        ui->repeatButton->setStyleSheet("QPushButton { border: none; }");
    }
}

void ProjectWorkWindow::on_repeatButton_clicked()
{
    repeat();
}

void ProjectWorkWindow::on_playbackStateChanged(QMediaPlayer::PlaybackState state)
{
    if (isRepeatOn && state == QMediaPlayer::StoppedState) {
        player->setPosition(0);
        player->play();
    }
}

void ProjectWorkWindow::loadAudioForVisualization(const QString &filePath)
{
    QVector<float> audioData;

    if (filePath.isEmpty()) {
        return;
    }

    SF_INFO sfInfoIn;
    memset(&sfInfoIn, 0, sizeof(sfInfoIn));
    SNDFILE* infile = sf_open(filePath.toUtf8().constData(), SFM_READ, &sfInfoIn);

    if (infile != nullptr) {
        const int BUFFER_SIZE = 200000;
        std::vector<float> buffer;
        buffer.resize(BUFFER_SIZE*sfInfoIn.channels);
        while(sf_count_t readItems = sf_readf_float(infile, &buffer.front(), BUFFER_SIZE)) {
            std::copy(buffer.begin(), buffer.begin()+readItems, std::back_inserter(audioData));
        }
        sf_close(infile);
    } else {
        showErrorMessage("Cannot load audio file content: " + QString(sf_strerror(NULL)));
        return;
    }

    if (!audioData.empty()) {
        waveformWidget->setDuration(projectDuration * 1000.0);
        waveformWidget->setAudioData(audioData);
    }
}

void ProjectWorkWindow::on_trimButton_clicked()
{
    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;

    if (startMs >= endMs) {
        QMessageBox::warning(this, "Error", "Invalid selection range");
        return;
    }

    if (QMessageBox::question(this, "Confirm",
                              "Trim audio to selected range?\n"
                              "(The selection will become the new audio from 0:00)",
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }
    clearFutureHistory();
    stopPlayerAndResetUI();

    qint64 newDurationMs = endMs - startMs;
    int newDurationSec = newDurationMs / 1000;

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        showErrorMessage("Cannot open audio file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    qint64 startSample = (startMs * sfinfo.samplerate) / 1000;
    qint64 endSample = (endMs * sfinfo.samplerate) / 1000;
    qint64 trimSamples = endSample - startSample;
    QString tempFile = createTempFilePath("temp_trim_audio");
    if (QFile::exists(tempFile)) {
        QFile::remove(tempFile);
    }

    SF_INFO outinfo = sfinfo;
    outinfo.frames = trimSamples;
    SNDFILE* outfile = sf_open(tempFile.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        sf_close(infile);
        showErrorMessage("Cannot create temporary file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    if (sf_seek(infile, startSample, SEEK_SET) == -1) {
        sf_close(infile);
        sf_close(outfile);
        showErrorMessage("Cannot seek to start position");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    const int BUFFER_SIZE = 200000;
    QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);
    sf_count_t totalFramesWritten = 0;
    sf_count_t framesToCopy = trimSamples;

    try {
        while (framesToCopy > 0) {
            sf_count_t framesThisTime = qMin(framesToCopy, (sf_count_t)BUFFER_SIZE);
            sf_count_t framesRead = sf_readf_float(infile, buffer.data(), framesThisTime);

            if (framesRead <= 0) break;

            sf_count_t framesWritten = sf_writef_float(outfile, buffer.data(), framesRead);
            if (framesWritten != framesRead) {
                throw std::runtime_error("Failed to write audio data");
            }

            totalFramesWritten += framesWritten;
            framesToCopy -= framesRead;
        }
    } catch (const std::exception& e) {
        sf_close(infile);
        sf_close(outfile);
        QFile::remove(tempFile);
        showErrorMessage(QString("Error during trim operation: %1").arg(e.what()));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_close(infile);
    sf_close(outfile);

    SF_INFO testinfo;
    memset(&testinfo, 0, sizeof(testinfo));
    SNDFILE* testfile = sf_open(tempFile.toUtf8().constData(), SFM_READ, &testinfo);
    if (!testfile || testinfo.frames != totalFramesWritten) {
        QFile::remove(tempFile);
        showErrorMessage("Failed to create valid output file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }
    sf_close(testfile);

    updateProjectWithNewFile(tempFile, totalFramesWritten, sfinfo.samplerate);


    showSuccessMessage(QString("Trim is done. Selection (%1 - %2) is now the new audio from 0:00")
                           .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
                           .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss")));
}

void ProjectWorkWindow::undo()
{
    if (currentHistoryIndex <= 0) {
        return;
    }

    player->stop();
    player->setSource(QUrl());

    currentHistoryIndex--;
    AudioState previousState = undoHistory[currentHistoryIndex];

    if (QFile::exists(previousState.filePath)) {
        player->setSource(QUrl::fromLocalFile(previousState.filePath));

        projectPath = previousState.filePath;
        projectDuration = (previousState.endMs - previousState.startMs) / 1000.0;

        trimStartMs = previousState.startMs;
        trimEndMs = previousState.endMs;
        isTrimmed = (previousState.startMs != 0);

        waveformWidget->setDuration(projectDuration * 1000);
        ui->timelineSlider->setRange(0, projectDuration * 1000);
        ui->durationLabel->setText(getPositionLabel(projectDuration, projectDuration));
        ui->currentTimeLabel->setText(getPositionLabel(0.0, projectDuration));

        loadAudioForVisualization(previousState.filePath);

        SF_INFO sfinfo;
        memset(&sfinfo, 0, sizeof(sfinfo));
        SNDFILE* testFile = sf_open(previousState.filePath.toUtf8().constData(), SFM_READ, &sfinfo);
        if (testFile) {
            projectSamplingRate = sfinfo.samplerate;
            projectFormat = getFormatFromSFInfo(sfinfo);
            sf_close(testFile);
        }

        QMessageBox::information(this, "Undo", "Operation undone successfully");
    }

    ui->undoButton->setEnabled(currentHistoryIndex > 0);
    ui->redoButton->setEnabled(currentHistoryIndex < undoHistory.size() - 1);
}

void ProjectWorkWindow::on_undoButton_clicked()
{
    undo();
}

void ProjectWorkWindow::redo()
{
    if (currentHistoryIndex >= undoHistory.size() - 1) {
        return;
    }

    player->stop();
    player->setSource(QUrl());

    currentHistoryIndex++;
    AudioState nextState = undoHistory[currentHistoryIndex];

    if (QFile::exists(nextState.filePath)) {
        player->setSource(QUrl::fromLocalFile(nextState.filePath));

        projectPath = nextState.filePath;
        projectDuration = (nextState.endMs - nextState.startMs) / 1000.0;

        trimStartMs = nextState.startMs;
        trimEndMs = nextState.endMs;
        isTrimmed = (nextState.startMs != 0);

        waveformWidget->setDuration(projectDuration * 1000);
        ui->timelineSlider->setRange(0, projectDuration * 1000);
        ui->durationLabel->setText(getPositionLabel(projectDuration, projectDuration));
        ui->currentTimeLabel->setText(getPositionLabel(0.0, projectDuration));

        loadAudioForVisualization(nextState.filePath);

        SF_INFO sfinfo;
        memset(&sfinfo, 0, sizeof(sfinfo));
        SNDFILE* testFile = sf_open(nextState.filePath.toUtf8().constData(), SFM_READ, &sfinfo);
        if (testFile) {
            projectSamplingRate = sfinfo.samplerate;
            projectFormat = getFormatFromSFInfo(sfinfo);
            sf_close(testFile);
        }

        QMessageBox::information(this, "Redo", "Operation redone successfully");
    }

    ui->undoButton->setEnabled(currentHistoryIndex > 0);
    ui->redoButton->setEnabled(currentHistoryIndex < undoHistory.size() - 1);
}

void ProjectWorkWindow::on_redoButton_clicked()
{
    redo();
}

QString ProjectWorkWindow::getFormatFromSFInfo(const SF_INFO& info)
{
    QString format;

    switch (info.format & SF_FORMAT_TYPEMASK) {
    case SF_FORMAT_WAV: format = "WAV"; break;
    case SF_FORMAT_FLAC: format = "FLAC"; break;
    case SF_FORMAT_MPEG: format = "MP3"; break;
    default: format = "Unknown"; break;
    }

    switch (info.format & SF_FORMAT_SUBMASK) {
    case SF_FORMAT_PCM_16: format += " 16-bit"; break;
    case SF_FORMAT_PCM_24: format += " 24-bit"; break;
    case SF_FORMAT_PCM_32: format += " 32-bit"; break;
    case SF_FORMAT_FLOAT: format += " Float"; break;
    case SF_FORMAT_DOUBLE: format += " Double"; break;
    default: format += " Unknown"; break;
    }

    return format;
}

void ProjectWorkWindow::copy()
{
    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;

    if (startMs >= endMs) {
        QMessageBox::warning(this, "Error", "Please select a valid audio range to copy");
        return;
    }

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        QMessageBox::critical(this, "Error", "Cannot open audio file: " + QString(sf_strerror(NULL)));
        return;
    }

    qint64 startSample = (startMs * sfinfo.samplerate) / 1000;
    qint64 endSample = (endMs * sfinfo.samplerate) / 1000;
    qint64 totalSamples = endSample - startSample;

    if (sf_seek(infile, startSample, SEEK_SET) == -1) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Cannot seek to start position");
        return;
    }

    copiedAudioData.resize(totalSamples * sfinfo.channels);
    sf_count_t samplesRead = sf_readf_float(infile, copiedAudioData.data(), totalSamples);
    sf_close(infile);

    if (samplesRead != totalSamples) {
        QMessageBox::critical(this, "Error", "Failed to read audio data for copying");
        copiedAudioData.clear();
        return;
    }

    copiedSampleRate = sfinfo.samplerate;
    copiedChannels = sfinfo.channels;
    copiedStartMs = startMs;
    copiedEndMs = endMs;
    hasCopiedData = true;

    lastActionWasCut = false;

    QMessageBox::information(this, "Copy",
                             QString("Audio from %1 to %2 copied to clipboard (%3 samples)")
                                 .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
                                 .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss"))
                                 .arg(totalSamples));
}

void ProjectWorkWindow::on_copyButton_clicked()
{
    copy();
}

void ProjectWorkWindow::cut()
{
    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;

    if (startMs >= endMs) {
        QMessageBox::warning(this, "Error", "Please select a valid audio range to cut");
        return;
    }

    if (QMessageBox::question(this, "Confirm Cut",
                              QString("Cut audio from %1 to %2?\nThis will remove the selected portion from the audio.")
                                  .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
                                  .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss")),
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    clearFutureHistory();
    stopPlayerAndResetUI();

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        QMessageBox::critical(this, "Error", "Cannot open audio file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    qint64 startSample = (startMs * sfinfo.samplerate) / 1000;
    qint64 endSample = (endMs * sfinfo.samplerate) / 1000;
    qint64 cutSamples = endSample - startSample;

    sf_count_t totalFrames = sf_seek(infile, 0, SF_SEEK_END);
    sf_seek(infile, 0, SF_SEEK_SET);

    // remember data to the buffer
    // {
    cutAudioData.resize(cutSamples * sfinfo.channels);
    sf_seek(infile, startSample, SEEK_SET);
    sf_count_t samplesRead = sf_readf_float(infile, cutAudioData.data(), cutSamples);

    if (samplesRead != cutSamples) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Failed to read audio data for cutting");
        cutAudioData.clear();
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    cutSampleRate = sfinfo.samplerate;
    cutChannels = sfinfo.channels;
    cutStartMs = startMs;
    cutEndMs = endMs;
    hasCutData = true;
    // }

    lastActionWasCut = true;

    QString tempFile = createTempFilePath("temp_cut");

    if (QFile::exists(tempFile)) {
        QFile::remove(tempFile);
    }

    SF_INFO outinfo = sfinfo;
    outinfo.frames = sfinfo.frames - cutSamples;
    SNDFILE* outfile = sf_open(tempFile.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Cannot create temporary file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    const int BUFFER_SIZE = 200000;
    QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);

    try {
        // move to the begin of a file
        sf_seek(infile, 0, SEEK_SET);
        sf_count_t framesToCopy = startSample;
        while (framesToCopy > 0) {
            sf_count_t framesThisTime = qMin(framesToCopy, (sf_count_t)BUFFER_SIZE);
            sf_count_t framesRead = sf_readf_float(infile, buffer.data(), framesThisTime);

            if (framesRead <= 0) break;

            sf_count_t framesWritten = sf_writef_float(outfile, buffer.data(), framesRead);
            if (framesWritten != framesRead) {
                throw std::runtime_error("Failed to write audio data");
            }
            framesToCopy -= framesRead;
        }
        sf_seek(infile, endSample, SEEK_SET);

        sf_count_t framesRead = (sf_count_t)BUFFER_SIZE;
        while (framesRead == (sf_count_t)BUFFER_SIZE) {
            framesRead = sf_readf_float(infile, buffer.data(), (sf_count_t)BUFFER_SIZE);

            const sf_count_t framesWritten = sf_writef_float(outfile, buffer.data(), framesRead);
            if (framesWritten != framesRead) {
                throw std::runtime_error("Failed to write audio data");
            }
        }

    } catch (const std::exception& e) {
        sf_close(infile);
        sf_close(outfile);
        QFile::remove(tempFile);
        QMessageBox::critical(this, "Error", QString("Error during cut operation: %1").arg(e.what()));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_close(infile);
    sf_close(outfile);

    SF_INFO testinfo;
    memset(&testinfo, 0, sizeof(testinfo));
    SNDFILE* testfile = sf_open(tempFile.toUtf8().constData(), SFM_READ, &testinfo);
    if (!testfile) {
        QFile::remove(tempFile);
        QMessageBox::critical(this, "Error", "Failed to create valid output file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }
    sf_close(testfile);

    updateProjectWithNewFile(tempFile, testinfo.frames, sfinfo.samplerate);

    QMessageBox::information(this, "Success",
                             QString("Audio from %1 to %2 cut successfully\nCut audio is available for pasting")
                                 .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
                                 .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss")));
}

void ProjectWorkWindow::on_cutButton_clicked()
{
    cut();
}

void ProjectWorkWindow::paste()
{
    bool useCutData = (hasCutData && lastActionWasCut);
    bool useCopiedData = (hasCopiedData && !lastActionWasCut);

    if (!useCutData && !useCopiedData) {
        if (hasCutData) useCutData = true;
        else if (hasCopiedData) useCopiedData = true;
        else {
            QMessageBox::warning(this, "Error", "No audio data available to paste");
            return;
        }
    }

    QVector<float>* dataToPaste = useCutData ? &cutAudioData : &copiedAudioData;
    int sampleRate = useCutData ? cutSampleRate : copiedSampleRate;
    int channels = useCutData ? cutChannels : copiedChannels;
    qint64 startMs = useCutData ? cutStartMs : copiedStartMs;
    qint64 endMs = useCutData ? cutEndMs : copiedEndMs;

    qint64 pastePositionMs = waveformWidget->getCursorPosition();
    auto selection = waveformWidget->getSelection();
    if (selection.first != selection.second) {
        pastePositionMs = selection.first;
    }

    if (QMessageBox::question(this, "Confirm Paste",
                              QString("Paste audio (%1 - %2) at position %3?")
                                  .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
                                  .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss"))
                                  .arg(QTime::fromMSecsSinceStartOfDay(pastePositionMs).toString("mm:ss")),
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    clearFutureHistory();
    stopPlayerAndResetUI();

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        QMessageBox::critical(this, "Error", "Cannot open audio file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    if (channels != sfinfo.channels) {
        sf_close(infile);
        QMessageBox::critical(this, "Error",
                              QString("Channel mismatch: Original has %1 channels, pasted has %2 channels")
                                  .arg(sfinfo.channels).arg(channels));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    qint64 pastePositionFrames = (pastePositionMs * sfinfo.samplerate) / 1000;
    qint64 pasteFrames = dataToPaste->size() / sfinfo.channels;

    QVector<float> originalAudio(sfinfo.frames * sfinfo.channels);
    sf_seek(infile, 0, SEEK_SET);
    sf_count_t framesRead = sf_readf_float(infile, originalAudio.data(), sfinfo.frames);
    sf_close(infile);

    if (framesRead != sfinfo.frames) {
        QMessageBox::critical(this, "Error", "Failed to read original audio data");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    QVector<float> newAudio;
    newAudio.reserve(originalAudio.size() + dataToPaste->size());

    qint64 samplesBeforePaste = pastePositionFrames * sfinfo.channels;
    samplesBeforePaste = qMin(samplesBeforePaste, (qint64)originalAudio.size());

    for (int i = 0; i < samplesBeforePaste; i++) {
        newAudio.append(originalAudio[i]);
    }

    for (int i = 0; i < dataToPaste->size(); i++) {
        newAudio.append((*dataToPaste)[i]);
    }

    for (int i = samplesBeforePaste; i < originalAudio.size(); i++) {
        newAudio.append(originalAudio[i]);
    }

    QString tempFile = createTempFilePath("temp_paste_audio");

    if (QFile::exists(tempFile)) {
        QFile::remove(tempFile);
    }

    SF_INFO outinfo;
    memset(&outinfo, 0, sizeof(outinfo));
    outinfo.samplerate = sfinfo.samplerate;
    outinfo.channels = sfinfo.channels;
    outinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    outinfo.frames = newAudio.size() / sfinfo.channels;

    SNDFILE* outfile = sf_open(tempFile.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        QMessageBox::critical(this, "Error", "Cannot create temporary file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t totalFrames = newAudio.size() / sfinfo.channels;
    sf_count_t framesWritten = sf_writef_float(outfile, newAudio.data(), totalFrames);
    sf_close(outfile);

    if (framesWritten != totalFrames) {
        QFile::remove(tempFile);
        QMessageBox::critical(this, "Error", "Failed to write audio data completely");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    updateProjectWithNewFile(tempFile, totalFrames, sfinfo.samplerate);

    QMessageBox::information(this, "Success",
                             QString("Audio pasted successfully at position %1")
                                 .arg(QTime::fromMSecsSinceStartOfDay(pastePositionMs).toString("mm:ss")));
}

void ProjectWorkWindow::on_pasteButton_clicked()
{
    paste();
}

void ProjectWorkWindow::deleteAction()
{
    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;

    if (startMs >= endMs) {
        QMessageBox::warning(this, "Error", "Please select a valid audio range to delete");
        return;
    }

    if (QMessageBox::question(this, "Confirm Delete",
                              QString("Delete audio from %1 to %2?\nThis will permanently remove the selected portion.")
                                  .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
                                  .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss")),
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    clearFutureHistory();
    stopPlayerAndResetUI();

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        QMessageBox::critical(this, "Error", "Cannot open audio file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    qint64 startSample = (startMs * sfinfo.samplerate) / 1000;
    qint64 endSample = (endMs * sfinfo.samplerate) / 1000;
    qint64 deleteSamples = endSample - startSample;

    QString tempFile = createTempFilePath("temp_delete_audio");

    if (QFile::exists(tempFile)) {
        QFile::remove(tempFile);
    }

    SF_INFO outinfo = sfinfo;
    outinfo.frames = sfinfo.frames - deleteSamples;
    SNDFILE* outfile = sf_open(tempFile.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Cannot create temporary file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    const int BUFFER_SIZE = 200000;
    QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);
    sf_count_t totalFramesWritten = 0;

    try {
        if (startSample > 0) {
            sf_seek(infile, 0, SEEK_SET);
            sf_count_t framesToCopy = startSample;

            while (framesToCopy > 0) {
                sf_count_t framesThisTime = qMin(framesToCopy, (sf_count_t)BUFFER_SIZE);
                sf_count_t framesRead = sf_readf_float(infile, buffer.data(), framesThisTime);

                if (framesRead <= 0) break;

                sf_count_t framesWritten = sf_writef_float(outfile, buffer.data(), framesRead);
                if (framesWritten != framesRead) {
                    throw std::runtime_error("Failed to write audio data");
                }

                totalFramesWritten += framesWritten;
                framesToCopy -= framesRead;
            }
        }

        sf_seek(infile, endSample, SEEK_SET);

        sf_count_t framesAfterDelete = sfinfo.frames - endSample;
        while (framesAfterDelete > 0) {
            sf_count_t framesThisTime = qMin(framesAfterDelete, (sf_count_t)BUFFER_SIZE);
            sf_count_t framesRead = sf_readf_float(infile, buffer.data(), framesThisTime);

            if (framesRead <= 0) break;

            sf_count_t framesWritten = sf_writef_float(outfile, buffer.data(), framesRead);
            if (framesWritten != framesRead) {
                throw std::runtime_error("Failed to write audio data");
            }

            totalFramesWritten += framesWritten;
            framesAfterDelete -= framesRead;
        }

    } catch (const std::exception& e) {
        sf_close(infile);
        sf_close(outfile);
        QFile::remove(tempFile);
        QMessageBox::critical(this, "Error", QString("Error during delete operation: %1").arg(e.what()));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_close(infile);
    sf_close(outfile);

    SF_INFO testinfo;
    memset(&testinfo, 0, sizeof(testinfo));
    SNDFILE* testfile = sf_open(tempFile.toUtf8().constData(), SFM_READ, &testinfo);
    if (!testfile || testinfo.frames != totalFramesWritten) {
        QFile::remove(tempFile);
        QMessageBox::critical(this, "Error", "Failed to create valid output file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }
    sf_close(testfile);

    updateProjectWithNewFile(tempFile, totalFramesWritten, sfinfo.samplerate);
    QMessageBox::information(this, "Success",
                             QString("Audio from %1 to %2 deleted successfully")
                                 .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
                                 .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss")));
}

void ProjectWorkWindow::on_deleteButton_clicked()
{
    deleteAction();
}

void ProjectWorkWindow::save()
{
    if (projectPath == originalFilePath && currentHistoryIndex == 0) {
        QMessageBox::information(this, "Info", "No changes to save - file is already in original state");
        return;
    }

    if (QMessageBox::question(this, "Confirm Save",
                              "Save all changes to original file?\nThis will overwrite the original file.",
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    player->stop();
    player->setSource(QUrl());

    SF_INFO sfInFileInfo;
    memset(&sfInFileInfo, 0, sizeof(sfInFileInfo));
    SF_INFO sfOutFileInfo;
    memset(&sfOutFileInfo, 0, sizeof(sfOutFileInfo));

    SNDFILE* inSndfile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfInFileInfo);
    SNDFILE* outSndfile = nullptr;

    if (inSndfile != nullptr) {
        sfOutFileInfo.frames = sfInFileInfo.frames;
        sfOutFileInfo.samplerate = projectSamplingRate;
        sfOutFileInfo.channels = projectChannelsCount;
        sfOutFileInfo.format = SF_FORMAT_MPEG | SF_FORMAT_MPEG_LAYER_III;
        sfOutFileInfo.sections = sfInFileInfo.sections;
        sfOutFileInfo.seekable = sfInFileInfo.seekable;

        if (sf_format_check(&sfOutFileInfo)) {
            outSndfile = sf_open(originalFilePath.toUtf8().constData(), SFM_WRITE, &sfOutFileInfo);
        }
    }

    if ((inSndfile != nullptr) && (outSndfile != nullptr)) {
        const int BUFFER_SIZE = 200000;
        QVector<float> buffer(BUFFER_SIZE * sfInFileInfo.channels);
        sf_count_t totalFramesWritten = 0;

        try {
            sf_seek(inSndfile, 0, SEEK_SET);

            while (true) {
                sf_count_t framesRead = sf_readf_float(inSndfile, buffer.data(), BUFFER_SIZE);

                if (framesRead <= 0) break;

                sf_count_t framesWritten = sf_writef_float(outSndfile, buffer.data(), framesRead);
                if (framesWritten != framesRead) {
                    throw std::runtime_error("Failed to write audio data");
                }

                totalFramesWritten += framesWritten;
            }
        } catch (const std::exception& e) {
            sf_close(inSndfile);
            sf_close(outSndfile);
            QMessageBox::critical(this, "Error", QString("Error during save operation: %1").arg(e.what()));
            return;
        }

        sf_close(inSndfile);
        sf_close(outSndfile);
    }

    reloadAudioVisualization();
    loadAndSetAudioFile(projectPath);
    player->setPosition(0);

    QMessageBox::information(this, "Success",
                             "All changes saved to original file successfully.\n"
                             "History has been preserved - you can continue using Undo/Redo.");
}

void ProjectWorkWindow::on_recordButton_clicked()
{
    if (isRecording) {
        mediaRecorder->stop();
        QIcon recordIcon(":/icons/images/record.png");
        ui->recordButton->setIcon(recordIcon);
        isRecording = false;
        QMessageBox::information(this, "Recording", "Recording stopped. Processing audio...");
        mixRecordedAudioWithOriginal();
    } else {
        auto selection = waveformWidget->getSelection();
        recordStartPosition = selection.first;
        recordEndPosition = selection.second;
        if (recordStartPosition >= recordEndPosition) {
            QMessageBox::warning(this, "Error", "Please select a valid range for recording overlay");
            return;
        }
        if (QMessageBox::question(this, "Confirm Recording",
                                  QString("Record audio overlay from %1 to %2?\nSpeak into the microphone when ready.")
                                      .arg(QTime::fromMSecsSinceStartOfDay(recordStartPosition).toString("mm:ss"))
                                      .arg(QTime::fromMSecsSinceStartOfDay(recordEndPosition).toString("mm:ss")),
                                  QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
            return;
        }

        recordedFilePath = createTempFilePath("temp_record");

        QAudioDevice inputDevice = QMediaDevices::defaultAudioInput();
        if (inputDevice.isNull()) {
            QMessageBox::critical(this, "Error", "No audio input device found!");
            return;
        }
        audioInput->setDevice(inputDevice);
        mediaRecorder->setOutputLocation(QUrl::fromLocalFile(recordedFilePath));

        QMediaFormat format;
        format.setFileFormat(QMediaFormat::Wave);
        format.setAudioCodec(QMediaFormat::AudioCodec::Wave);
        mediaRecorder->setMediaFormat(format);

        mediaRecorder->setQuality(QMediaRecorder::HighQuality);
        mediaRecorder->setAudioSampleRate(44100);
        mediaRecorder->setAudioChannelCount(1);

        mediaRecorder->record();

        QIcon stopIcon(":/icons/images/stop_recording.png");
        ui->recordButton->setIcon(stopIcon);
        isRecording = true;

        QMessageBox::information(this, "Recording", "Recording started. Speak into the microphone...");
    }
}

void ProjectWorkWindow::onRecordingStateChanged(QMediaRecorder::RecorderState state)
{
    if (state == QMediaRecorder::StoppedState && isRecording) {
        isRecording = false;
        QIcon recordIcon(":/icons/images/record.png");
        ui->recordButton->setIcon(recordIcon);
    }
}

void ProjectWorkWindow::onRecordingError(QMediaRecorder::Error error, const QString &errorString)
{
    Q_UNUSED(error);
    QMessageBox::critical(this, "Recording Error", "Recording error: " + errorString);
    isRecording = false;
    QIcon recordIcon(":/icons/images/record.png");
    ui->recordButton->setIcon(recordIcon);
    if (QFile::exists(recordedFilePath)) {
        QFile::remove(recordedFilePath);
    }
}

QVector<float> ProjectWorkWindow::resampleAudio(const QVector<float>& audioData, int originalSampleRate, int targetSampleRate, int channels)
{
    if (originalSampleRate == targetSampleRate) {
        return audioData;
    }

    double ratio = (double)targetSampleRate / (double)originalSampleRate;
    int newSize = (int)(audioData.size() * ratio / channels) * channels;
    QVector<float> resampled(newSize);

    for (int i = 0; i < newSize / channels; i++) {
        double oldIndex = i / ratio;
        int index1 = (int)oldIndex;
        int index2 = qMin(index1 + 1, audioData.size() / channels - 1);
        double fraction = oldIndex - index1;

        for (int ch = 0; ch < channels; ch++) {
            float sample1 = audioData[index1 * channels + ch];
            float sample2 = audioData[index2 * channels + ch];
            resampled[i * channels + ch] = sample1 + fraction * (sample2 - sample1);
        }
    }

    return resampled;
}

void ProjectWorkWindow::mixRecordedAudioWithOriginal()
{
    if (!QFile::exists(recordedFilePath)) {
        QMessageBox::critical(this, "Error", "Recorded audio file not found!");
        return;
    }

    clearFutureHistory();
    stopPlayerAndResetUI();

    SF_INFO originalInfo;
    memset(&originalInfo, 0, sizeof(originalInfo));
    SNDFILE* originalFile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &originalInfo);

    if (!originalFile) {
        QMessageBox::critical(this, "Error", "Cannot open original audio file: " + QString(sf_strerror(NULL)));
        return;
    }

    SF_INFO recordedInfo;
    memset(&recordedInfo, 0, sizeof(recordedInfo));
    SNDFILE* recordedFile = sf_open(recordedFilePath.toUtf8().constData(), SFM_READ, &recordedInfo);

    if (!recordedFile) {
        sf_close(originalFile);
        QMessageBox::critical(this, "Error", "Cannot open recorded audio file: " + QString(sf_strerror(NULL)));
        return;
    }

    QVector<float> originalData(originalInfo.frames * originalInfo.channels);
    sf_seek(originalFile, 0, SEEK_SET);
    sf_readf_float(originalFile, originalData.data(), originalInfo.frames);
    sf_close(originalFile);

    QVector<float> recordedData(recordedInfo.frames * recordedInfo.channels);
    sf_seek(recordedFile, 0, SEEK_SET);
    sf_readf_float(recordedFile, recordedData.data(), recordedInfo.frames);
    sf_close(recordedFile);

    if (recordedInfo.samplerate != originalInfo.samplerate) {
        recordedData = resampleAudio(recordedData, recordedInfo.samplerate, originalInfo.samplerate, recordedInfo.channels);
        recordedInfo.samplerate = originalInfo.samplerate;
        recordedInfo.frames = recordedData.size() / recordedInfo.channels;
    }

    qint64 startSample = (recordStartPosition * originalInfo.samplerate) / 1000;
    qint64 endSample = (recordEndPosition * originalInfo.samplerate) / 1000;
    qint64 overlaySamples = endSample - startSample;

    sf_count_t recordedSamples = recordedInfo.frames;
    if (recordedSamples > overlaySamples) {
        recordedSamples = overlaySamples;
        recordedData.resize(recordedSamples * recordedInfo.channels);
    }

    const float gain = 2.0f;
    for (int i = 0; i < recordedData.size(); i++) {
        recordedData[i] = qBound(-1.0f, recordedData[i] * gain, 1.0f);
    }

    for (sf_count_t i = 0; i < recordedSamples; i++) {
        if (startSample + i < originalInfo.frames) {
            float fadeIn = i < 1000 ? (float)i / 1000.0f : 1.0f;
            float fadeOut = i > recordedSamples - 1000 ? (float)(recordedSamples - i) / 1000.0f : 1.0f;
            float fade = qMin(fadeIn, fadeOut);

            for (int ch = 0; ch < qMin(originalInfo.channels, recordedInfo.channels); ch++) {
                qint64 origIndex = (startSample + i) * originalInfo.channels + ch;
                qint64 recIndex = i * recordedInfo.channels + ch;
                float mixed = originalData[origIndex] + (recordedData[recIndex] * fade);
                originalData[origIndex] = qBound(-1.0f, mixed, 1.0f);
            }
        }
    }

    QString mixedFilePath = createTempFilePath("temp_mixed_audio");

    SF_INFO outInfo = originalInfo;
    SNDFILE* outFile = sf_open(mixedFilePath.toUtf8().constData(), SFM_WRITE, &outInfo);

    if (!outFile) {
        QMessageBox::critical(this, "Error", "Cannot create mixed audio file: " + QString(sf_strerror(NULL)));
        return;
    }

    sf_writef_float(outFile, originalData.data(), originalInfo.frames);
    sf_close(outFile);

    updateProjectWithNewFile(mixedFilePath, originalInfo.frames, originalInfo.samplerate);

    if (QFile::exists(recordedFilePath)) {
        QFile::remove(recordedFilePath);
    }

    QMessageBox::information(this, "Success",
                             QString("Recorded audio successfully mixed from %1 to %2")
                                 .arg(QTime::fromMSecsSinceStartOfDay(recordStartPosition).toString("mm:ss"))
                                 .arg(QTime::fromMSecsSinceStartOfDay(recordEndPosition).toString("mm:ss")));
}

void ProjectWorkWindow::on_helpButton_clicked()
{
    hide();
    HelpWindow *helpWindow = new HelpWindow(this, this);
    helpWindow->setAttribute(Qt::WA_DeleteOnClose);
    helpWindow->show();
    helpWindow->raise();
    helpWindow->activateWindow();
}

void ProjectWorkWindow::reverse()
{
    if (QMessageBox::question(this, "Confirm Reverse",
                              "Reverse the entire audio track?\nThis will play the audio backwards.",
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    clearFutureHistory();
    stopPlayerAndResetUI();

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        QMessageBox::critical(this, "Error", "Cannot open audio file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t totalFrames = sf_seek(infile, 0, SEEK_END);
    sf_seek(infile, 0, SEEK_SET);

    if (totalFrames <= 0) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Invalid audio file or empty file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    QVector<float> audioData(totalFrames * sfinfo.channels);

    sf_count_t framesRead = 0;
    sf_count_t totalFramesRead = 0;
    const sf_count_t BUFFER_SIZE = 200000;
    QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);

    while ((framesRead = sf_readf_float(infile, buffer.data(), BUFFER_SIZE)) > 0) {
        memcpy(&audioData[totalFramesRead * sfinfo.channels], &buffer[0], framesRead * sfinfo.channels * sizeof(float));
        totalFramesRead += framesRead;
    }

    sf_close(infile);

    if (totalFramesRead != totalFrames) {
        QMessageBox::warning(this, "Warning",
                             QString("Read %1 frames out of %2 total frames")
                                 .arg(totalFramesRead).arg(totalFrames));
        totalFrames = totalFramesRead;
    }

    reverseAudioData(audioData, sfinfo.channels, totalFrames);

    QString tempFile = createTempFilePath("temp_reverse");

    if (QFile::exists(tempFile)) {
        QFile::remove(tempFile);
    }

    SF_INFO outinfo = sfinfo;
    outinfo.frames = totalFrames;
    SNDFILE* outfile = sf_open(tempFile.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        QMessageBox::critical(this, "Error", "Cannot create temporary file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t framesWritten = sf_writef_float(outfile, audioData.data(), totalFrames);
    sf_close(outfile);

    if (framesWritten != totalFrames) {
        QFile::remove(tempFile);
        QMessageBox::critical(this, "Error",
                              QString("Failed to write reversed audio data completely. Wrote %1 out of %2 frames")
                                  .arg(framesWritten).arg(totalFrames));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    updateProjectWithNewFile(tempFile, totalFrames, sfinfo.samplerate);

    QMessageBox::information(this, "Success", "Audio reversed successfully");
}

void ProjectWorkWindow::reverseAudioData(QVector<float>& audioData, int channels, sf_count_t totalFrames)
{
    for (sf_count_t i = 0; i < totalFrames / 2; i++) {
        sf_count_t reverseIndex = totalFrames - 1 - i;

        for (int channel = 0; channel < channels; channel++) {
            sf_count_t index1 = i * channels + channel;
            sf_count_t index2 = reverseIndex * channels + channel;

            float temp = audioData[index1];
            audioData[index1] = audioData[index2];
            audioData[index2] = temp;
        }
    }
}

void ProjectWorkWindow::applyEchoEffect()
{
    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;

    bool applyToSelection = (startMs < endMs);

    EchoDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    double delayMs = dialog.getDelay();
    double decay = dialog.getDecay();
    int repetitions = dialog.getRepetitions();

    QString message;
    if (applyToSelection) {
        message = QString("Apply echo effect to selection (%1 - %2)?\nDelay: %3ms, Decay: %4, Repetitions: %5")
        .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
            .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss"))
            .arg(delayMs).arg(decay).arg(repetitions);
    } else {
        message = QString("Apply echo effect to entire track?\nDelay: %1ms, Decay: %2, Repetitions: %3")
        .arg(delayMs).arg(decay).arg(repetitions);
    }

    if (QMessageBox::question(this, "Confirm Echo Effect", message,
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    clearFutureHistory();
    stopPlayerAndResetUI();

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        QMessageBox::critical(this, "Error", "Cannot open audio file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t totalFrames = sf_seek(infile, 0, SEEK_END);
    sf_seek(infile, 0, SEEK_SET);

    if (totalFrames <= 0) {
        totalFrames = (projectDuration * sfinfo.samplerate);
    }

    qint64 startSample = applyToSelection ? (startMs * sfinfo.samplerate) / 1000 : 0;
    qint64 endSample = applyToSelection ? (endMs * sfinfo.samplerate) / 1000 : totalFrames;
    qint64 processSamples = endSample - startSample;

    qDebug() << "Audio info: samplerate:" << sfinfo.samplerate << "channels:" << sfinfo.channels
             << "totalFrames:" << totalFrames << "processSamples:" << processSamples;

    QVector<float> audioData(totalFrames * sfinfo.channels);
    const int BUFFER_SIZE = 200000;
    QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);
    sf_count_t totalFramesRead = 0;

    try {
        while (true) {
            sf_count_t framesRead = sf_readf_float(infile, buffer.data(), BUFFER_SIZE);

            if (framesRead <= 0) break;

            memcpy(&audioData[totalFramesRead * sfinfo.channels], &buffer[0], framesRead * sfinfo.channels * sizeof(float));

            totalFramesRead += framesRead;

            if (totalFramesRead > totalFrames * 2) {
                qWarning() << "Breaking infinite loop, read too many frames:" << totalFramesRead;
                break;
            }
        }
    } catch (const std::exception& e) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", QString("Error reading audio data: %1").arg(e.what()));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_close(infile);

    qDebug() << "Frames read:" << totalFramesRead << "Expected:" << totalFrames;

    if (totalFramesRead == 0) {
        QMessageBox::critical(this, "Error", "No audio data read from file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    totalFrames = totalFramesRead;

    if (endSample > totalFrames) {
        endSample = totalFrames;
        processSamples = endSample - startSample;
    }

    if (startSample >= totalFrames || processSamples <= 0) {
        QMessageBox::critical(this, "Error",
                              QString("Invalid selection range: startSample=%1, totalFrames=%2")
                                  .arg(startSample).arg(totalFrames));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    CEchoEffect().apply(audioData, sfinfo.samplerate, sfinfo.channels, startSample, processSamples, delayMs, decay, repetitions);

    QString tempFile = createTempFilePath("temp_echo_audio");

    SF_INFO outinfo;
    memset(&outinfo, 0, sizeof(outinfo));
    outinfo.samplerate = sfinfo.samplerate;
    outinfo.channels = sfinfo.channels;
    outinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    outinfo.frames = totalFrames;

    SNDFILE* outfile = sf_open(tempFile.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        QMessageBox::critical(this, "Error", "Cannot create temporary file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    QVector<short> pcmData(audioData.size());
    for (int i = 0; i < audioData.size(); i++) {
        pcmData[i] = static_cast<short>(audioData[i] * 32767.0f);
    }

    sf_count_t totalFramesWritten = 0;
    const int WRITE_BUFFER_SIZE = 200000;

    try {
        for (sf_count_t i = 0; i < totalFrames; i += WRITE_BUFFER_SIZE) {
            sf_count_t framesThisTime = qMin(WRITE_BUFFER_SIZE, totalFrames - i);
            sf_count_t framesWritten = sf_writef_short(outfile, pcmData.data() + (i * sfinfo.channels), framesThisTime);

            if (framesWritten != framesThisTime) {
                throw std::runtime_error("Failed to write audio data block");
            }

            totalFramesWritten += framesWritten;
        }
    } catch (const std::exception& e) {
        sf_close(outfile);
        QFile::remove(tempFile);
        QMessageBox::critical(this, "Error", QString("Error writing audio data: %1").arg(e.what()));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_close(outfile);

    qDebug() << "Frames written:" << totalFramesWritten << "Expected:" << totalFrames;

    if (totalFramesWritten != totalFrames) {
        QFile::remove(tempFile);
        QMessageBox::critical(this, "Error",
                              QString("Failed to write processed audio data completely. Wrote %1 out of %2 frames")
                                  .arg(totalFramesWritten).arg(totalFrames));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    QFileInfo tempFileInfo(tempFile);
    if (!tempFileInfo.exists() || tempFileInfo.size() == 0) {
        QMessageBox::critical(this, "Error", "Created temporary file is empty or invalid");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    updateProjectWithNewFile(tempFile, totalFrames, sfinfo.samplerate);

    QString successMessage = QString("Echo effect applied successfully\nDelay: %1ms, Decay: %2, Repetitions: %3")
                                 .arg(delayMs).arg(decay).arg(repetitions);
    QMessageBox::information(this, "Success", successMessage);
}

void ProjectWorkWindow::applyEqualizerEffect()
{
    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;
    bool applyToSelection = (startMs < endMs);

    EqualizerDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    double lowGain = dialog.getLowGain();
    double midGain = dialog.getMidGain();
    double highGain = dialog.getHighGain();

    if (lowGain == 0 && midGain == 0 && highGain == 0) {
        QMessageBox::information(this, "Info", "No equalizer changes applied (all gains are 0 dB)");
        return;
    }

    QString message;
    if (applyToSelection) {
        message = QString("Apply equalizer to selection (%1 - %2)?\nBass: %3 dB, Mid: %4 dB, Treble: %5 dB")
        .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
            .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss"))
            .arg(lowGain).arg(midGain).arg(highGain);
    } else {
        message = QString("Apply equalizer to entire track?\nBass: %1 dB, Mid: %2 dB, Treble: %3 dB")
        .arg(lowGain).arg(midGain).arg(highGain);
    }

    if (QMessageBox::question(this, "Confirm Equalizer", message,
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    clearFutureHistory();
    stopPlayerAndResetUI();

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        QMessageBox::critical(this, "Error", "Cannot open audio file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t totalFrames = sf_seek(infile, 0, SEEK_END);
    sf_seek(infile, 0, SEEK_SET);

    if (totalFrames <= 0) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Invalid audio file or empty file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    qDebug() << "Total frames in file:" << totalFrames;
    qDebug() << "Sample rate:" << sfinfo.samplerate;
    qDebug() << "Channels:" << sfinfo.channels;

    qint64 startSample = applyToSelection ? (startMs * sfinfo.samplerate) / 1000 : 0;
    qint64 endSample = applyToSelection ? (endMs * sfinfo.samplerate) / 1000 : totalFrames;
    qint64 processSamples = endSample - startSample;

    if (startSample >= totalFrames) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Start position is beyond audio length");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    if (endSample > totalFrames) {
        endSample = totalFrames;
        processSamples = endSample - startSample;
    }

    qDebug() << "Applying equalizer: low=" << lowGain << "dB, mid=" << midGain << "dB, high=" << highGain << "dB";
    qDebug() << "Processing" << processSamples << "samples from" << startSample << "to" << endSample;

    QVector<float> audioData(totalFrames * sfinfo.channels);

    const sf_count_t BUFFER_SIZE = 200000;
    QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);
    sf_count_t totalFramesRead = 0;
    sf_count_t framesRead;

    try {
        while ((framesRead = sf_readf_float(infile, buffer.data(), BUFFER_SIZE)) > 0) {

            memcpy(&audioData[totalFramesRead * sfinfo.channels], &buffer[0], framesRead * sfinfo.channels * sizeof(float));
            totalFramesRead += framesRead;

            if (totalFramesRead > totalFrames * 2) {
                qWarning() << "Breaking infinite loop, read too many frames:" << totalFramesRead;
                break;
            }
        }
    } catch (const std::exception& e) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", QString("Error reading audio data: %1").arg(e.what()));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_close(infile);

    qDebug() << "Frames read:" << totalFramesRead << "Expected:" << totalFrames;

    if (totalFramesRead == 0) {
        QMessageBox::critical(this, "Error", "No audio data read from file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    if (totalFramesRead < totalFrames) {
        audioData.resize(totalFramesRead * sfinfo.channels);
        totalFrames = totalFramesRead;

        if (endSample > totalFrames) {
            endSample = totalFrames;
            processSamples = endSample - startSample;
        }
    }

    if (startSample >= totalFrames || processSamples <= 0) {
        QMessageBox::critical(this, "Error",
                              QString("Invalid selection range: startSample=%1, totalFrames=%2")
                                  .arg(startSample).arg(totalFrames));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    CEqualizerEffect().apply(audioData, sfinfo.samplerate, sfinfo.channels,
                             startSample, processSamples, lowGain, midGain, highGain);

    QString tempFile = createTempFilePath("temp_equalizer_audio");

    SF_INFO outinfo = sfinfo;
    outinfo.frames = totalFrames;
    SNDFILE* outfile = sf_open(tempFile.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        QMessageBox::critical(this, "Error", "Cannot create temporary file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t framesWritten = sf_writef_float(outfile, audioData.data(), totalFrames);
    sf_close(outfile);

    qDebug() << "Frames written:" << framesWritten << "Expected:" << totalFrames;

    if (framesWritten != totalFrames) {
        QFile::remove(tempFile);
        QMessageBox::critical(this, "Error",
                              QString("Failed to write processed audio data completely. Wrote %1 out of %2 frames")
                                  .arg(framesWritten).arg(totalFrames));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    QFileInfo tempFileInfo(tempFile);
    if (!tempFileInfo.exists() || tempFileInfo.size() == 0) {
        QMessageBox::critical(this, "Error", "Created temporary file is empty or invalid");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    updateProjectWithNewFile(tempFile, totalFrames, sfinfo.samplerate);

    QString successMessage = QString("Equalizer applied successfully\nBass: %1 dB, Mid: %2 dB, Treble: %3 dB")
                                 .arg(lowGain).arg(midGain).arg(highGain);
    QMessageBox::information(this, "Success", successMessage);
}

void ProjectWorkWindow::applyNoiseReduction()
{
    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;

    bool applyToSelection = (startMs < endMs);

    NoiseReductionDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    double noiseThreshold = dialog.getNoiseThreshold();
    int fftSize = dialog.getFftSize();
    double reductionStrength = dialog.getReductionStrength();
    double smoothing = dialog.getSmoothing();
    QString method = dialog.getMethod();
    bool learnNoiseProfile = dialog.getLearnNoiseProfile();

    QString message;
    if (applyToSelection) {
        message = QString("Apply %1 noise reduction to selection (%2 - %3)?\n"
                          "Threshold: %4 dB, FFT: %5 points\n"
                          "Strength: %6, Smoothing: %7")
                      .arg(method)
                      .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
                      .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss"))
                      .arg(noiseThreshold)
                      .arg(fftSize)
                      .arg(reductionStrength)
                      .arg(smoothing);
    } else {
        message = QString("Apply %1 noise reduction to entire track?\n"
                          "Threshold: %2 dB, FFT: %3 points\n"
                          "Strength: %4, Smoothing: %5")
                      .arg(method)
                      .arg(noiseThreshold)
                      .arg(fftSize)
                      .arg(reductionStrength)
                      .arg(smoothing);
    }

    if (QMessageBox::question(this, "Confirm Noise Reduction", message,
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    clearFutureHistory();
    stopPlayerAndResetUI();

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        QMessageBox::critical(this, "Error", "Cannot open audio file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t totalFrames = sf_seek(infile, 0, SEEK_END);
    sf_seek(infile, 0, SEEK_SET);

    if (totalFrames <= 0) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Invalid audio file or empty file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    qDebug() << "Total frames in file:" << totalFrames;
    qDebug() << "Sample rate:" << sfinfo.samplerate;
    qDebug() << "Channels:" << sfinfo.channels;

    qint64 startSample = applyToSelection ? (startMs * sfinfo.samplerate) / 1000 : 0;
    qint64 endSample = applyToSelection ? (endMs * sfinfo.samplerate) / 1000 : totalFrames;
    qint64 processSamples = endSample - startSample;

    if (startSample >= totalFrames) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Start position is beyond audio length");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    if (endSample > totalFrames) {
        endSample = totalFrames;
        processSamples = endSample - startSample;
    }

    qDebug() << "Processing" << processSamples << "samples from" << startSample << "to" << endSample;

    QVector<float> audioData(totalFrames * sfinfo.channels);

    const sf_count_t BUFFER_SIZE = 200000;
    QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);
    sf_count_t totalFramesRead = 0;

    while (totalFramesRead < totalFrames) {
        sf_count_t framesToRead = qMin(BUFFER_SIZE, totalFrames - totalFramesRead);
        sf_count_t framesRead = sf_readf_float(infile, buffer.data(), framesToRead);

        if (framesRead <= 0) {
            break;
        }

        memcpy(&audioData[totalFramesRead * sfinfo.channels], &buffer[0], framesRead * sfinfo.channels * sizeof(float));
        totalFramesRead += framesRead;

        qDebug() << "Read" << framesRead << "frames, total:" << totalFramesRead;
    }

    sf_close(infile);

    qDebug() << "Total frames read:" << totalFramesRead << "Expected:" << totalFrames;

    if (totalFramesRead == 0) {
        QMessageBox::critical(this, "Error", "No audio data read from file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    if (totalFramesRead < totalFrames) {
        qDebug() << "Adjusting audio data size from" << totalFrames << "to" << totalFramesRead;
        audioData.resize(totalFramesRead * sfinfo.channels);
        totalFrames = totalFramesRead;

        if (endSample > totalFrames) {
            endSample = totalFrames;
            processSamples = endSample - startSample;
        }
    }

    if (startSample >= totalFrames || processSamples <= 0) {
        QMessageBox::critical(this, "Error",
                              QString("Invalid selection range: startSample=%1, totalFrames=%2, processSamples=%3")
                                  .arg(startSample).arg(totalFrames).arg(processSamples));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    qDebug() << "Applying noise reduction to" << processSamples << "samples";

    CNoiseReductionEffect().apply(audioData, sfinfo.samplerate, sfinfo.channels, startSample, processSamples,
                                  noiseThreshold, reductionStrength, smoothing);


    QString tempFile = createTempFilePath("temp_noise_reduction");

    SF_INFO outinfo = sfinfo;
    outinfo.frames = totalFrames;
    SNDFILE* outfile = sf_open(tempFile.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        QMessageBox::critical(this, "Error", "Cannot create temporary file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t framesWritten = sf_writef_float(outfile, audioData.data(), totalFrames);
    sf_close(outfile);

    qDebug() << "Frames written:" << framesWritten << "Expected:" << totalFrames;

    if (framesWritten != totalFrames) {
        QFile::remove(tempFile);
        QMessageBox::warning(this, "Warning",
                             QString("Partially written audio data. Wrote %1 out of %2 frames")
                                 .arg(framesWritten).arg(totalFrames));
    }

    QFileInfo tempFileInfo(tempFile);
    if (!tempFileInfo.exists() || tempFileInfo.size() == 0) {
        QMessageBox::critical(this, "Error", "Created temporary file is empty or invalid");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    updateProjectWithNewFile(tempFile, totalFrames, sfinfo.samplerate);

    QMessageBox::information(this, "Success",
                             QString("Noise reduction applied successfully\n"
                                     "Processed %1 samples with %2 method")
                                 .arg(processSamples).arg(method));
}

void ProjectWorkWindow::applyFadeIn()
{
    FadeDialog dialog(FadeDialog::FadeIn, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    double fadeDuration = dialog.getFadeDuration();
    QString curveType = dialog.getCurveType();
    bool applyToSelection = dialog.applyToSelection();

    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;

    if (applyToSelection && startMs >= endMs) {
        QMessageBox::warning(this, "Warning", "Please select an audio range first or choose 'Entire Track'");
        return;
    }

    QString message;
    if (applyToSelection) {
        message = QString("Apply fade in to selection (%1 - %2)?\nDuration: %3 seconds, Curve: %4")
        .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
            .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss"))
            .arg(fadeDuration)
            .arg(curveType);
    } else {
        message = QString("Apply fade in to entire track?\nDuration: %1 seconds, Curve: %2")
        .arg(fadeDuration)
            .arg(curveType);
    }

    if (QMessageBox::question(this, "Confirm Fade In", message,
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    applyFadeEffect(true, fadeDuration, curveType, applyToSelection, startMs, endMs);
}

void ProjectWorkWindow::applyFadeOut()
{
    FadeDialog dialog(FadeDialog::FadeOut, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    double fadeDuration = dialog.getFadeDuration();
    QString curveType = dialog.getCurveType();
    bool applyToSelection = dialog.applyToSelection();

    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;

    if (applyToSelection && startMs >= endMs) {
        QMessageBox::warning(this, "Warning", "Please select an audio range first or choose 'Entire Track'");
        return;
    }

    QString message;
    if (applyToSelection) {
        message = QString("Apply fade out to selection (%1 - %2)?\nDuration: %3 seconds, Curve: %4")
        .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
            .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss"))
            .arg(fadeDuration)
            .arg(curveType);
    } else {
        message = QString("Apply fade out to entire track?\nDuration: %1 seconds, Curve: %2")
        .arg(fadeDuration)
            .arg(curveType);
    }

    if (QMessageBox::question(this, "Confirm Fade Out", message,
                              QMessageBox::Yes|QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    applyFadeEffect(false, fadeDuration, curveType, applyToSelection, startMs, endMs);
}

void ProjectWorkWindow::applyFadeEffect(bool isFadeIn, double fadeDuration, const QString& curveTypeName,
                                        bool applyToSelection, qint64 startMs, qint64 endMs)
{

    clearFutureHistory();
    stopPlayerAndResetUI();

    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        QMessageBox::critical(this, "Error", "Cannot open audio file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t totalFrames = sf_seek(infile, 0, SEEK_END);
    sf_seek(infile, 0, SEEK_SET);

    if (totalFrames <= 0) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Invalid audio file or empty file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    qDebug() << "Audio file info: frames:" << totalFrames << "samplerate:" << sfinfo.samplerate << "channels:" << sfinfo.channels;

    qint64 startSample = applyToSelection ? (startMs * sfinfo.samplerate) / 1000 : 0;
    qint64 endSample = applyToSelection ? (endMs * sfinfo.samplerate) / 1000 : totalFrames;
    qint64 processSamples = endSample - startSample;

    if (startSample >= totalFrames) {
        sf_close(infile);
        QMessageBox::critical(this, "Error", "Start position is beyond audio length");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    if (endSample > totalFrames) {
        endSample = totalFrames;
        processSamples = endSample - startSample;
    }

    qint64 fadeSamples = static_cast<qint64>(fadeDuration * sfinfo.samplerate);

    if (fadeSamples > processSamples) {
        fadeSamples = processSamples;
        QMessageBox::warning(this, "Warning",
                             QString("Fade duration adjusted to %1 seconds to fit selection")
                                 .arg(static_cast<double>(fadeSamples) / sfinfo.samplerate, 0, 'f', 1));
    }

    qDebug() << "Processing fade:" << (isFadeIn ? "IN" : "OUT")
             << "startSample:" << startSample << "endSample:" << endSample
             << "fadeSamples:" << fadeSamples;

    QVector<float> audioData(totalFrames * sfinfo.channels);

    const sf_count_t BUFFER_SIZE = 200000;
    QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);
    sf_count_t totalFramesRead = 0;

    while (totalFramesRead < totalFrames) {
        sf_count_t framesToRead = qMin(BUFFER_SIZE, totalFrames - totalFramesRead);
        sf_count_t framesRead = sf_readf_float(infile, buffer.data(), framesToRead);

        if (framesRead <= 0) {
            break;
        }

        memcpy(&audioData[totalFramesRead * sfinfo.channels], &buffer[0], framesRead * sfinfo.channels * sizeof(float));
        totalFramesRead += framesRead;
    }

    sf_close(infile);

    qDebug() << "Total frames read:" << totalFramesRead << "Expected:" << totalFrames;

    if (totalFramesRead == 0) {
        QMessageBox::critical(this, "Error", "No audio data read from file");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    if (totalFramesRead < totalFrames) {
        qDebug() << "Adjusting audio data size from" << totalFrames << "to" << totalFramesRead;
        audioData.resize(totalFramesRead * sfinfo.channels);
        totalFrames = totalFramesRead;

        if (endSample > totalFrames) {
            endSample = totalFrames;
            processSamples = endSample - startSample;
        }
    }

    if (startSample >= totalFrames || processSamples <= 0) {
        QMessageBox::critical(this, "Error",
                              QString("Invalid selection range: startSample=%1, totalFrames=%2, processSamples=%3")
                                  .arg(startSample).arg(totalFrames).arg(processSamples));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    CFadeEffect::ECurveType curveType = CFadeEffect::resolve(curveTypeName);
    if (curveType != CFadeEffect::UNDEFINED) {
        if (isFadeIn) {
            CFadeInEffect().apply(audioData, sfinfo.channels, startSample, fadeSamples, curveType);
        } else {
            CFadeOutEffect().apply(audioData, sfinfo.channels, endSample - fadeSamples, fadeSamples, curveType);
        }
    } else {
        // error
    }

    QString tempFile = createTempFilePath(QString("temp_fade_") + (isFadeIn ? "in" : "out"));

    if (QFile::exists(tempFile)) {
        QFile::remove(tempFile);
    }

    SF_INFO outinfo = sfinfo;
    outinfo.frames = totalFrames;
    SNDFILE* outfile = sf_open(tempFile.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        QMessageBox::critical(this, "Error", "Cannot create temporary file: " + QString(sf_strerror(NULL)));
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    sf_count_t totalFramesWritten = 0;
    const sf_count_t WRITE_BUFFER_SIZE = 200000;

    for (sf_count_t i = 0; i < totalFrames; i += WRITE_BUFFER_SIZE) {
        sf_count_t framesThisTime = qMin(WRITE_BUFFER_SIZE, totalFrames - i);
        sf_count_t framesWritten = sf_writef_float(outfile,
                                                   audioData.data() + (i * sfinfo.channels),
                                                   framesThisTime);

        if (framesWritten != framesThisTime) {
            sf_close(outfile);
            QFile::remove(tempFile);
            QMessageBox::critical(this, "Error",
                                  QString("Failed to write audio data at position %1").arg(i));
            player->setSource(QUrl::fromLocalFile(projectPath));
            return;
        }

        totalFramesWritten += framesWritten;
    }

    sf_close(outfile);

    qDebug() << "Frames written:" << totalFramesWritten << "Expected:" << totalFrames;

    if (totalFramesWritten != totalFrames) {
        QFile::remove(tempFile);
        QMessageBox::warning(this, "Warning",
                             QString("Partially written audio data. Wrote %1 out of %2 frames")
                                 .arg(totalFramesWritten).arg(totalFrames));
    }

    QFileInfo tempFileInfo(tempFile);
    if (!tempFileInfo.exists() || tempFileInfo.size() == 0) {
        QMessageBox::critical(this, "Error", "Created temporary file is empty or invalid");
        player->setSource(QUrl::fromLocalFile(projectPath));
        return;
    }

    updateProjectWithNewFile(tempFile, totalFrames, sfinfo.samplerate);

    QString effectName = isFadeIn ? "Fade in" : "Fade out";
    QMessageBox::information(this, "Success",
                             QString("%1 applied successfully\nDuration: %2 seconds, Curve: %3")
                                 .arg(effectName)
                                 .arg(fadeDuration)
                                 .arg(curveType));
}

void ProjectWorkWindow::zoomIn()
{
    if (waveformWidget) {
        waveformWidget->zoomIn();
    }
}

void ProjectWorkWindow::on_zoomInButton_clicked()
{
    zoomIn();
}

void ProjectWorkWindow::zoomOut()
{
    if (waveformWidget) {
        waveformWidget->zoomOut();
    }
}

void ProjectWorkWindow::on_zoomOutButton_clicked()
{
    zoomOut();
}

void ProjectWorkWindow::newFile()
{
    this->close();
    CreateProjectWindow *dlg = new CreateProjectWindow(this);
    dlg->show();
}

void ProjectWorkWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Audio File",
                                                    QDir::homePath(),
                                                    "Audio Files (*.wav *.mp3 *.flac);;"
                                                    "All Files (*)");

    if (!fileName.isEmpty()) {
        if (currentHistoryIndex > 0) {
            int result = QMessageBox::question(this, "Unsaved Changes",
                                               "Current project has unsaved changes. Do you want to save before opening new file?",
                                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            if (result == QMessageBox::Yes) {
                save();
            } else if (result == QMessageBox::Cancel) {
                return;
            }
        }

        loadAndSetAudioFile(fileName);
        updateProjectInfo(fileName);
        loadAudioForVisualization(fileName);

        QMessageBox::information(this, "Open File",
                                 QString("Opened file: %1").arg(fileName));
    }
}

void ProjectWorkWindow::updateProjectInfo(const QString& filePath)
{
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* file = sf_open(filePath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (file) {
        projectName = QFileInfo(filePath).fileName();
        projectDuration = double(sfinfo.frames) / sfinfo.samplerate;
        projectSamplingRate = sfinfo.samplerate;
        projectChannelsCount = sfinfo.channels;
        projectFormat = getFormatDescription(sfinfo.format);
        projectPath = filePath;
        originalFilePath = filePath;

        ui->projectNameLabel->setText(projectName);
        waveformWidget->setDuration(projectDuration * 1000);

        undoHistory.clear();
        currentHistoryIndex = -1;

        AudioState initialState;
        initialState.filePath = filePath;
        initialState.startMs = 0;
        initialState.endMs = projectDuration * 1000;
        initialState.timestamp = QDateTime::currentDateTime();

        undoHistory.append(initialState);
        currentHistoryIndex = 0;

        ui->undoButton->setEnabled(false);
        ui->redoButton->setEnabled(false);

        ui->durationLabel->setText(getPositionLabel(projectDuration, projectDuration));
        ui->currentTimeLabel->setText(getPositionLabel(0.0, projectDuration));
        ui->timelineSlider->setRange(0, projectDuration * 1000);

        sf_close(file);
    }
}

QString ProjectWorkWindow::getFormatDescription(int format)
{
    QString description;

    switch (format & SF_FORMAT_TYPEMASK) {
    case SF_FORMAT_WAV: description = "WAV"; break;
    case SF_FORMAT_FLAC: description = "FLAC"; break;
    case SF_FORMAT_MPEG: description = "MP3"; break;
    default: description = "Unknown"; break;
    }

    description += " (";
    switch (format & SF_FORMAT_SUBMASK) {
    case SF_FORMAT_PCM_S8: description += "8-bit PCM"; break;
    case SF_FORMAT_PCM_16: description += "16-bit PCM"; break;
    case SF_FORMAT_PCM_24: description += "24-bit PCM"; break;
    case SF_FORMAT_PCM_32: description += "32-bit PCM"; break;
    case SF_FORMAT_FLOAT: description += "32-bit Float"; break;
    case SF_FORMAT_DOUBLE: description += "64-bit Double"; break;
    }
    description += ")";

    return description;
}

void ProjectWorkWindow::closeFile()
{
    if (currentHistoryIndex > 0) {
        int result = QMessageBox::question(this, "Unsaved Changes",
                                           "Current project has unsaved changes. Do you want to save before closing?",
                                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (result == QMessageBox::Yes) {
            save();
        } else if (result == QMessageBox::Cancel) {
            return;
        }
    }

    if (player->playbackState() != QMediaPlayer::StoppedState) {
        player->stop();
    }

    disconnect(player, &QMediaPlayer::positionChanged, this, &ProjectWorkWindow::updatePosition);
    disconnect(player, &QMediaPlayer::durationChanged, this, &ProjectWorkWindow::updateDuration);

    player->setSource(QUrl());

    for (const AudioState& state : undoHistory) {
        if (!state.filePath.isEmpty() && state.filePath != originalFilePath && QFile::exists(state.filePath)) {
            QFile tempFile(state.filePath);
            if (tempFile.exists()) {
                tempFile.remove();
            }
        }
    }

    projectName = "Untitled";
    projectDuration = 0;
    projectSamplingRate = 0;
    projectChannelsCount = 2;
    projectFormat = "None";
    projectPath = "";
    originalFilePath = "";

    ui->projectNameLabel->setText("Untitled");
    ui->durationLabel->setText("00:00");
    ui->currentTimeLabel->setText("00:00");
    ui->timelineSlider->setRange(0, 0);
    ui->timelineSlider->setValue(0);

    QVector<float> emptyData;
    if (waveformWidget) {
        waveformWidget->setAudioData(emptyData);
        waveformWidget->setDuration(0);
        waveformWidget->setPosition(0);
    }

    undoHistory.clear();
    currentHistoryIndex = -1;

    if (ui->undoButton && ui->redoButton) {
        ui->undoButton->setEnabled(false);
        ui->redoButton->setEnabled(false);
    }

    if (ui->playPauseButton) {
        QIcon pauseIcon(":/icons/images/pause.png");
        ui->playPauseButton->setIcon(pauseIcon);
    }
    isPlaying = false;

    copiedAudioData.clear();
    cutAudioData.clear();
    hasCopiedData = false;
    hasCutData = false;
    lastActionWasCut = false;

    connect(player, &QMediaPlayer::positionChanged, this, &ProjectWorkWindow::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &ProjectWorkWindow::updateDuration);

    this->close();
    MainWindow *dlg = new MainWindow(this);
    dlg->show();

    QMessageBox::information(this, "Close", "Project closed");
}

void ProjectWorkWindow::showInfo()
{
    QString message = QString(
                          "<h3>Project Information</h3>"
                          "<table style='border-collapse: collapse; width: 100%;'>"
                          "<tr><td style='padding: 5px; border-bottom: 1px solid #555;'><b>Name:</b></td><td style='padding: 5px; border-bottom: 1px solid #555;'>%1</td></tr>"
                          "<tr><td style='padding: 5px; border-bottom: 1px solid #555;'><b>Duration:</b></td><td style='padding: 5px; border-bottom: 1px solid #555;'>%2</td></tr>"
                          "<tr><td style='padding: 5px; border-bottom: 1px solid #555;'><b>Sampling Rate:</b></td><td style='padding: 5px; border-bottom: 1px solid #555;'>%3 Hz</td></tr>"
                          "<tr><td style='padding: 5px; border-bottom: 1px solid #555;'><b>Format:</b></td><td style='padding: 5px; border-bottom: 1px solid #555;'>%4</td></tr>"
                          "<tr><td style='padding: 5px;'><b>Path:</b></td><td style='padding: 5px;'>%5</td></tr>"
                          "</table>"
                          ).arg(projectName)
                          .arg(QTime::fromMSecsSinceStartOfDay(projectDuration * 1000).toString("mm:ss"))
                          .arg(projectSamplingRate)
                          .arg(projectFormat)
                          .arg(projectPath.isEmpty() ? "Not saved" : projectPath);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Project Information");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(message);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.exec();
}

void ProjectWorkWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Audio File As",
                                                    QDir::homePath(),
                                                    "WAV (*.wav);;FLAC (*.flac);;MP3 (*.mp3)");

    if (!fileName.isEmpty()) {
        int format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
        if (fileName.endsWith(".flac", Qt::CaseInsensitive)) {
            format = SF_FORMAT_FLAC | SF_FORMAT_PCM_16;
        } else if (fileName.endsWith(".mp3", Qt::CaseInsensitive)) {
            format = SF_FORMAT_MPEG | SF_FORMAT_MPEG_LAYER_III;
        }

        if (convertAudioFormat(projectPath, fileName, format)) {
            updateProjectInfo(fileName);
            QMessageBox::information(this, "Save As", "File saved successfully");
        } else {
            QMessageBox::critical(this, "Error", "Failed to save file");
        }
    }
}

bool ProjectWorkWindow::convertAudioFormat(const QString& inputPath, const QString& outputPath, int format)
{
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(sfinfo));
    SNDFILE* infile = sf_open(inputPath.toUtf8().constData(), SFM_READ, &sfinfo);

    if (!infile) {
        return false;
    }

    SF_INFO outinfo = sfinfo;
    outinfo.format = format;

    SNDFILE* outfile = sf_open(outputPath.toUtf8().constData(), SFM_WRITE, &outinfo);
    if (!outfile) {
        sf_close(infile);
        return false;
    }

    const int BUFFER_SIZE = 200000;
    QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);

    sf_count_t framesRead;
    while ((framesRead = sf_readf_float(infile, buffer.data(), BUFFER_SIZE)) > 0) {
        sf_writef_float(outfile, buffer.data(), framesRead);
    }

    sf_close(infile);
    sf_close(outfile);

    return true;
}

void ProjectWorkWindow::saveSelectionAs()
{
    auto selection = waveformWidget->getSelection();
    qint64 startMs = selection.first;
    qint64 endMs = selection.second;

    if (startMs >= endMs) {
        QMessageBox::warning(this, "Error", "Please select a valid audio range first");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save Selection As",
                                                    QDir::homePath(),
                                                    "WAV (*.wav);;FLAC (*.flac);;MP3 (*.mp3)");

    if (!fileName.isEmpty()) {
        SF_INFO sfinfo;
        memset(&sfinfo, 0, sizeof(sfinfo));
        SNDFILE* infile = sf_open(projectPath.toUtf8().constData(), SFM_READ, &sfinfo);

        if (!infile) {
            QMessageBox::critical(this, "Error", "Cannot open audio file");
            return;
        }

        qint64 startSample = (startMs * sfinfo.samplerate) / 1000;
        qint64 endSample = (endMs * sfinfo.samplerate) / 1000;
        qint64 selectionSamples = endSample - startSample;

        int format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
        if (fileName.endsWith(".flac", Qt::CaseInsensitive)) {
            format = SF_FORMAT_FLAC | SF_FORMAT_PCM_16;
        } if (fileName.endsWith(".mp3", Qt::CaseInsensitive)) {
            format = SF_FORMAT_MPEG | SF_FORMAT_MPEG_LAYER_III;
        }

        SF_INFO outinfo = sfinfo;
        outinfo.format = format;
        outinfo.frames = selectionSamples;

        SNDFILE* outfile = sf_open(fileName.toUtf8().constData(), SFM_WRITE, &outinfo);
        if (!outfile) {
            sf_close(infile);
            QMessageBox::critical(this, "Error", "Cannot create output file");
            return;
        }

        sf_seek(infile, startSample, SEEK_SET);

        const int BUFFER_SIZE = 200000;
        QVector<float> buffer(BUFFER_SIZE * sfinfo.channels);
        sf_count_t totalFramesWritten = 0;

        while (totalFramesWritten < selectionSamples) {
            sf_count_t framesToRead = qMin(BUFFER_SIZE, selectionSamples - totalFramesWritten);
            sf_count_t framesRead = sf_readf_float(infile, buffer.data(), framesToRead);

            if (framesRead <= 0) break;

            sf_writef_float(outfile, buffer.data(), framesRead);
            totalFramesWritten += framesRead;
        }

        sf_close(infile);
        sf_close(outfile);

        QMessageBox::information(this, "Save Selection As",
                                 QString("Selection saved as: %1\nFrom %2 to %3")
                                     .arg(fileName)
                                     .arg(QTime::fromMSecsSinceStartOfDay(startMs).toString("mm:ss"))
                                     .arg(QTime::fromMSecsSinceStartOfDay(endMs).toString("mm:ss")));
    }
}

void ProjectWorkWindow::exitApplication()
{
    if (currentHistoryIndex > 0) {
        int result = QMessageBox::question(this, "Unsaved Changes",
                                           "Current project has unsaved changes. Do you want to save before exiting?",
                                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (result == QMessageBox::Yes) {
            save();
        } else if (result == QMessageBox::Cancel) {
            return;
        }
    }

    qApp->quit();
}
