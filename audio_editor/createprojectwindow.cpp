// createprojectwindow.cpp
#include "createprojectwindow.h"
#include "projectworkwindow.h"
#include "mainwindow.h"
#include "ui_createprojectwindow.h"
#include <QRegularExpression>
#include <QIntValidator>
#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include <QGroupBox>
#include <sndfile.h>
#include <lame.h>

CreateProjectWindow::CreateProjectWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CreateProjectWindow)
{
    ui->setupUi(this);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #000000; color: white;");
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    mainLayout->setAlignment(Qt::AlignCenter);

    QLabel *titleLabel = new QLabel("Create Project");
    titleLabel->setStyleSheet("font-size: 16pt; font-weight: bold; color: white;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QGroupBox *projectGroup = new QGroupBox("Create New Audio Project");
    projectGroup->setStyleSheet("QGroupBox { color: white; font-weight: bold; font-size: 14px; border: 1px solid #333; border-radius: 5px; margin-top: 10px; }"
                                "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; }");
    QVBoxLayout *groupLayout = new QVBoxLayout(projectGroup);
    groupLayout->setContentsMargins(30, 30, 30, 30);
    groupLayout->setSpacing(15);
    groupLayout->setAlignment(Qt::AlignCenter);

    QLabel *projectNameLabel = new QLabel("Project Name");
    projectNameLabel->setStyleSheet("color: white; font-size: 14px;");
    projectNameLabel->setAlignment(Qt::AlignLeft);
    groupLayout->addWidget(projectNameLabel);

    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameLayout->setAlignment(Qt::AlignCenter);
    nameLayout->setSpacing(10);

    ui->projectNameInput->setMinimumSize(681, 51);
    ui->projectNameInput->setStyleSheet("QTextEdit { background-color: #1E1E2F; color: white; border-radius: 3px; border: 1px solid #444; }");

    ui->browseButton->setFixedSize(101, 41);
    ui->browseButton->setStyleSheet("QPushButton { background-color: #1E1E2F; color: white; border-radius: 12px; border: 1px solid #444; }"
                                    "QPushButton:hover { background-color: #3A3A5C; }");

    nameLayout->addWidget(ui->projectNameInput);
    nameLayout->addWidget(ui->browseButton);
    groupLayout->addLayout(nameLayout);

    projectNameError = new QLabel();
    projectNameError->setStyleSheet("color: red; font-size: 14px;");
    projectNameError->setVisible(false);
    projectNameError->setAlignment(Qt::AlignLeft);
    groupLayout->addWidget(projectNameError);

    QLabel *samplingFreqLabel = new QLabel("Sampling Frequency (Rate)");
    samplingFreqLabel->setStyleSheet("color: white; font-size: 14px;");
    samplingFreqLabel->setAlignment(Qt::AlignLeft);
    groupLayout->addWidget(samplingFreqLabel);

    ui->samplingFrequencyBox->setMinimumSize(681, 51);
    ui->samplingFrequencyBox->setStyleSheet("QComboBox { background-color: #1E1E2F; color: white; border-radius: 3px; border: 1px solid #444; padding: 5px; }"
                                            "QComboBox::drop-down { border: none; width: 20px; }"
                                            "QComboBox::down-arrow {"
                                            "    image: url(:/icons/images/down_arrow.png);"
                                            "    width: 16px;"
                                            "    height: 16px;"
                                            "}");
    groupLayout->addWidget(ui->samplingFrequencyBox);

    QLabel *audioFormatLabel = new QLabel("Audio Format");
    audioFormatLabel->setStyleSheet("color: white; font-size: 14px;");
    audioFormatLabel->setAlignment(Qt::AlignLeft);
    groupLayout->addWidget(audioFormatLabel);

    ui->audioFormatBox->setMinimumSize(681, 51);
    ui->audioFormatBox->setStyleSheet("QComboBox { background-color: #1E1E2F; color: white; border-radius: 3px; border: 1px solid #444; padding: 5px; }"
                                      "QComboBox::drop-down { border: none; width: 20px; }"
                                      "QComboBox::down-arrow {"
                                      "    image: url(:/icons/images/down_arrow.png);"
                                      "    width: 16px;"
                                      "    height: 16px;"
                                      "}");
    groupLayout->addWidget(ui->audioFormatBox);

    QLabel *durationLabel = new QLabel("Duration (seconds)");
    durationLabel->setStyleSheet("color: white; font-size: 14px;");
    durationLabel->setAlignment(Qt::AlignLeft);
    groupLayout->addWidget(durationLabel);

    ui->durationInput->setMinimumSize(681, 51);
    ui->durationInput->setStyleSheet("QTextEdit { background-color: #1E1E2F; color: white; border-radius: 3px; border: 1px solid #444; }");
    groupLayout->addWidget(ui->durationInput);

    durationError = new QLabel();
    durationError->setStyleSheet("color: red; font-size: 14px;");
    durationError->setVisible(false);
    durationError->setAlignment(Qt::AlignLeft);
    groupLayout->addWidget(durationError);

    mainLayout->addWidget(projectGroup);

    mainLayout->addStretch(1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->addStretch(1);

    ui->cancelButton->setFixedSize(101, 41);
    ui->cancelButton->setStyleSheet("QPushButton { background-color: #1E1E2F; color: white; border-radius: 12px; border: 1px solid #444; }"
                                    "QPushButton:hover { background-color: #3A3A5C; }");

    ui->OKCreateProjectButton->setFixedSize(101, 41);
    ui->OKCreateProjectButton->setText("OK");
    ui->OKCreateProjectButton->setStyleSheet("QPushButton { background-color: #1E1E2F; color: white; border-radius: 12px; border: 1px solid #444; }"
                                             "QPushButton:hover { background-color: #3A3A5C; }");

    buttonLayout->addWidget(ui->cancelButton);
    buttonLayout->addWidget(ui->OKCreateProjectButton);
    mainLayout->addLayout(buttonLayout);

    ui->projectNameInput->setPlaceholderText("Enter project name");
    ui->durationInput->setPlaceholderText("Enter duration of audio");

    ui->samplingFrequencyBox->clear();
    ui->samplingFrequencyBox->addItems({"44.1 kHz", "48 kHz", "96 kHz"});
    ui->samplingFrequencyBox->setCurrentIndex(0);

    ui->audioFormatBox->clear();
    ui->audioFormatBox->addItems({"MP3", "WAV", "FLAC"});
    ui->audioFormatBox->setCurrentIndex(0);

    mainLayout->setStretchFactor(projectGroup, 1);
    groupLayout->setStretchFactor(nameLayout, 0);

    connect(ui->browseButton, &QPushButton::clicked, this, &CreateProjectWindow::browseForProjectFolder);
    connect(ui->projectNameInput, &QTextEdit::textChanged, this, &CreateProjectWindow::validateProjectName);
    connect(ui->durationInput, &QTextEdit::textChanged, this, &CreateProjectWindow::validateDuration);
<<<<<<< HEAD
=======
    connect(ui->OKCreateProjectButton, &QPushButton::clicked, this, &CreateProjectWindow::on_OKCreateProjectButton_clicked);
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
    connect(ui->cancelButton, &QPushButton::clicked, this, &CreateProjectWindow::on_cancelButton_clicked);

    setMinimumSize(1070, 685);
}

CreateProjectWindow::~CreateProjectWindow()
{
    delete ui;
}

void CreateProjectWindow::browseForProjectFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Project Folder",
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly);

    if (!dir.isEmpty()) {
        projectFolderPath = dir;
    }
}

