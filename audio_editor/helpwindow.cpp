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
<p style="color: #cccccc;">Auditor has a powerful desktop application for creating and editing audio files. This guide will help you understand the basic features and get started with audio editing.</p>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Interface Overview</h2>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Timeline Editor</h3>
<p style="color: #cccccc;">The main workspace where you can enter and edit your audio coordinates.</p>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Tools Panel</h3>
<p style="color: #cccccc;">Contains all editing tools and editing you can apply to your audio.</p>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Quick Links</h2>
<ul style="color: #cccccc;">
<li style="margin-bottom: 10px;"><strong style="color: #ffffff;">Video Tutorials</strong><br>Reads through copy guides</li>
<li style="margin-bottom: 10px;"><strong style="color: #ffffff;">Documentation</strong><br>Read detailed documentation</li>
<li style="margin-bottom: 10px;"><strong style="color: #ffffff;">Community Filters</strong><br>Create a collection area</li>
</ul>
</div>
)";

    contentMap["Basic Tools"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">Basic Tools</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Essential tools for audio editing and manipulation.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Selection Tools</h2>
<ul style="color: #cccccc;">
<li>Time Selection Tool</li>
<li>Clip Selection Tool</li>
<li>Range Selection Tool</li>
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Editing Tools</h2>
<ul style="color: #cccccc;">
<li>Cut, Copy, Paste</li>
<<<<<<< HEAD
<li>Trim and Delete</li>
<li>Zoom In/Out</li>
<li>Volume Adjustment</li>
<li>Speed Adjustment</li>
=======
<li>Trim and Split</li>
<li>Fade In/Out</li>
<li>Volume Adjustment</li>
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
</ul>
</div>
)";

    contentMap["Audio Effects"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">Audio Effects</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Apply various effects to enhance your audio.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Common Effects</h2>
<ul style="color: #cccccc;">
<li>Equalizer</li>
<<<<<<< HEAD
<li>Reverse</li>
<li>Noise Reduction</li>
<li>Fade In/Out</li>
=======
<li>Reverb</li>
<li>Compression</li>
<li>Noise Reduction</li>
<li>Pitch Correction</li>
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Effect Chains</h2>
<p style="color: #cccccc;">Create and save effect chains for repeated use.</p>
</div>
)";

    contentMap["File Management"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">File Management</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Manage your audio projects and files efficiently.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Supported Formats</h2>
<ul style="color: #cccccc;">
<<<<<<< HEAD
<li>WAV, MP3, FLAC</li>
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Export Formats</h2>
<li>WAV, MP3, FLAC</li>
=======
<li>WAV, MP3, FLAC, AAC</li>
<li>Project Files (.aup)</li>
<li>Export Formats</li>
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
</ul>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Project Organization</h2>
<p style="color: #cccccc;">Keep your projects organized with folders and tags.</p>
</div>
)";

    contentMap["Settings"] = R"(
<h1 style="color: #00b4d8; font-size: 28px; margin-bottom: 20px;">Settings</h1>
<p style="color: #ffffff; font-size: 16px; line-height: 1.6;">Customize the application to your preferences.</p>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Common Settings</h2>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Audio Input Device</h3>
<p style="color: #cccccc;">Configure in Performance > Audio</p>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Sample Rate</h3>
<p style="color: #cccccc;">44.1kHz (default)</p>

<h3 style="color: #00b4d8; font-size: 18px; margin: 15px 0 10px 0;">Bit Depth</h3>
<p style="color: #cccccc;">24-bit</p>
</div>

<div style="background-color: #151515; border-radius: 8px; padding: 20px; margin: 15px 0; border: 1px solid #2a2a2a;">
<h2 style="color: #cccccc; font-size: 22px; margin-bottom: 15px;">Performance Settings</h2>
<ul style="color: #cccccc;">
<li>Buffer Size</li>
<li>Threading Options</li>
<li>Cache Settings</li>
</ul>
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
