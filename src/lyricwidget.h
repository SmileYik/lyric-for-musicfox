#ifndef LYRICWIDGET_H
#define LYRICWIDGET_H

#include <QWidget>
#include <QResizeEvent>

class LyricWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LyricWidget(QWidget *parent = nullptr);
    ~LyricWidget();
    void setText(const QString& text);
    void setSpeed(const double speed);
    void setColor(const QString& color);
    void tick();
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    QFont getFont() const;
    void setFont(const QFont &newFont);

private:
    double speed;
    QString originText;
    QFont font;
    QColor color;
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
};

#endif // LYRICWIDGET_H
