#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Cell.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <algorithm>
#include <random>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , startButton(new QPushButton("New Game", this))
{
    ui->setupUi(this);
    startButton->setFixedSize(90, 30);

    generateGrid();

    connect(startButton, &QPushButton::clicked
            , this, &MainWindow::generateGrid);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateGrid(){
    if (central) {
        startButton->setParent(this);
        delete central;
        central = nullptr;
    }

    central = new QWidget(this);
    QVBoxLayout *mainVerticalLayout = new QVBoxLayout(central);
    mainVerticalLayout->setContentsMargins(10, 5, 10, 5); //左上右下margin

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(startButton);
    buttonsLayout->setAlignment(Qt::AlignHCenter);
    mainVerticalLayout->addLayout(buttonsLayout);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setVerticalSpacing(0);

    //set mines
    std::vector<bool> mines (gridHeight * gridLength, false);
    for (int i=0; i < mineNumber; ++i) {
        mines[i] = true;
    }
    std::random_device rd;
    std::mt19937 randomGenerator(rd());
    std::shuffle(mines.begin(), mines.end(), randomGenerator);

    for (int i=0; i < gridHeight; ++i) {
        for (int j=0; j < gridLength; ++j) {
            Cell *cell = new Cell(i, j, this);

            bool whetherMine = mines[i * gridLength + j];
            cell->setMine(whetherMine);

            connect(cell, &Cell::clicked, this, &MainWindow::onCellClicked);
            gridLayout->addWidget(cell, i, j);
        }
    }

    for (int i=0; i < gridHeight; ++i) {
        for (int j=0; j < gridLength; ++j) {
            int countAround = 0;
            for (int m=std::max(0, i-1); m <= std::min(gridHeight-1, i+1); ++m) {
                for (int n=std::max(0, j-1); n <= std::min(gridLength-1, j+1); ++n) {
                    if (i == m && j == n) continue;
                    if (qobject_cast<Cell*>(gridLayout->itemAtPosition(m, n)->widget())->isMine()) {
                        countAround++;
                    }
                }
                qobject_cast<Cell*>(gridLayout->itemAtPosition(i, j)->widget())->setMinesAround(countAround);
            }
        }
    }

    mainVerticalLayout->addLayout(gridLayout);

    mainVerticalLayout->addStretch();

    setCentralWidget(central);
}

void MainWindow::onCellClicked() {
    Cell *clickedCell = qobject_cast<Cell*>(sender());

    if (clickedCell) {
        if (clickedCell->isOpened()) return ;
        clickedCell->setOpened(true);
    }

}

















