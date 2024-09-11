#include "lyricwidget.h"
#include <QPainter>
#include <QFontMetrics>
#include "debug.h"
#include <QThread>

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

    int spaceWidth = fm.boundingRect("-").width();
    DEBUG("Space Char Width: " << spaceWidth);

    int maxLength = 0;
    for (auto begin = lines.begin(),
              end   = lines.end(); begin != end; ++begin)
    {
        *begin = QString(" ").append(*begin).append("   ");
        QRect rect = fm.boundingRect(*begin);
        maxLength = std::max(maxLength, rect.width());
    }
    DEBUG("Max Line Width: " << maxLength);

    QStringList newLines;
    for (auto begin = lines.cbegin(),
              end   = lines.cend(); begin != end; ++begin)
    {
        QRect rect = fm.boundingRect(*begin);
        int lineWidth = rect.width();
        int appendWidth = (width() - lineWidth) / spaceWidth;
        DEBUG(*begin << QString("Width: ") << lineWidth);
        // newLines << QString(*begin);
        newLines << QString(" ").repeated(appendWidth / 2).append(*begin).append(QString(" ").repeated(appendWidth / 2 + (appendWidth & 0x1)));
    }

    maxWidth = maxLength;
    needScroll = maxWidth > width();
    originText = newLines.isEmpty() ? "" : newLines.join("\n");
    speed = maxWidth / width() * 16;
    offset = -speed;

    if (pmills != -1 && this->autoTickTimer)
    {
        speed = (maxWidth - width() - offset) / (pmills / 1000);
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
    offset = offset + (front ? speed : -speed) * (this->autoTickTimer->interval() + 2);
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

    if (needScroll)
    {
        painter.drawText(QRect(-offset, 0, width() + offset, height()), Qt::AlignmentFlag::AlignVCenter | Qt::AlignLeft, originText);
        return;
    }

    QFontMetrics fm(font);
    QStringList lines = originText.split("\n");
    int lineHeight = height() / lines.size() * 0.8;
    int idx = 0;
    int currentOffset = offset, currentWidth = width();
    for (auto begin = lines.cbegin(), end = lines.cend(); begin != end; ++begin)
    {
        int w = fm.boundingRect(*begin).width();
        int x = (currentWidth - w) >> 1;
        int y = idx * lineHeight;
        QRect rect = QRect(0, y, currentWidth, y + lineHeight);
        // DEBUG(rect << *begin);
        painter.drawText(rect, Qt::AlignmentFlag::AlignCenter, *begin);
        ++idx;
    }
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
