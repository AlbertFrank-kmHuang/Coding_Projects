#ifndef CELL_H
#define CELL_H

#include <QPushButton>
#include <QPaintEvent>

class Cell:public QPushButton{
    Q_OBJECT
public:
    Cell(int row, int col, QWidget *parent = nullptr);

    //position
    int getRow() const { return cellRow; }
    int getCol() const { return cellCol; }

    //mine
    bool isMine() const { return mine; }
    void setMine(bool mineBool) { mine = mineBool; }

    //opened
    bool isOpened() const { return opened; }
    void setOpened(bool openedBool) {
        opened = openedBool;
        update();
    }

    //minesAround
    int countMinesAround() const { return minesAround; }
    void setMinesAround(int count) { minesAround = count; }

    //flagged
    bool isFlagged() {
        return flagged;
    }
    void setFlagged(bool whether) {
        flagged = whether;
    }

    //flagged_expanded
    bool isFlagged_expanded() {
        return flagged_expanded;
    }
    void setFlagged_expanded(bool whether) {
        flagged_expanded = whether;
    }

private:
    int cellRow;
    int cellCol;
    bool mine = false;
    bool opened = false;
    int minesAround;
    bool flagged;
    bool flagged_expanded;

protected:
    void paintEvent(QPaintEvent *event) override;

};

#endif // CELL_H
