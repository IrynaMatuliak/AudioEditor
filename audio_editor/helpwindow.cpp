#include "helpwindow.h"
#include <QScrollBar>
#include <QFont>

HelpWindow::HelpWindow(QWidget *parent, QWidget *previousWindow)
    : QMainWindow(parent)
    , m_previousWindow(previousWindow)
{
    setupUI();
    setupContent();

    setWindowTitle("Audio Editor Help");
    setMinimumSize(900, 650);
    resize(1100, 750);

    setStyleSheet("background-color: #0a0a0a;");
}

HelpWindow::~HelpWindow()
{
}

void HelpWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #0a0a0a;");
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    mainSplitter->setStyleSheet("QSplitter::handle { background-color: #1a1a1a; }");

    topicsList = new QListWidget(mainSplitter);
    topicsList->setMaximumWidth(280);
    topicsList->setStyleSheet(
        "QListWidget {"
        "    background-color: #151515;"
        "    border: 1px solid #2a2a2a;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    color: #ffffff;"
        "    outline: none;"
        "}"
        "QListWidget::item {"
        "    padding: 12px 15px;"
        "    border-bottom: 1px solid #2a2a2a;"
        "    background-color: #151515;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #1e1e1e;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #00b4d8;"
        "    color: #ffffff;"
        "    font-weight: bold;"
        "    border-left: 4px solid #ffffff;"
        "}"
        "QListWidget::item:first {"
        "    border-top-left-radius: 4px;"
        "    border-top-right-radius: 4px;"
        "}"
        "QListWidget::item:last {"
        "    border-bottom-left-radius: 4px;"
        "    border-bottom-right-radius: 4px;"
        "}"
        );

    contentScrollArea = new QScrollArea(mainSplitter);
    contentScrollArea->setWidgetResizable(true);
    contentScrollArea->setFrameShape(QFrame::NoFrame);
    contentScrollArea->setStyleSheet(
        "QScrollArea {"
        "    background-color: #0a0a0a;"
        "    border: none;"
        "}"
        "QScrollBar:vertical {"
        "    background-color: #151515;"
        "    width: 15px;"
        "    border-radius: 7px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: #333333;"
        "    border-radius: 7px;"
        "    min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: #444444;"
        "}"
        );

    contentWidget = new QWidget();
    contentWidget->setStyleSheet("background-color: #0a0a0a;");
    contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(20);

    contentScrollArea->setWidget(contentWidget);

    backButton = new QPushButton("Back");
    backButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #00b4d8;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0096c7;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0077b6;"
        "}"
        );
    backButton->setFixedWidth(100);

    mainSplitter->addWidget(topicsList);
    mainSplitter->addWidget(contentScrollArea);
    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setSizes({280, 700});

    mainLayout->addWidget(mainSplitter);

    connect(topicsList, &QListWidget::itemClicked, this, &HelpWindow::onTopicSelected);
    connect(backButton, &QPushButton::clicked, this, &HelpWindow::onBackButtonClicked);
}

void HelpWindow::createContentSection(const QString &title, const QString &content, QVBoxLayout *layout)
{
    QFrame *sectionFrame = new QFrame();
    sectionFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #151515;"
        "    border: 1px solid #2a2a2a;"
        "    border-radius: 8px;"
        "    padding: 0px;"
        "}"
        );

    QVBoxLayout *sectionLayout = new QVBoxLayout(sectionFrame);
    sectionLayout->setContentsMargins(20, 15, 20, 15);
    sectionLayout->setSpacing(10);

    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #00b4d8;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    padding: 5px 0px;"
        "}"
        );
    sectionLayout->addWidget(titleLabel);

    QLabel *contentLabel = new QLabel(content);
    contentLabel->setStyleSheet(
        "QLabel {"
        "    color: #cccccc;"
        "    font-size: 14px;"
        "    line-height: 1.5;"
        "    padding: 5px 0px;"
        "}"
        );
    contentLabel->setWordWrap(true);
    contentLabel->setTextFormat(Qt::RichText);
    sectionLayout->addWidget(contentLabel);

    layout->addWidget(sectionFrame);
}

