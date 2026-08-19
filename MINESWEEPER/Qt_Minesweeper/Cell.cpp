#include "Cell.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QTimer>

Cell::Cell(int row, int col, QWidget *parent)
    : QWidget(parent)
    , cellRow(row)
    , cellCol(col){
    setFixedSize(40, 40);
}

void Cell::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    if( !opened ){
        painter.setPen(Qt::NoPen);
        if (!isFlagged()) {
            painter.setBrush(QBrush(QColor(200, 200, 200)));
            painter.drawRect(rect().adjusted(0, 0, -1, -1));

            painter.setPen(QPen(Qt::white, 2));
            painter.drawLine(0, 0, width()-1, 0);
            painter.drawLine(0, 0, 0, height()-1);

            painter.setPen(QPen(Qt::darkGray, 2));
            painter.drawLine(width()-1, 0, width()-1, height()-1);
            painter.drawLine(0, height()-1, width()-1, height()-1);

        } else {//flagged
            if (!wrongFlagged) {
                painter.setBrush(QBrush(QColor(200,150,150)));
                painter.drawRect(rect().adjusted(0, 0, -1, -1));

                painter.setPen(QPen(QColor(250, 200, 200), 2));
                painter.drawLine(0, 0, width()-1, 0);
                painter.drawLine(0, 0, 0, height()-1);

                painter.setPen(QPen(QColor(150, 100, 100), 2));
                painter.drawLine(width()-1, 0, width()-1, height()-1);
                painter.drawLine(0, height()-1, width()-1, height()-1);
            } else {//wrongFlagged

            }
        }

        // painter.drawPixmap(rect(), QPixmap(":/images/cover.png"));
    }else{//opened
        painter.setPen(QPen(Qt::darkGray, 1));
        painter.setBrush(QBrush(QColor(230, 230, 230))); // 更浅的灰色
        painter.drawRect(0, 0, width() - 1, height() - 1);

        if (mine) {
            // 如果是雷，画一个红色的圆圈代表雷
            painter.setBrush(QBrush(QColor(20, 20, 20)));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(rect().adjusted(4, 4, -4, -4));

            // 后期放雷的图片：
            // painter.drawPixmap(rect(), QPixmap(":/images/mine.png"));
        }else if(minesAround > 0) {

            QColor color;
            switch (minesAround) {
            case 1:
                color = QColor(64, 64, 255, 255);
                break;
            case 2:
                color = QColor(64, 180, 64, 255);
                break;
            case 3:
                color = QColor(180, 64, 64, 255);
                break;
            case 4:
                color = QColor(64, 0, 128, 255);
                break;
            case 5:
                color = QColor(135, 85, 0, 255);
                break;
            case 6:
                color = QColor(0, 128, 128, 255);
                break;
            case 7:
                color = QColor(32, 32, 32, 255);
                break;
            case 8:
                color = QColor(255, 0, 0, 255);
                break;
            default:
                color = QColor(0, 0, 0, 255);
                break;
            }

            painter.setPen(color); // 现阶段统一用蓝色，后期可以根据数字变色
            QFont font = painter.font();
            font.setBold(true);
            painter.setFont(font);
            // 将数字居中绘制在格子上
            painter.drawText(rect().adjusted(0, 0, -1, -1)
                             , Qt::AlignCenter, QString::number(minesAround));
        }
    }
}

void Cell::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit leftClicked();
    }
    else if (event->button() == Qt::RightButton) {
        emit rightClicked();
    }
}