void CreateProjectWindow::validateProjectName()
{
    QString projectName = ui->projectNameInput->toPlainText().trimmed();
    QRegularExpression regex("^[A-Za-z0-9_\\s]{1,30}$");
    QRegularExpressionMatch match = regex.match(projectName);

    if (projectName.isEmpty()) {
        projectNameError->setText("Project name cannot be empty");
        projectNameError->setVisible(true);
    }
    else if (!match.hasMatch()) {
        projectNameError->setText("Only A-Z, a-z, 0-9, space and _ allowed. Max 30 characters.");
        projectNameError->setVisible(true);
    }
    else if (projectName.endsWith('_')) {
        projectNameError->setText("Project name cannot end with underscore");
        projectNameError->setVisible(true);
    }
    else {
        projectNameError->setVisible(false);
    }
}

void CreateProjectWindow::validateDuration()
{
    QString text = ui->durationInput->toPlainText();
    bool ok;
    int value = text.toInt(&ok);

    if (!ok || value <= 0 || value > 300)
    {
        durationError->setText("Must be a number between 1 and 300");
        durationError->setVisible(true);
    }
    else
    {
        durationError->setVisible(false);
    }
}

void CreateProjectWindow::on_OKCreateProjectButton_clicked()
{
    validateProjectName();
    validateDuration();

    QStringList errors;

    if (projectNameError->isVisible()) {
        errors << "Project name: " + projectNameError->text();
    }

    if (durationError->isVisible()) {
        errors << "Duration: " + durationError->text();
    }

    if (ui->samplingFrequencyBox->currentIndex() == -1) {
        errors << "Sampling frequency: Not selected";
    }

    if (ui->audioFormatBox->currentIndex() == -1) {
        errors << "Audio format: Not selected";
    }

    if (projectFolderPath.isEmpty()) {
        errors << "Project location: Please select folder for project";
    }

    if (!errors.isEmpty()) {
        QString errorStyle = "QMessageBox { background-color: black; } "
                             "QLabel { color: white; } "
                             "QPushButton { color: white; background-color: #333; }";

        QMessageBox msgBox(this);
        msgBox.setStyleSheet(errorStyle);
        msgBox.setWindowTitle("Error");

        QString errorText;
        if (errors.count() == 1) {
            errorText = errors.first();
        } else {
            errorText = "Please correct the following errors:\n- " + errors.join("\n- ");
        }

        msgBox.setText(errorText);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }

    QString projectName = ui->projectNameInput->toPlainText().trimmed();
    int duration = ui->durationInput->toPlainText().toInt();
    QString samplingRateStr = ui->samplingFrequencyBox->currentText().split(" ").first();
    int samplingRate = static_cast<int>(samplingRateStr.toFloat() * 1000);
    QString audioFormat = ui->audioFormatBox->currentText();
    QString fullPath = projectFolderPath + "/" + projectName;

    QString fileExtension;
    int format = 0;

    if (audioFormat == "WAV") {
        fileExtension = "wav";
        format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    }
    else if (audioFormat == "FLAC") {
        fileExtension = "flac";
        format = SF_FORMAT_FLAC | SF_FORMAT_PCM_16;
    }
    else if (audioFormat == "MP3") {
        fileExtension = "mp3";
<<<<<<< HEAD
        format = SF_FORMAT_MPEG | SF_FORMAT_MPEG_LAYER_III;
=======
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
    }

    QString audioFilePath = fullPath + "." + fileExtension;

<<<<<<< HEAD
    createAudioFile(audioFilePath, duration, samplingRate, format);
=======
    if (audioFormat == "MP3") {
        createMp3File(audioFilePath, duration, samplingRate);
    } else {
        createAudioFile(audioFilePath, duration, samplingRate, format);
    }
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0

    ProjectWorkWindow *workWindow = new ProjectWorkWindow();
    workWindow->initializeProject(projectName, duration,
                                  samplingRate,
                                  2, // 2 channels by default
                                  audioFormat,
                                  audioFilePath);
    workWindow->show();

    this->close();
}

