#ifndef LYRICWIDGET_H
#define LYRICWIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QTimer>

class LyricWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LyricWidget(QWidget *parent = nullptr);
    ~LyricWidget();
    void setText(const QString& text, int64_t pmills = -1);
    void setSpeed(const double speed);
    void setColor(const QString& color);
    void setOutlineColor(const QString& color);
    void setOutlineWidth(const int width);
    void tick();
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    QFont getFont() const;
    void setFont(const QFont &newFont);
    void enableAutoTick(int64_t period);
    void pauseAutoTick(bool flag);

private:
    void drawText(QPainter& painter, const QRect& rect, const QFont& font, const QString& text, const QColor& fontColor, const QColor& outlineColor = QColor(Qt::transparent));

private slots:
    void doTick();

signals:
    void needRepaint();

private:
    double speed;
    QString originText;
    QFont font;
    QColor color;
    QColor outlineColor = QColor(Qt::transparent);
    int outlineWidth = 0;

    double offset = 0;
    /**
     * 是否需要滚动
     */
    bool needScroll;
    /**
     * 滚动方向
     */
    bool front = true;
    int maxWidth;
    QTimer* autoTickTimer = nullptr;
    int64_t autoTickPeriod;
};

#endif // LYRICWIDGET_H
