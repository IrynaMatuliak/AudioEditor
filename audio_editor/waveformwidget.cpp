#include "waveformwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QTime>
#include <cmath>
#include <QScrollBar>
#include <QWheelEvent>
#include <QDebug>

WaveformWidget::WaveformWidget(QWidget *parent)
    : QWidget(parent)
    , cursorPosition(0)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(10, 10, 10));
    pal.setColor(QPalette::Base, QColor(20, 20, 20));
    setAutoFillBackground(true);
    setPalette(pal);

    setMouseTracking(true);

    m_selectionStartMs = 0;
    m_selectionEndMs = 0;
    m_draggingHandle = NoHandle;
    m_showSelection = false;
    m_timeScale = 2000;
    m_offsetMs = 0;
    m_minTimeScale = 500;
    m_maxTimeScale = 10000;
    m_horizontalScrollBar = new QScrollBar(Qt::Horizontal, this);
    m_horizontalScrollBar->setRange(0, 0);
    m_horizontalScrollBar->setPageStep(1000);
    m_horizontalScrollBar->hide();

    connect(m_horizontalScrollBar, &QScrollBar::valueChanged, this, [this](int value) {
        m_offsetMs = value;
        update();
    });
}

<<<<<<< HEAD
void WaveformWidget::setAudioData(const QVector<float> &data)
=======
void WaveformWidget::setAudioData(const QVector<qint16> &data)
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
{
    m_audioData = data;
    calculatePeaks();
    updateScrollBars();
    // stop to show selection after file has been changed
    m_selectionStartMs = 0;
    m_selectionEndMs = 0;
    m_showSelection = false;

    this->update();
}

void WaveformWidget::setDuration(qint64 durationMs)
{
    m_durationMs = durationMs;
    updateScrollBars();
    this->update();
}

void WaveformWidget::setPosition(qint64 positionMs)
{
    m_positionMs = positionMs;

    if (!m_dragging) {
        ensurePositionVisible(positionMs);
    }

    this->update();
}

void WaveformWidget::ensurePositionVisible(qint64 positionMs)
{
    if (m_durationMs <= 0) return;

    const int scaleMargin = 25;
    int visibleWidth = width() - scaleMargin;
    qint64 visibleDurationMs = static_cast<qint64>(m_timeScale * (visibleWidth / 100.0));

    if (positionMs < m_offsetMs || positionMs > m_offsetMs + visibleDurationMs) {
        m_offsetMs = qMax(0ll, positionMs - visibleDurationMs / 2);
        m_offsetMs = qMin(m_offsetMs, m_durationMs - visibleDurationMs);

        updateScrollBars();
    }
}

void WaveformWidget::calculatePeaks()
{
    if (m_audioData.isEmpty()) return;

    const int w = this->width();
<<<<<<< HEAD
    const int numBins = w * 80;
    m_peaks.resize(numBins);

    peakMax = 0.0f;

=======
    const int numBins = w * 1.5;
    m_peaks.resize(numBins);

>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
    const int samplesPerBin = m_audioData.size() / numBins;
    if (samplesPerBin == 0) return;

    for (int i = 0; i < numBins; ++i) {
<<<<<<< HEAD
        float peakValue = 0;
=======
        qint16 max = 0;
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
        const int start = i * samplesPerBin;
        const int end = qMin(start + samplesPerBin, m_audioData.size());

        for (int j = start; j < end; ++j) {
<<<<<<< HEAD
            peakValue = qMax(peakValue, qAbs(m_audioData[j]));
        }

        m_peaks[i] = qMin(peakValue, 1.0f);
        peakMax = qMax(peakMax, peakValue);
=======
            max = qMax(max, qAbs(m_audioData[j]));
        }

        m_peaks[i] = static_cast<float>(max) / 32768.0f;
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
    }
}

void WaveformWidget::updateScrollBars()
{
    if (m_durationMs <= 0) {
        m_horizontalScrollBar->hide();
        return;
    }

    const int scaleMargin = 25;
    int visibleWidth = width() - scaleMargin;
    qint64 visibleDurationMs = static_cast<qint64>(m_timeScale * (visibleWidth / 100.0));

    if (visibleDurationMs < m_durationMs) {
        m_horizontalScrollBar->show();
        m_horizontalScrollBar->setRange(0, m_durationMs - visibleDurationMs);
        m_horizontalScrollBar->setPageStep(visibleDurationMs);
        m_horizontalScrollBar->setValue(m_offsetMs);
        m_horizontalScrollBar->setGeometry(0, height() - 20, width(), 20);
    } else {
        m_offsetMs = 0;
        // recalculate time scale to fit entire audio track into the window
        m_timeScale = static_cast<int>(m_durationMs * 100 / visibleWidth);
        m_horizontalScrollBar->hide();
    }
}

void WaveformWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_durationMs > 0) {
        const int scaleMargin = 25;
        int visibleWidth = width() - scaleMargin;
        m_timeScale = static_cast<int>(m_durationMs * 100 / visibleWidth);
        m_timeScale = qMax(m_minTimeScale, qMin(m_maxTimeScale, m_timeScale));

        m_offsetMs = 0;
    }

    updateScrollBars();
    calculatePeaks();
    update();
}

void WaveformWidget::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int delta = event->angleDelta().y();

        if (delta > 0) {
            m_timeScale = qMax(m_minTimeScale, m_timeScale - 500);
        } else {
            m_timeScale = qMin(m_maxTimeScale, m_timeScale + 500);
        }

        const int scaleMargin = 25;
        int visibleWidth = width() - scaleMargin;
        qint64 visibleDurationMs = static_cast<qint64>(m_timeScale * (visibleWidth / 100.0));

        QPoint mousePos = event->position().toPoint();
        if (mousePos.x() >= 0 && mousePos.x() < width()) {
            double relativePos = static_cast<double>(mousePos.x() - scaleMargin) / visibleWidth;
            m_offsetMs = qMax(0ll, qMin(m_durationMs - visibleDurationMs,
                                        static_cast<qint64>(m_offsetMs + relativePos * (visibleDurationMs / 2))));
        }

        updateScrollBars();
        update();
        event->accept();
    } else {
        int delta = event->angleDelta().y();
        m_horizontalScrollBar->setValue(m_horizontalScrollBar->value() - delta);
        event->accept();
    }
}

void WaveformWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const int scaleMargin = 25;

        qint64 timeAtClick = screenXToTime(event->pos().x());

        int posX = timeToScreenX(m_positionMs);
        if (abs(event->pos().x() - posX) < 15) {
            m_dragging = true;
            update();
            return;
        }

        if (m_showSelection) {
            int startX = timeToScreenX(m_selectionStartMs);
            int endX = timeToScreenX(m_selectionEndMs);

            if (abs(event->pos().x() - startX) < 10) {
                m_draggingHandle = StartHandle;
                return;
            } else if (abs(event->pos().x() - endX) < 10) {
                m_draggingHandle = EndHandle;
                return;
            }
        }

        if (event->pos().x() >= scaleMargin) {
            m_showSelection = true;
            m_selectionStartMs = timeAtClick;
            m_selectionEndMs = timeAtClick;
            m_draggingHandle = EndHandle;
            update();
        }
    }
    QWidget::mousePressEvent(event);
}

void WaveformWidget::mouseMoveEvent(QMouseEvent *event)
{
    const int scaleMargin = 25;

    if (m_dragging) {
        qint64 newPosition = screenXToTime(event->pos().x());
        newPosition = qMax(0ll, qMin(m_durationMs, newPosition));

        if (newPosition != m_lastUserPosition) {
            m_positionMs = newPosition;
            m_lastUserPosition = newPosition;
            emit userPositionChanged(newPosition);
            update();
        }
    }
    else if (m_draggingHandle != NoHandle) {
        qint64 newTime = screenXToTime(event->pos().x());
        newTime = qMax(0ll, qMin(m_durationMs, newTime));

        if (m_draggingHandle == StartHandle) {
            m_selectionStartMs = qMin(newTime, m_selectionEndMs);
        } else {
            m_selectionEndMs = qMax(newTime, m_selectionStartMs);
        }

        emit selectionChanged(m_selectionStartMs, m_selectionEndMs);
        update();
    }
    else {
        if (m_showSelection) {
            int startX = timeToScreenX(m_selectionStartMs);
            int endX = timeToScreenX(m_selectionEndMs);

            if (abs(event->pos().x() - startX) < 15 || abs(event->pos().x() - endX) < 15) {
                setCursor(Qt::SizeHorCursor);
            } else {
                setCursor(Qt::ArrowCursor);
            }
        }
    }

    QWidget::mouseMoveEvent(event);
}

void WaveformWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_dragging) {
            m_dragging = false;
            m_lastUserPosition = -1;
        }
        if (m_draggingHandle != NoHandle) {
            m_draggingHandle = NoHandle;
        }
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

int WaveformWidget::timeToScreenX(qint64 timeMs) const
{
    const int scaleMargin = 25;
    const int w = width();

    if (m_durationMs <= 0) return scaleMargin;

    qint64 visibleDurationMs = static_cast<qint64>(m_timeScale * ((w - scaleMargin) / 100.0));
    qint64 startTime = m_offsetMs;
    qint64 endTime = startTime + visibleDurationMs;

    if (timeMs < startTime) return scaleMargin;
    if (timeMs > endTime) return w;

    double relativePos = static_cast<double>(timeMs - startTime) / visibleDurationMs;
    return scaleMargin + static_cast<int>(relativePos * (w - scaleMargin));
}

qint64 WaveformWidget::screenXToTime(int x) const
{
    const int scaleMargin = 25;
    const int w = width();

    if (m_durationMs <= 0) return 0;

    qint64 visibleDurationMs = static_cast<qint64>(m_timeScale * ((w - scaleMargin) / 100.0));

    double relativePos = static_cast<double>(x - scaleMargin) / (w - scaleMargin);
    relativePos = qMax(0.0, qMin(1.0, relativePos));
    return m_offsetMs + static_cast<qint64>(relativePos * visibleDurationMs);
}

int WaveformWidget::getOptimalTimeInterval() const
{
    if (m_durationMs <= 0) return 1000;
    const int scaleMargin = 25;
    int visibleWidth = width() - scaleMargin;
    double pixelsPerSecond = (visibleWidth * 1000.0) / (m_timeScale * (visibleWidth / 100.0));

    if (pixelsPerSecond > 200) {
        return 100;
    } else if (pixelsPerSecond > 100) {
        return 250;
    } else if (pixelsPerSecond > 50) {
        return 500;
    } else if (pixelsPerSecond > 25) {
        return 1000;
    } else if (pixelsPerSecond > 10) {
        return 2000;
    } else if (pixelsPerSecond > 5) {
        return 5000;
    } else if (pixelsPerSecond > 2) {
        return 10000;
    } else if (pixelsPerSecond > 1) {
        return 30000;
    } else {
        return 60000;
    }
}

<<<<<<< HEAD
namespace {
    const int scaleMargin = 25;
    const int timeLabelHeight = 35;
    const int timeLabelTopMargin = 10;
    const int timeLabelLabelDistance = 5;
}

void WaveformWidget::paintTimeMarkers(QPainter &painter, int w, int h, const QColor& scaleColor)
{
    struct TimeLabel {
        QRect rect;
        QString text;
    };

    std::vector<TimeLabel> labels;
    labels.reserve(50);
    bool doSkipOddLabels = false;

    int timeMarkerInterval = getOptimalTimeInterval();
    qint64 visibleStart = m_offsetMs;
    qint64 visibleEnd = visibleStart + static_cast<qint64>(m_timeScale * ((w - scaleMargin) / 100.0));
    qint64 startTime = (visibleStart / timeMarkerInterval) * timeMarkerInterval;
    QRect previousTextPlace(0, 0, 0, 0);
    for (qint64 timeMs = startTime; timeMs <= visibleEnd; timeMs += timeMarkerInterval) {
        if (timeMs > m_durationMs)
            break;
        const int x = timeToScreenX(timeMs);
        if (x < scaleMargin || x >= w)
            continue;
        painter.setPen(QPen(QColor(80, 80, 160), 1, Qt::DotLine));
        painter.drawLine(x, timeLabelTopMargin + 20, x, h - timeLabelHeight);
        painter.setPen(QPen(scaleColor, 1));

        QString text;
        if (timeMarkerInterval < 1000) {
            int seconds = timeMs / 1000;
            int milliseconds = timeMs % 1000;
            text = QString("%1.%2").arg(seconds).arg(milliseconds, 3, 10, QChar('0'));
        } else if (timeMarkerInterval < 60000) {
            int seconds = timeMs / 1000;
            int minutes = seconds / 60;
            seconds = seconds % 60;
            text = QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));

            if (timeMarkerInterval < 2000) {
                int ms = timeMs % 1000;
                text += QString(".%1").arg(ms / 100);
            }
        } else {
            int minutes = timeMs / 60000;
            int seconds = (timeMs % 60000) / 1000;
            text = QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
        }

        QRect textBR = painter.fontMetrics().boundingRect(text);
        textBR.adjust(-timeLabelLabelDistance, 0, +timeLabelLabelDistance, 0);
        textBR.moveCenter(QPoint(x, timeLabelTopMargin+10));

        doSkipOddLabels = previousTextPlace.intersects(textBR);
        if (doSkipOddLabels || previousTextPlace.isEmpty()) {
            previousTextPlace = textBR;
        }

        painter.drawLine(x, timeLabelTopMargin + 15, x, timeLabelTopMargin + 20);
        labels.push_back({textBR, text});
    }

    bool doDraw = true;
    for (auto& label: labels) {
        if (doDraw || !doSkipOddLabels) {
            painter.drawText(label.rect, Qt::AlignCenter, label.text);
        }
        doDraw = !doDraw;
    }
}

