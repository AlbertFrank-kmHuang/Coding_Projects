#include "Cell.h"
#include <QPainter>
#include <QPen>
#include <QBrush>

Cell::Cell(int row, int col, QWidget *parent)
    : QPushButton(parent)
    , cellRow(row)
    , cellCol(col){
    setFixedSize(40, 40);
}

void Cell::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if( !opened ){
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(200, 200, 200)));
        painter.drawRect(rect());

        painter.setPen(QPen(Qt::white, 2));
        painter.drawLine(0, 0, width(), 0);
        painter.drawLine(0, 0, 0, height());
        painter.setPen(QPen(Qt::darkGray, 2));
        painter.drawLine(width(), 0, width(), height());
        painter.drawLine(0, height(), width(), height());

        // painter.drawPixmap(rect(), QPixmap(":/images/cover.png"));
    }else{
        painter.setPen(QPen(Qt::darkGray, 1));
        painter.setBrush(QBrush(QColor(230, 230, 230))); // 更浅的灰色
        painter.drawRect(0, 0, width() - 1, height() - 1);

        if (mine) {
            // 如果是雷，画一个红色的圆圈代表雷
            painter.setBrush(QBrush(Qt::red));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect().adjusted(4, 4, -4, -4));

            // 📌 后期放雷的图片：
            // painter.drawPixmap(rect(), QPixmap(":/images/mine.png"));
        }else if(minesAround > 0) {
            painter.setPen(Qt::blue); // 现阶段统一用蓝色，后期可以根据数字变色
            QFont font = painter.font();
            font.bold();
            painter.setFont(font);
            // 将数字居中绘制在格子上
            painter.drawText(rect(), Qt::AlignCenter, QString::number(minesAround));
        }
    }
}