void CreateProjectWindow::createAudioFile(const QString &filePath, int durationSec, int sampleRate, int format)
{
    SF_INFO sfinfo;
<<<<<<< HEAD
    memset(&sfinfo, 0, sizeof(sfinfo));
=======
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
    sfinfo.samplerate = sampleRate;
    sfinfo.channels = 2;
    sfinfo.format = format;

    SNDFILE* file = sf_open(filePath.toUtf8().constData(), SFM_WRITE, &sfinfo);
    if (!file) {
        QMessageBox::critical(nullptr, "Error", "Failed to create audio file: " + QString(sf_strerror(NULL)));
        return;
    }

    const int bufferSize = 1024;
    const int channels = 2;
    std::vector<float> buffer(bufferSize * channels, 0.0f);
    int totalFrames = durationSec * sfinfo.samplerate;
    int framesWritten = 0;

    while (framesWritten < totalFrames) {
        int framesToWrite = std::min(bufferSize, totalFrames - framesWritten);
        sf_writef_float(file, buffer.data(), framesToWrite);
        framesWritten += framesToWrite;
    }

    sf_close(file);
}

<<<<<<< HEAD
=======
void CreateProjectWindow::createMp3File(const QString &filePath, int durationSec, int sampleRate)
{
    QString tempWavPath = QDir::tempPath() + "/temp_silence.wav";
    createAudioFile(tempWavPath, durationSec, sampleRate, SF_FORMAT_WAV | SF_FORMAT_PCM_16);

    lame_global_flags *lame = lame_init();
    lame_set_in_samplerate(lame, sampleRate);
    lame_set_num_channels(lame, 2);
    lame_set_quality(lame, 2);
    lame_init_params(lame);

    SF_INFO sfinfo;
    SNDFILE *infile = sf_open(tempWavPath.toUtf8().constData(), SFM_READ, &sfinfo);
    if (!infile) {
        QMessageBox::critical(nullptr, "Error", "Failed to open temporary WAV file");
        lame_close(lame);
        return;
    }

    FILE *mp3file = nullptr;
    fopen_s(&mp3file, filePath.toUtf8().constData(), "wb");
    if (!mp3file) {
        QMessageBox::critical(nullptr, "Error", "Failed to create MP3 file");
        sf_close(infile);
        lame_close(lame);
        return;
    }

    const int MP3_BUFFER_SIZE = 8192;
    short pcm_buffer[2 * 8192];
    unsigned char mp3_buffer[MP3_BUFFER_SIZE];

    int read, write;
    do {
        read = sf_readf_short(infile, pcm_buffer, 8192);
        if (read == 0) break;

        write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_BUFFER_SIZE);
        fwrite(mp3_buffer, 1, write, mp3file);
    } while (read > 0);

    write = lame_encode_flush(lame, mp3_buffer, MP3_BUFFER_SIZE);
    fwrite(mp3_buffer, 1, write, mp3file);

    sf_close(infile);
    fclose(mp3file);
    lame_close(lame);

    QFile::remove(tempWavPath);
}

>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
void CreateProjectWindow::on_cancelButton_clicked()
{
    QWidget *parentWindow = this->parentWidget();
    while (parentWindow) {
        if (qobject_cast<QMainWindow*>(parentWindow) ||
            qobject_cast<ProjectWorkWindow*>(parentWindow) ||
            qobject_cast<MainWindow*>(parentWindow)) {
            parentWindow->show();
            break;
        }
        parentWindow = parentWindow->parentWidget();
    }
    if (!parentWindow) {
        QWidget *mainWindow = QApplication::activeWindow();
        if (mainWindow && mainWindow != this) {
            mainWindow->show();
        }
    }

    this->close();
}