=======
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
void WaveformWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int w = this->width();
    const int h = this->height();
    const int centerY = h / 2;
<<<<<<< HEAD

=======
    const int scaleMargin = 25;
    const int timeLabelHeight = 35;
    const int timeLabelTopMargin = 10;
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
    painter.fillRect(0, 0, w, h, QColor(20, 20, 20));
    if (m_showSelection && m_selectionStartMs != m_selectionEndMs) {
        if (m_selectionStartMs > 0) {
            int leftX = timeToScreenX(0);
            int rightX = timeToScreenX(m_selectionStartMs);
            painter.fillRect(leftX, timeLabelTopMargin + 20, rightX - leftX, h - timeLabelTopMargin - 20, QColor(0x2A, 0x2A, 0x4C));
        }

        if (m_selectionEndMs < m_durationMs) {
            int leftX = timeToScreenX(m_selectionEndMs);
            int rightX = timeToScreenX(m_durationMs);
            painter.fillRect(leftX, timeLabelTopMargin + 20, rightX - leftX, h - timeLabelTopMargin - 20, QColor(0x2A, 0x2A, 0x4C));
        }
    }

    painter.fillRect(scaleMargin, timeLabelTopMargin + 20, w - scaleMargin, h - timeLabelHeight - timeLabelTopMargin - 20, QColor(20, 20, 20));

    QFont font = painter.font();
    font.setPointSize(9);
    painter.setFont(font);
