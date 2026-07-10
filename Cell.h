#ifndef CELL_H
#define CELL_H

#include <QPushButton>
#include <QPaintEvent>

class Cell:public QPushButton{
    Q_OBJECT
public:
    Cell(int row, int col, QWidget *parent = nullptr);
    int getRow() const { return cellRow; }
    int getCol() const { return cellCol; }

    bool isMine() const { return mine; }
    void setMine(bool mineBool) { mine = mineBool; }

    bool isOpened() const { return opened; }
    void setOpened(bool openedBool) {
        opened = openedBool;
        update();
    }

    int countMinesAround() const { return minesAround; }
    void setMinesAround(int count) { minesAround = count; }

private:
    int cellRow;
    int cellCol;
    bool mine = false;
    bool opened = false;
    int minesAround;
protected:
    void paintEvent(QPaintEvent *event) override;

};

#endif // CELL_H
