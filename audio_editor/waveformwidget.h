#ifndef WAVEFORMWIDGET_H
#define WAVEFORMWIDGET_H

#include <QWidget>
#include <QVector>
#include <QScrollBar>

class WaveformWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WaveformWidget(QWidget *parent = nullptr);

    enum Handle { NoHandle, StartHandle, EndHandle };

    void setAudioData(const QVector<float> &data);
    void setDuration(qint64 durationMs);
    void setPosition(qint64 positionMs);

    void clearSelection();
    QPair<qint64, qint64> getSelection() const;
    void setSelection(qint64 startMs, qint64 endMs);
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void zoomToSelection();

    qint64 getCursorPosition() const;
    void setCursorPosition(qint64 position);

signals:
    void positionChanged(qint64 position);
    void userPositionChanged(qint64 position);
    void selectionChanged(qint64 startMs, qint64 endMs);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;
    void paintTimeMarkers(QPainter &painter, int w, int h, const QColor& scaleColor);

    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void calculatePeaks();
    void updateScrollBars();
    int timeToScreenX(qint64 timeMs) const;
    qint64 screenXToTime(int x) const;
    void ensurePositionVisible(qint64 positionMs);
    int getOptimalTimeInterval() const;

    QVector<float> m_audioData;
    qint64 m_durationMs = 0;
    qint64 m_positionMs = 0;

    float peakMax;
    QVector<float> m_peaks;


    bool m_dragging = false;
    qint64 m_lastUserPosition = -1;
    qint64 m_selectionStartMs = 0;
    qint64 m_selectionEndMs = 0;
    Handle m_draggingHandle = NoHandle;
    bool m_showSelection = false;
    qint64 cursorPosition;
    QScrollBar* m_horizontalScrollBar;
    int m_timeScale;
    qint64 m_offsetMs;
    int m_minTimeScale;
    int m_maxTimeScale;
    bool m_isRecording = false;
};

#endif // WAVEFORMWIDGET_H
