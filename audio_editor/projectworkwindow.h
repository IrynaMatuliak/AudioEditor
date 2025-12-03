// projectworkwindow.h
#ifndef PROJECTWORKWINDOW_H
#define PROJECTWORKWINDOW_H

#include "waveformwidget.h"
#include "menubarmanager.h"
#include <sndfile.h>
#include <lame.h>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QLabel>
#include <QSlider>
#include <QDateTime>
#include <QPropertyAnimation>
#include <QAudioDevice>
#include <QMediaCaptureSession>
#include <QAudioInput>
#include <QMediaRecorder>
#include <QAudioFormat>
#include <QMediaFormat>
#include <QVBoxLayout>
<<<<<<< HEAD
=======
#include <complex>
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0

namespace Ui {
class ProjectWorkWindow;
}

class ProjectWorkWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProjectWorkWindow(QWidget *parent = nullptr);
    ~ProjectWorkWindow();

    void initializeProject(const QString& name, double duration, int samplingRate, int channels, const QString& format, const QString& projectPath);
    void loadAndSetAudioFile(const QString& filePath);
    QString getProjectPath() const { return projectPath; }
    void togglePlayPause();
    void reset();
    void rewind();
    void fastForward();
    void repeat();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void deleteAction();
    void save();
    void reverse();
    void applyEchoEffect();
    void applyEqualizerEffect();
    void applyNoiseReduction();
    void applyFadeIn();
    void applyFadeOut();
    void newFile();
    void openFile();
    void closeFile();
    void showInfo();
    void saveAs();
    void saveSelectionAs();
    void exitApplication();
<<<<<<< HEAD
    void zoomIn();
    void zoomOut();
=======
    // void zoomIn();
    // void zoomOut();
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0

signals:
    void returnToMainWindow();

private slots:
    // Playback control slots
    void on_playPauseButton_clicked();
    void on_resetButton_clicked();
    void on_rewindButton_clicked();
    void on_fastForwardButton_clicked();
    void on_repeatButton_clicked();
    void on_playbackStateChanged(QMediaPlayer::PlaybackState state);

    // Audio editing slots
    void on_trimButton_clicked();
    void on_undoButton_clicked();
    void on_redoButton_clicked();
    void on_copyButton_clicked();
    void on_pasteButton_clicked();
    void on_cutButton_clicked();
    void on_deleteButton_clicked();

    // Recording slots
    void on_recordButton_clicked();
    void onRecordingStateChanged(QMediaRecorder::RecorderState state);
    void onRecordingError(QMediaRecorder::Error error, const QString &errorString);

    // UI update slots
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void on_speedSlider_valueChanged(int value);
    void on_volumeSlider_valueChanged(int value);
    void updateVolumeIcon(int volume);
    void on_helpButton_clicked();
<<<<<<< HEAD
    void on_zoomOutButton_clicked();
    void on_zoomInButton_clicked();
=======
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0