<<<<<<< HEAD
    QFontMetrics fontMetrics(font);

    const float amplitudes[] = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f, 0.0f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f};
    const QColor scaleColor(180, 220, 255);

    const float waveformAreaHeight = h - timeLabelHeight - timeLabelTopMargin - 20;
    const float waveformAreaHeightMult = waveformAreaHeight / h;

    painter.setPen(QPen(scaleColor, 1));
    for (float amplitude : amplitudes) {
        int y = centerY - (amplitude * centerY * waveformAreaHeightMult);
=======

    const float amplitudes[] = {0.8f, 0.6f, 0.4f, 0.2f, 0.0f, -0.2f, -0.4f, -0.6f, -0.8f};
    const QColor scaleColor(180, 220, 255);

    painter.setPen(QPen(scaleColor, 1));
    for (float amplitude : amplitudes) {
        int y = centerY - (amplitude * centerY);
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
        painter.setPen(QPen(QColor(80, 80, 160), 1, Qt::DotLine));
        painter.drawLine(scaleMargin, y, w, y);
        painter.setPen(QPen(scaleColor, 1));
        painter.drawLine(scaleMargin - 5, y, scaleMargin, y);
        QString label = QString::number(amplitude, 'f', 1);
        QRect labelRect(scaleMargin - 45, y - 8, 40, 16);
        painter.drawText(labelRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }

    if (m_durationMs > 0) {
<<<<<<< HEAD
        paintTimeMarkers(painter, w, h, scaleColor);
=======
        int timeMarkerInterval = getOptimalTimeInterval();
        qint64 visibleStart = m_offsetMs;
        qint64 visibleEnd = visibleStart + static_cast<qint64>(m_timeScale * ((w - scaleMargin) / 100.0));
        qint64 startTime = (visibleStart / timeMarkerInterval) * timeMarkerInterval;
        for (qint64 timeMs = startTime; timeMs <= visibleEnd; timeMs += timeMarkerInterval) {
            if (timeMs > m_durationMs) break;
            int x = timeToScreenX(timeMs);
            if (x < scaleMargin || x >= w) continue;
            painter.setPen(QPen(QColor(80, 80, 160), 1, Qt::DotLine));
            painter.drawLine(x, timeLabelTopMargin + 20, x, h - timeLabelHeight);
            painter.setPen(QPen(scaleColor, 1));

            QString text;
            if (timeMarkerInterval < 1000) {
                int seconds = timeMs / 1000;
                int milliseconds = timeMs % 1000;
                text = QString("%1.%2").arg(seconds).arg(milliseconds, 3, 10, QChar('0'));
            } else if (timeMarkerInterval < 60000) {
                int seconds = timeMs / 1000;
                int minutes = seconds / 60;
                seconds = seconds % 60;
                text = QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));

                if (timeMarkerInterval < 2000) {
                    int ms = timeMs % 1000;
                    text += QString(".%1").arg(ms / 100);
                }
            } else {
                int minutes = timeMs / 60000;
                int seconds = (timeMs % 60000) / 1000;
                text = QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
            }

            QRect textRect(x - 40, timeLabelTopMargin, 80, 20);
            painter.drawText(textRect, Qt::AlignCenter, text);

            painter.drawLine(x, timeLabelTopMargin + 15, x, timeLabelTopMargin + 20);
        }

        // // Draw current time at the position indicator
        // if (m_positionMs >= visibleStart && m_positionMs <= visibleEnd) {
        //     int posX = timeToScreenX(m_positionMs);
        //     QTime currentTime = QTime::fromMSecsSinceStartOfDay(m_positionMs);
        //     QString timeText = currentTime.toString("m:ss.zzz");

        //     painter.setPen(QPen(Qt::yellow, 1));
        //     QRect timeRect(posX - 50, timeLabelTopMargin, 100, 20);
        //     painter.drawText(timeRect, Qt::AlignCenter, timeText);
        // }
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
    }

    painter.setPen(QPen(QColor(150, 200, 255), 1));
    painter.drawLine(scaleMargin, centerY, w, centerY);

    // Draw waveform
    if (!m_peaks.isEmpty()) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 255, 0));

        qint64 visibleStart = m_offsetMs;
        qint64 visibleEnd = visibleStart + static_cast<qint64>(m_timeScale * ((w - scaleMargin) / 100.0));

        int drawXPrev = 0;
        for (int x = 0; x < m_peaks.size(); ++x) {
<<<<<<< HEAD
            const qint64 binTime = static_cast<qint64>(x * m_durationMs / m_peaks.size());

            if (binTime < visibleStart || binTime > visibleEnd) continue;

            const float peak = m_peaks[x] * waveformAreaHeightMult;
            const int drawX = timeToScreenX(binTime);
            const int height = peak * h;
            const int y = centerY - height/2;
=======
            qint64 binTime = static_cast<qint64>(x * m_durationMs / m_peaks.size());

            if (binTime < visibleStart || binTime > visibleEnd) continue;

            float peak = m_peaks[x];
            int drawX = timeToScreenX(binTime);
            int y = centerY - (peak * centerY);
            int height = peak * (h - timeLabelHeight - timeLabelTopMargin - 20);
>>>>>>> de8e1a508d86f85302d557ebbb4302794635d9c0
            if (drawXPrev == 0) drawXPrev = drawX; // first time init
            painter.drawRect(drawXPrev, y, drawX-drawXPrev, height);
            drawXPrev = drawX;
        }
    }

    if (m_showSelection && m_selectionStartMs != m_selectionEndMs) {
        int startX = timeToScreenX(m_selectionStartMs);
        int endX = timeToScreenX(m_selectionEndMs);

        // Draw selection area highlight
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(100, 100, 255, 50));
        painter.drawRect(startX, timeLabelTopMargin + 20, endX - startX, h - timeLabelHeight - timeLabelTopMargin - 20);

        // Draw left handle
        painter.setPen(QPen(Qt::white, 2));
        painter.setBrush(QBrush(Qt::white));
        painter.drawRect(startX - 5, timeLabelTopMargin + 20, 10, h - timeLabelHeight - timeLabelTopMargin - 20);

        // Draw right handle
        painter.drawRect(endX - 5, timeLabelTopMargin + 20, 10, h - timeLabelHeight - timeLabelTopMargin - 20);

        // Draw selection duration at the top
        qint64 selectionDuration = m_selectionEndMs - m_selectionStartMs;
        QTime durationTime = QTime::fromMSecsSinceStartOfDay(selectionDuration);
        QString durationText = durationTime.toString("m:ss.zzz");

        int middleX = (startX + endX) / 2;
        painter.setPen(QPen(Qt::white, 1));
        QRect durationRect(middleX - 50, timeLabelTopMargin, 100, 20);
        painter.drawText(durationRect, Qt::AlignCenter, durationText);
    }

    // Draw position indicator
    if (m_durationMs > 0) {
        int posX = timeToScreenX(m_positionMs);
        if (posX >= scaleMargin && posX <= w) {
            painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(posX, timeLabelTopMargin + 20, posX, h - timeLabelHeight);

            int arrowSize = 8;
            int arrowY = centerY;
            QPolygon arrow;
            arrow << QPoint(posX, arrowY)
                  << QPoint(posX - arrowSize, arrowY - arrowSize)
                  << QPoint(posX - arrowSize, arrowY + arrowSize);
            painter.setPen(QPen(Qt::green, 1));
            painter.setBrush(QBrush(Qt::green));
            painter.drawPolygon(arrow);

            if (m_dragging) {
                painter.setPen(QPen(QColor(255, 100, 100, 150), 3));
                painter.drawLine(posX, timeLabelTopMargin + 20, posX, h - timeLabelHeight);
            }
        }
    }

    // Draw timeline background at the bottom
    painter.fillRect(scaleMargin, h - timeLabelHeight, w - scaleMargin, timeLabelHeight, QColor(50, 50, 80));
    painter.setPen(QPen(QColor(180, 180, 220), 1));
    painter.drawLine(scaleMargin, h - timeLabelHeight, w, h - timeLabelHeight);
}

