#include "lyricwidget.h"
#include <QPainter>
#include <QFontMetrics>
#include "debug.h"

LyricWidget::LyricWidget(QWidget *parent)
    : QWidget(parent)
{

}

LyricWidget::~LyricWidget()
{

}

void LyricWidget::setText(const QString &text)
{
    this->originText = QString(text);
    this->offset = 0;
    front = true;

    QFontMetrics fm(font);
    auto lines = originText.split("\n");
    QString newLine = "";
    maxWidth = 0;
    for (auto begin = lines.cbegin(),
         end   = lines.cend(); begin != end; ++begin)
    {
        QRect rect = fm.boundingRect(*begin);
        maxWidth = std::max(maxWidth, rect.width());
        int spaceWidth = fm.boundingRect("-").width();
        if (rect.width() < width())
        {
            int sub = (width() - rect.width()) / spaceWidth;
            int right = sub >> 1;
            int left = right + (sub & 0x1);
            newLine += "\n" + QString(" ").repeated(left) + *begin + QString(" ").repeated(right);
        } else {
            newLine += "\n" + *begin;
        }
    }
    needScroll = maxWidth > width();
    originText = newLine.length() == 0 ? "" : newLine.removeFirst();
    speed = maxWidth / width() * 8;
}

void LyricWidget::setSpeed(const double speed)
{
    this->speed = speed;
}

void LyricWidget::setColor(const QString &color)
{
    this->color = QColor(color);
}

void LyricWidget::tick()
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
    offset = offset + (front ? speed : -speed);
}

void LyricWidget::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setFont(font);
    painter.setPen(color);
    // painter.drawRect(0, 0, width(), height());
    painter.drawText(QRect(-offset, 0, width() + offset, height()), Qt::AlignmentFlag::AlignVCenter | Qt::AlignLeft, originText);
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