void HelpWindow::setupContent()
{
    QStringList topics = {
        "Getting Started",
        "Basic Tools",
        "Audio Effects",
        "File Management",
        "Settings",
        "Keyboard Shortcuts",
        "Translate Learning"
    };

    topicsList->addItems(topics);

    contentMap["Getting Started"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">Getting Started with Audio Editor Application</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Learn the basics of audio editing with our comprehensive guide.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Introduction</h2>
<p style="color: #cccccc;">Audio Editor is a powerful desktop application for creating and editing audio files. This guide will help you understand the basic features and get started with audio editing.</p>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Complete User Manual</h2>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Starting the Application</h3>
<p style="color: #cccccc;">After launching the executable file with *.exe extension, the main program window opens.</p>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Help & Support</h3>
<p style="color: #cccccc;">Using the "Help & Support" option, you can view the application instructions containing explanations and information about the functionality provided by the software. The window opens on the "Getting Started" tab.</p>
<p style="color: #cccccc;">Navigation between help sections:</p>
<ul style="color: #cccccc;">
<li>From "Getting Started" → "Basic Tools"</li>
<li>From "Basic Tools" → "Audio Effects"</li>
<li>From "Audio Effects" → "File Management"</li>
<li>From "File Management" → "Settings"</li>
<li>From "Settings" → "Keyboard Shortcuts"</li>
<li>From "Keyboard Shortcuts" → "Translate Learning"</li>
</ul>
<p style="color: #cccccc;">Clicking the "Back" button returns you to the main menu.</p>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Creating a New Audio File</h3>
<p style="color: #cccccc;">From the main menu, go to "Create New Audio Project". The new audio file creation window opens where you can:</p>
<ul style="color: #cccccc;">
<li>Set the file name (max 30 characters, English alphabet only, "_" and " " allowed, cannot end with these symbols)</li>
<li>Select sample rate from dropdown (44.1kHz, 48kHz, 96kHz)</li>
<li>Choose file format (WAV, FLAC, MP3)</li>
<li>Set audio file length in seconds</li>
<li>Choose file path using the "Browse" button</li>
</ul>
<p style="color: #cccccc;">Clicking "Browse" opens a system window to select the save location in your computer's file system.</p>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Working with Projects</h3>
<p style="color: #cccccc;">After entering new audio file parameters, the new file opens in the project workspace.</p>
<p style="color: #cccccc;">To return to the main menu, use the "File" menu → "Close" option. The project closes.</p>
<p style="color: #cccccc;">To open an existing audio file, use "Import Project" from the main menu. Select the desired file in the system window. After selection, the file displays in the workspace.</p>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Audio Editing</h3>
<p style="color: #cccccc;">To edit a specific part of audio, select the desired area on the audio track visualization.</p>
<p style="color: #cccccc;">Available editing tools for the selected audio part:</p>
<ul style="color: #cccccc;">
<li>Trim - Crop the selected part</li>
<li>Cut - Cut the selection</li>
<li>Copy - Copy the selection</li>
<li>Paste - Paste copied/cut content</li>
<li>Delete - Remove the selection</li>
<li>Record - Record audio from microphone</li>
</ul>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Undo/Redo Functionality</h3>
<p style="color: #cccccc;">After an editing operation is performed, it's saved to the project state history. You can undo the previous action using the "Undo" button (becomes active after the first state change).</p>
<p style="color: #cccccc;">You can redo a previously undone action using the "Redo" button. All changes are saved in the state history, allowing you to manage project states.</p>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Recording from Microphone</h3>
<p style="color: #cccccc;">To record sound from microphone:</p>
<ol style="color: #cccccc;">
<li>Select the audio fragment where you want to overlay the recorded sound</li>
<li>Click the "Record" button and confirm recording</li>
<li>Recording starts - the "Record" button icon turns red</li>
<li>Click the "Record" button again to stop recording</li>
</ol>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Playback Controls</h3>
<p style="color: #cccccc;">Audio playback controls:</p>
<ul style="color: #cccccc;">
<li><strong>Play/Pause</strong> - Click the "Play" button to listen to audio. Click again to stop.</li>
<li><strong>Fast Forward/Rewind</strong> - Skip forward/backward several seconds using "Fast Forward" and "Rewind" buttons</li>
<li><strong>Volume Control</strong> - Drag the volume slider right to increase volume, left to decrease</li>
<li><strong>Speed Control</strong> - Drag the speed slider right to increase playback speed, left to decrease</li>
<li><strong>Reset</strong> - Return playback to beginning without using timeline slider</li>
<li><strong>Repeat</strong> - Enable loop playback. Button lights blue when active. Click again to disable.</li>
</ul>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Timeline Navigation</h3>
<p style="color: #cccccc;">Zoom the timeline scale using "Zoom In" to increase scale and "Zoom Out" to decrease scale.</p>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Menu Controls</h3>
<p style="color: #cccccc;">Alternative ways to control the application:</p>
<ul style="color: #cccccc;">
<li><strong>Edit Menu</strong> - Perform editing operations similar to editing buttons</li>
<li><strong>Control Menu</strong> - Control playback similar to playback control buttons</li>
<li><strong>Help Menu</strong> - Open the help window (also accessible via "Help & Support" in main menu)</li>
</ul>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Applying Audio Effects</h3>
<p style="color: #cccccc;">To apply an effect to part of an audio file:</p>
<ol style="color: #cccccc;">
<li>Select the desired part of the audio</li>
<li>Go to "Effects" menu and choose one of the available effects</li>
</ol>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">File Management</h3>
<p style="color: #cccccc;">The "File" menu contains standard commands for managing audio files and the application:</p>
<ul style="color: #cccccc;">
<li><strong>New</strong> - Create a new empty audio file</li>
<li><strong>Open</strong> - Open an existing audio file from disk for editing</li>
<li><strong>Close</strong> - Close the current active audio file without closing the application</li>
<li><strong>Information</strong> - Open information window about the current audio file</li>
<li><strong>Save</strong> - Save current changes to the same file it was opened from</li>
<li><strong>Save As</strong> - Save the current audio file with a new name, in a new format, or in a different location</li>
<li><strong>Save Selection As</strong> - Save only the selected fragment of the audio track as a separate file</li>
<li><strong>Exit</strong> - Close the application completely</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Quick Start Tips</h2>
<ul style="color: #cccccc;">
<li style="margin-bottom: 10px;">Use keyboard shortcuts for faster workflow (see "Keyboard Shortcuts" section)</li>
<li style="margin-bottom: 10px;">Always save your work regularly using Ctrl+S</li>
<li style="margin-bottom: 10px;">Use Undo/Redo (Ctrl+Z/Ctrl+Y) to experiment safely with edits</li>
<li style="margin-bottom: 10px;">Adjust zoom level for detailed editing of specific sections</li>
<li style="margin-bottom: 10px;">Test effects on a copy of your audio before applying to the original</li>
</ul>
</div>
)";

    contentMap["Basic Tools"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">Basic Tools</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Essential tools for audio editing and manipulation.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Selection Tools</h2>
<p style="color: #cccccc;">To edit audio, you first need to select the area you want to work with:</p>
<ul style="color: #cccccc;">
<li><strong>Time Selection Tool</strong> - Select a specific time range on the timeline</li>
<li><strong>Clip Selection Tool</strong> - Select entire audio clips or segments</li>
<li><strong>Range Selection Tool</strong> - Select a frequency range for spectral editing</li>
</ul>
<p style="color: #cccccc;">Tip: Click and drag on the waveform to make a selection. Double-click to select the entire track.</p>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Editing Tools</h2>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Basic Editing Operations</h3>
<ul style="color: #cccccc;">
<li><strong>Cut (Ctrl+X)</strong> - Remove selected audio and copy to clipboard</li>
<li><strong>Copy (Ctrl+C)</strong> - Copy selected audio to clipboard</li>
<li><strong>Paste (Ctrl+V)</strong> - Insert clipboard content at cursor position</li>
<li><strong>Trim</strong> - Remove everything outside the selection</li>
<li><strong>Delete</strong> - Permanently remove selected audio</li>
</ul>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Recording Tools</h3>
<p style="color: #cccccc;">The Record tool allows you to capture audio from your microphone:</p>
<ol style="color: #cccccc;">
<li>Select where you want to insert the recording</li>
<li>Click the "Record" button</li>
<li>Confirm recording when prompted</li>
<li>The button turns red while recording</li>
<li>Click again to stop recording</li>
</ol>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Zoom Tools</h3>
<p style="color: #cccccc;">For detailed editing, use zoom controls:</p>
<ul style="color: #cccccc;">
<li><strong>Zoom In</strong> - Increase timeline magnification for precise editing</li>
<li><strong>Zoom Out</strong> - Decrease magnification to see more of the timeline</li>
</ul>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Adjustment Tools</h3>
<ul style="color: #cccccc;">
<li><strong>Volume Adjustment</strong> - Change the loudness of selected audio</li>
<li><strong>Speed Adjustment</strong> - Modify playback speed without changing pitch</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Undo/Redo System</h2>
<p style="color: #cccccc;">All editing operations are saved in a history:</p>
<ul style="color: #cccccc;">
<li><strong>Undo (Ctrl+Z)</strong> - Reverse the last operation</li>
<li><strong>Redo (Ctrl+Y)</strong> - Restore an undone operation</li>
</ul>
<p style="color: #cccccc;">The Undo button becomes active after your first edit. You can undo multiple steps in sequence.</p>
</div>
)";

    contentMap["Audio Effects"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">Audio Effects</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Apply various effects to enhance your audio.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Applying Effects</h2>
<p style="color: #cccccc;">To apply an effect:</p>
<ol style="color: #cccccc;">
<li>Select the audio segment you want to modify</li>
<li>Go to "Effects" menu</li>
<li>Choose the desired effect from the list</li>
<li>Configure effect parameters in the dialog window</li>
<li>Click "Apply" to add the effect</li>
</ol>
<p style="color: #cccccc;">Effects can be applied to the entire track or only to selected portions.</p>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Equalizer</h2>
<p style="color: #cccccc;">Adjust frequency bands to shape your sound:</p>
<ul style="color: #cccccc;">
<li><strong>Bass</strong> - Controls low frequencies (-12 dB to +12 dB)
<ul>
<li>Adds depth and warmth to your audio</li>
<li>Reduces muddiness when lowered</li>
</ul>
</li>
<li><strong>Mid</strong> - Controls mid frequencies (-12 dB to +12 dB)
<ul>
<li>Affects vocals, guitars, pianos, and instrument harmonics</li>
<li>Adds presence when boosted</li>
</ul>
</li>
<li><strong>Treble</strong> - Controls high frequencies (-12 dB to +12 dB)
<ul>
<li>Adds brightness, clarity, and detail</li>
<li>Reduces harshness when lowered</li>
</ul>
</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Fade Effects</h2>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Fade Out</h3>
<p style="color: #cccccc;">Gradually decrease volume at the end of audio:</p>
<ul style="color: #cccccc;">
<li><strong>Fade Duration</strong> - Total time for volume decrease</li>
<li><strong>Fade Curve</strong> - Shape of volume change:
<ul>
<li>Linear - Constant rate decrease</li>
<li>Exponential - Fast start, slow end</li>
<li>Logarithmic - Slow start, fast end</li>
<li>S-Curve - Slow start/fast middle/slow end</li>
<li>Cosine - Smooth cosine-based transition</li>
</ul>
</li>
<li><strong>Apply to</strong> - Entire audio or selection only</li>
</ul>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Fade In</h3>
<p style="color: #cccccc;">Gradually increase volume at the beginning of audio:</p>
<ul style="color: #cccccc;">
<li><strong>Fade Duration</strong> - Total time for volume increase</li>
<li><strong>Fade Curve</strong> - Same curve options as Fade Out</li>
<li><strong>Apply to</strong> - Entire audio or selection only</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Reverse</h2>
<p style="color: #cccccc;">Play audio backwards. This effect has no parameters - simply select "Reverse" from the Effects menu to apply it to your selection.</p>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Noise Reduction</h2>
<p style="color: #cccccc;">Remove unwanted background noise:</p>
<ul style="color: #cccccc;">
<li><strong>Method</strong> - Algorithm for noise identification
<ul>
<li>Spectral Subtraction - Estimate noise spectrum and subtract from signal</li>
</ul>
</li>
<li><strong>Noise Threshold</strong> - Minimum volume level considered as noise</li>
<li><strong>FFT Size</strong> - Fourier Transform window size
<ul>
<li>Larger values: better frequency resolution, worse time resolution</li>
<li>Smaller values: better time resolution, worse frequency resolution</li>
</ul>
</li>
<li><strong>Reduction Strength</strong> - How aggressively to reduce noise
<ul>
<li>Higher values: more noise reduction but may cause artifacts</li>
</ul>
</li>
<li><strong>Smoothing</strong> - Smooth transitions between processed/unprocessed areas</li>
<li><strong>Learn noise profile from selection</strong> - Analyze selected noise-only section to create noise profile</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Echo</h2>
<p style="color: #cccccc;">Add repeating echoes to your audio:</p>
<ul style="color: #cccccc;">
<li><strong>Echo Delay</strong> - Time between original sound and first echo</li>
<li><strong>Echo Decay</strong> - How much quieter each echo repetition becomes</li>
<li><strong>Repetitions</strong> - Number of echo repetitions</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Effect Tips</h2>
<ul style="color: #cccccc;">
<li>Apply effects in this order for best results: Noise Reduction → Equalizer → Echo → Fade</li>
<li>Always listen to the effect before applying it permanently</li>
<li>Use "Apply to Selection" to test effects on small portions first</li>
<li>You can combine multiple effects for complex sound design</li>
</ul>
</div>
)";

    contentMap["File Management"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">File Management</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Manage your audio projects and files efficiently.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Supported Formats</h2>
<p style="color: #cccccc;">Audio Editor supports the following file formats:</p>
<ul style="color: #cccccc;">
<li><strong>Import Formats:</strong>
<ul>
<li>WAV (Waveform Audio File Format)</li>
<li>MP3 (MPEG Audio Layer III)</li>
<li>FLAC (Free Lossless Audio Codec)</li>
</ul>
</li>
<li><strong>Export Formats:</strong>
<ul>
<li>WAV - High quality, uncompressed</li>
<li>MP3 - Compressed, smaller file size</li>
<li>FLAC - Lossless compression</li>
</ul>
</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">File Menu Operations</h2>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Creating New Projects</h3>
<p style="color: #cccccc;">File → New or "Create New Audio Project":</p>
<ul style="color: #cccccc;">
<li>Filename: Max 30 characters (English alphabet, "_", " ")</li>
<li>Sample Rate: 44.1kHz, 48kHz, or 96kHz</li>
<li>Format: WAV, FLAC, or MP3</li>
<li>Duration: Length in seconds</li>
<li>Location: Choose via "Browse" button</li>
</ul>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Opening Files</h3>
<p style="color: #cccccc;">File → Open or "Import Project":</p>
<ol style="color: #cccccc;">
<li>Select file from system dialog</li>
<li>File loads into workspace</li>
<li>Project information displays in status bar</li>
</ol>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Saving Files</h3>
<ul style="color: #cccccc;">
<li><strong>Save (Ctrl+S)</strong> - Save changes to current file</li>
<li><strong>Save As</strong> - Save with new name/format/location</li>
<li><strong>Save Selection As</strong> - Export only selected portion</li>
</ul>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Closing and Exiting</h3>
<ul style="color: #cccccc;">
<li><strong>Close</strong> - Close current project, return to main menu</li>
<li><strong>Exit</strong> - Quit the application</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Project Information</h2>
<p style="color: #cccccc;">File → Information shows:</p>
<ul style="color: #cccccc;">
<li>File name and path</li>
<li>Format and sample rate</li>
<li>Duration</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Project Organization</h2>
<p style="color: #cccccc;">Tips for managing your audio projects:</p>
<ul style="color: #cccccc;">
<li>Use descriptive filenames</li>
<li>Organize projects in dedicated folders</li>
<li>Use "Save Selection As" for extracting samples</li>
<li>Keep original files before applying destructive edits</li>
</ul>
</div>
)";

    contentMap["Settings"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">Settings</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Customize the application to your preferences.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Audio Settings</h2>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Sample Rate</h3>
<p style="color: #cccccc;">Set default sample rate for new projects:</p>
<ul style="color: #cccccc;">
<li>44.1kHz - Standard for music (CD quality)</li>
<li>48kHz - Standard for video</li>
<li>96kHz - High-resolution audio</li>
</ul>
<p style="color: #cccccc;">Higher sample rates provide better frequency response but larger file sizes.</p>
</div>
)";

    contentMap["Keyboard Shortcuts"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">Keyboard Shortcuts</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Work faster with these keyboard shortcuts.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Editing Shortcuts</h2>
<ul style="color: #cccccc;">
<li>Ctrl+C - Copy</li>
<li>Ctrl+V - Paste</li>
<li>Ctrl+X - Cut</li>
<li>Ctrl+Z - Undo</li>
<li>Ctrl+Y - Redo</li>
<li>Space - Play/Pause</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Navigation Shortcuts</h2>
<ul style="color: #cccccc;">
<li>Home - Go to Start</li>
<li>End - Go to End</li>
<li>Ctrl+A - Select All</li>
</ul>
</div>
)";

    contentMap["Translate Learning"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">Translate Learning</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Help and support resources.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Read Help?</h2>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Technical Support</h3>
<p style="color: #cccccc;">Contact our support team for technical assistance:<br>
<strong style="color: #00b4d8;">support@mediaelibrary@audio.com</strong></p>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Additional Resources</h2>
<ul style="color: #cccccc;">
<li>Online Documentation</li>
<li>Video Tutorials</li>
<li>Community Forum</li>
</ul>
</div>
)";

    contentLayout->addWidget(backButton);
    contentLayout->addStretch();

    if (!topics.isEmpty()) {
        topicsList->setCurrentRow(0);
        onTopicSelected(topicsList->item(0));
    }
}