void WaveformWidget::clearSelection()
{
    m_showSelection = false;
    update();
}

QPair<qint64, qint64> WaveformWidget::getSelection() const
{
    return qMakePair(m_selectionStartMs, m_selectionEndMs);
}

void WaveformWidget::setSelection(qint64 startMs, qint64 endMs)
{
    m_showSelection = true;
    m_selectionStartMs = qMax(0ll, qMin(m_durationMs, startMs));
    m_selectionEndMs = qMax(0ll, qMin(m_durationMs, endMs));
    update();
}

qint64 WaveformWidget::getCursorPosition() const {
    return cursorPosition;
}

void WaveformWidget::setCursorPosition(qint64 position) {
    cursorPosition = position;
    ensurePositionVisible(position);
    update();
}

void WaveformWidget::zoomIn()
{
    m_timeScale = qMax(m_minTimeScale, m_timeScale - 500);
    updateScrollBars();
    update();
}

void WaveformWidget::zoomOut()
{
    m_timeScale = qMin(m_maxTimeScale, m_timeScale + 500);
    updateScrollBars();
    update();
}

void WaveformWidget::zoomReset()
{
    if (m_durationMs > 0) {
        const int scaleMargin = 25;
        int visibleWidth = width() - scaleMargin;

        // Calculate time scale to fit entire track
        m_timeScale = static_cast<int>(m_durationMs * 100 / visibleWidth);
        m_timeScale = qMax(m_minTimeScale, qMin(m_maxTimeScale, m_timeScale));

        m_offsetMs = 0;
    } else {
        m_timeScale = 2000;
        m_offsetMs = 0;
    }

    updateScrollBars();
    update();
}

void WaveformWidget::zoomToSelection()
{
    if (m_showSelection && m_selectionStartMs != m_selectionEndMs) {
        qint64 selectionDuration = m_selectionEndMs - m_selectionStartMs;

        // Calculate zoom level to fit selection
        const int scaleMargin = 25;
        int visibleWidth = width() - scaleMargin;
        m_timeScale = qMax(m_minTimeScale,
                           qMin(m_maxTimeScale,
                                static_cast<int>(selectionDuration * 100 / visibleWidth)));

        // Center view on selection
        m_offsetMs = m_selectionStartMs - (selectionDuration / 4);
        m_offsetMs = qMax(0ll, qMin(m_durationMs - static_cast<qint64>(m_timeScale * (visibleWidth / 100.0)), m_offsetMs));

        updateScrollBars();
        update();
    }
}