private:

    virtual void closeEvent(QCloseEvent *event) override;

    // UI Components
    Ui::ProjectWorkWindow *ui;
    WaveformWidget *waveformWidget;
    MenuBarManager *menuManager;

    // to keep all WaveformWidget within
    QHBoxLayout *waveformLayout;
    QVector<WaveformWidget*> waveformWidgets;

    // Audio playback components
    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    // Recording components
    QMediaCaptureSession *captureSession;
    QAudioInput *audioInput;
    QMediaRecorder *mediaRecorder;

    // Project data
    QString projectName;
    double projectDuration;
    int projectSamplingRate;
    int projectChannelsCount;
    QString projectFormat;
    QString projectPath;

    // Playback state
    bool isPlaying;
    bool isRepeatOn = false;
    static constexpr qint64 REWIND_STEP_MS = 10000;

    // Editing state
    qint64 trimStartMs = 0;
    qint64 trimEndMs = 0;
    bool isTrimmed = false;

    // History management
    struct AudioState {
        QString filePath;
        qint64 startMs;
        qint64 endMs;
        QDateTime timestamp;
    };
    QList<AudioState> undoHistory;
    int currentHistoryIndex;
    QString originalFilePath;

    // Copy/Cut/Paste data
    QVector<float> copiedAudioData;
    int copiedSampleRate;
    int copiedChannels;
    qint64 copiedStartMs;
    qint64 copiedEndMs;
    bool hasCopiedData = false;

    QVector<float> cutAudioData;
    int cutSampleRate = 0;
    int cutChannels = 0;
    qint64 cutStartMs = 0;
    qint64 cutEndMs = 0;
    bool hasCutData = false;

    qint64 cutRegionStartMs = 0;
    qint64 cutRegionEndMs = 0;
    bool hasCutRegion = false;
    bool lastActionWasCut = false;

    // Recording state
    bool isRecording;
    QString recordedFilePath;
    qint64 recordStartPosition;
    qint64 recordEndPosition;

    // UI setup
    void setupMainLayout();
    void setupTopButtonsWithLabels(QVBoxLayout *mainLayout);
    void createButtonWithLabel(QHBoxLayout *layout, QPushButton *button, const QString &labelText);
    void setupTimelineSlider(QVBoxLayout *mainLayout);
    void setupWaveformArea(QVBoxLayout *mainLayout);
    void setupBottomControls(QVBoxLayout *mainLayout);
    void initializeComponents();
    void setupUI();
    void connectSignals();
    void setupStyles();
    void setupButton(QPushButton* button, const QString& iconPath, const QSize& iconSize);
    void setupSpeedControls();
    void setupVolumeControls();

    // Audio processing
    void loadAudioForVisualization(const QString &filePath);
    QVector<float> resampleAudio(const QVector<float>& audioData, int originalSampleRate, int targetSampleRate, int channels);
    void mixRecordedAudioWithOriginal();
<<<<<<< HEAD
=======
    void convertToMP3(const QString& inputPath, const QString& outputPath, int startMs, int endMs);
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
    QString getFormatFromSFInfo(const SF_INFO& info);
    void reverseAudioData(QVector<float>& audioData, int channels, sf_count_t totalFrames);

    void clearFutureHistory();
<<<<<<< HEAD
    void updateProjectWithNewFile(const QString& newFilePath, qint64 framesCount, int sampleRate);
=======
    void updateProjectWithNewFile(const QString& newFilePath, double newDurationSec, int sampleRate);
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0

    QString createTempFilePath(const QString& prefix);
    void stopPlayerAndResetUI();
    bool validateAudioSelection(qint64 startMs, qint64 endMs);
    void showSuccessMessage(const QString& message);
    void showErrorMessage(const QString& message);
    bool waitForMediaLoad(int timeoutMs = 3000);
    void reloadAudioVisualization();
<<<<<<< HEAD

    void applyFadeEffect(bool isFadeIn, double fadeDuration, const QString& curveTypeName, bool applyToSelection, qint64 startMs, qint64 endMs);

=======
    void applyEchoToAudioData(QVector<float>& audioData, int sampleRate, int channels, qint64 startSample, qint64 processSamples,
                              double delayMs, double decay, int repetitions);
    void applyEqualizerToAudioData(QVector<float>& audioData, int sampleRate, int channels, qint64 startSample, qint64 processSamples,
                                   double lowGain, double midGain, double highGain);
    double applyBiquadFilter(double input, double& x1, double& x2, double& y1, double& y2,
                             int sampleRate, double frequency, double gain, const QString& type);
    void applyNoiseReductionToAudioData(QVector<float>& audioData, int sampleRate, int channels, qint64 startSample, qint64 processSamples,
                                        float noiseThreshold, float reductionStrength, float smoothing);
    void applyFadeEffect(bool isFadeIn, double fadeDuration, const QString& curveType, bool applyToSelection, qint64 startMs, qint64 endMs);
    void applyFadeInToAudioData(QVector<float>& audioData, int channels, qint64 startSample, qint64 fadeSamples, const QString& curveType);
    void applyFadeOutToAudioData(QVector<float>& audioData, int channels, qint64 startSample, qint64 fadeSamples,const QString& curveType);
    double calculateFadeGain(double progress, const QString& curveType, bool isFadeIn);
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
    void updateProjectInfo(const QString& filePath);
    QString getFormatDescription(int format);
    bool convertAudioFormat(const QString& inputPath, const QString& outputPath, int format);
};

#endif // PROJECTWORKWINDOW_H
