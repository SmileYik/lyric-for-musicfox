#include "lyricwidget.h"
#include <QPainter>
#include <QFontMetrics>
#include "debug.h"
#include <QThread>
#include <QPainterPath>

LyricWidget::LyricWidget(QWidget *parent)
    : QWidget(parent)
{

}

LyricWidget::~LyricWidget()
{
    if (this->autoTickTimer)
    {
        disconnect(this->autoTickTimer);
        this->autoTickTimer->stop();
        this->autoTickTimer->deleteLater();
    }
}

void LyricWidget::setText(const QString &text, int64_t pmills)
{
    this->originText = QString(text);
    this->offset = 0;
    front = true;

    // clean space
    QString tempText = "";
    int textlen = text.length();
    for (int i = 0; i < textlen; ++i)
    {
        if (i == 0)
        {
            tempText.append(text[i]);
            continue;
        }
        if (text[i] == text[i - 1] && text[i] == ' ') continue;
        tempText.append(text[i]);
    }
    if (!tempText.isEmpty())
    {
        if (tempText[0] == ' ') tempText = tempText.remove(0, 1);
        if (tempText[tempText.length() - 1] == ' ') tempText = tempText.remove(tempText.length() - 1, 1);
    }
    DEBUG("Set Lyric Text: " << tempText);

    // 对齐文本
    QFontMetrics fm(font);
    QStringList lines = tempText.split("\n");
    int maxLength = 0;
    for (auto begin = lines.begin(),
              end   = lines.end(); begin != end; ++begin)
    {
        maxLength = std::max(maxLength, fm.horizontalAdvance(*begin));
    }
    DEBUG("Max Line Width: " << maxLength);

    maxWidth = maxLength;
    needScroll = maxWidth > width();
    originText = lines.isEmpty() ? "" : lines.join("\n");
    speed = maxWidth * 1.0 / width() * 16;
    offset = -speed;

    if (pmills != -1 && this->autoTickTimer)
    {
        speed = (maxWidth - width() - offset) / (pmills / 1000.0);
        offset = -speed;
    }

    DEBUG("Width" << width() << "Need Scroll: " << needScroll);
    DEBUG(originText);
    update();
}

void LyricWidget::setSpeed(const double speed)
{
    this->speed = speed;
}

void LyricWidget::setColor(const QString &color)
{
    this->color = QColor(color);
}

void LyricWidget::setOutlineColor(const QString& color)
{
    this->outlineColor = QColor(color);
}

void LyricWidget::setOutlineWidth(const int width)
{
    this->outlineWidth = width;
}

void LyricWidget::enableAutoTick(int64_t period)
{
    if (this->autoTickTimer != nullptr) return;
    this->autoTickTimer = new QTimer(this);
    this->autoTickPeriod = period;

    connect(this->autoTickTimer, &QTimer::timeout, this, &LyricWidget::doTick);

    this->autoTickTimer->setInterval(period);
    this->autoTickTimer->start();
}

void LyricWidget::pauseAutoTick(bool flag)
{
    if (this->autoTickTimer == nullptr) return;
    if (flag)
    {
        this->autoTickTimer->stop();
    }
    else
    {
        this->autoTickTimer->start();
    }
}

void LyricWidget::doTick()
{
    update();
    if (!needScroll)
    {
        return;
    }
    if (front)
    {
        if (maxWidth - offset < width())
        {
            front = false;
        }
    } else {
        if (offset <= 0)
        {
            front = true;
        }
    }
    offset = offset + (front ? speed : -speed) * (this->autoTickTimer ? (this->autoTickTimer->interval() + 2) : 1);
}

void LyricWidget::tick()
{
    if (nullptr != autoTickTimer) return;
    doTick();
}

void LyricWidget::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setFont(font);

#   ifdef DEBUG_FLAG
    painter.setPen(QColor::fromRgb(0, 0, 0, 255));
    painter.drawRect(0, 0, width(), height());
    painter.setPen(QColor::fromRgb(255, 255, 255, 255));
    painter.drawRect(-offset, 0, width() + offset, height());
#   endif
    painter.setPen(color);

    QFontMetrics fm(font);
    QStringList lines = originText.split("\n");
    int lineRawHeight = height() / lines.size();
    int lineHeight = lineRawHeight * 0.8;
    int idx = 0;
    int currentOffset = offset, currentWidth = width();
    for (auto begin = lines.cbegin(), end = lines.cend(); begin != end; ++begin)
    {
        int w = fm.boundingRect(*begin).width();
        int x = (currentWidth - w) >> 1;
        int y = idx * lineRawHeight;

        painter.save();
        if (needScroll)
        {
            QRect rect = QRect(-offset, y, width() + offset, lineHeight);
            drawText(painter, rect, font, *begin, color, outlineColor);
        } else
        {
            QRect rect = QRect(0, y, currentWidth, lineHeight);
            drawText(painter, rect, font, *begin, color, outlineColor);
        }

        painter.restore();
        ++idx;
    }
}

void LyricWidget::drawText(QPainter& painter, const QRect& rect, const QFont& font, const QString& text, const QColor& fontColor, const QColor& outlineColor)
{
    QFontMetrics fm(font);
    QRect textRect = fm.boundingRect(text);
    QString str = text;
    int strLen = fm.horizontalAdvance(str);
    int startX = rect.x() >= 0 ? rect.x() : 0;
    QRect absRect(startX, rect.y(), rect.right() - startX, rect.height());
    QTransform offset;
    // 宽度布满矩形情况
    if (absRect.width() < strLen)
    {
        int len = fm.horizontalAdvance(str);
        offset.translate(rect.x(), 0);
    }
    // 宽度没有布满，居中放置
    else {
        absRect.setX(absRect.x() + ((absRect.width() - strLen) >> 1));
    }

    QPainterPath path;
    path.addText(absRect.x(), absRect.bottom(), font, str);

    QPen pen;
    pen.setWidth(outlineWidth);
    pen.setColor(outlineColor);
    pen.setStyle(Qt::PenStyle::SolidLine);

    painter.setTransform(offset);
    painter.setRenderHint(QPainter::RenderHint::Antialiasing);
    painter.setRenderHint(QPainter::RenderHint::TextAntialiasing);
    painter.setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);
    painter.strokePath(path, pen);
    painter.fillPath(path, QBrush(fontColor));

}

void LyricWidget::resizeEvent(QResizeEvent *event)
{
    int size = (event->size().height() / 2.5);
    font.setPixelSize(size);
}

QFont LyricWidget::getFont() const
{
    return font;
}

void LyricWidget::setFont(const QFont &newFont)
{
    font = newFont;
}