void HelpWindow::onTopicSelected(QListWidgetItem *item)
{
    QString topic = item->text();
    if (contentMap.contains(topic)) {
        QLayoutItem *child;
        while ((child = contentLayout->takeAt(1)) != nullptr) {
            delete child->widget();
            delete child;
        }

        if (contentLayout->indexOf(backButton) == -1) {
            contentLayout->insertWidget(0, backButton);
        }

        QTextBrowser *contentBrowser = new QTextBrowser();
        contentBrowser->setHtml(contentMap[topic]);
        contentBrowser->setOpenExternalLinks(true);
        contentBrowser->setStyleSheet(
            "QTextBrowser {"
            "    background-color: #0a0a0a;"
            "    border: none;"
            "    color: #cccccc;"
            "    font-size: 14px;"
            "    line-height: 1.5;"
            "}"
            "QTextBrowser a {"
            "    color: #00b4d8;"
            "    text-decoration: none;"
            "}"
            "QTextBrowser a:hover {"
            "    color: #48cae4;"
            "    text-decoration: underline;"
            "}"
            );

        contentLayout->insertWidget(1, contentBrowser);
    }
}

void HelpWindow::onBackButtonClicked()
{
    if (m_previousWindow) {
        m_previousWindow->show();
        m_previousWindow->raise();
        m_previousWindow->activateWindow();
    }
    close();
}
