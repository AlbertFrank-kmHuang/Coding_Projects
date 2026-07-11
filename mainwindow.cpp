#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Cell.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <algorithm>
#include <random>
#include <queue>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , startButton(new QPushButton("New Game", this))
{
    ui->setupUi(this);
    startButton->setFixedSize(90, 30);

    onStartButtonClicked();

    connect(startButton, &QPushButton::clicked
            , this, &MainWindow::onStartButtonClicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartButtonClicked() {
    if (central) {
        startButton->setParent(this);
        delete central;
        central = nullptr;
    }

    firstClick = true;

    central = new QWidget(this);
    QVBoxLayout *mainVerticalLayout = new QVBoxLayout(central);
    mainVerticalLayout->setContentsMargins(10, 5, 10, 5); //左上右下margin

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(startButton);
    buttonsLayout->setAlignment(Qt::AlignHCenter);
    mainVerticalLayout->addLayout(buttonsLayout);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setVerticalSpacing(0);

    for (int i=0; i < gridHeight; ++i) {
        for (int j=0; j < gridLength; ++j) {
            Cell* cell = new Cell(i, j, this);
            connect(cell, &Cell::clicked, this, &MainWindow::onCellClicked);
            gridLayout->addWidget(cell, i, j);
        }
    }

    mainVerticalLayout->addLayout(gridLayout);
    setCentralWidget(central);
    central->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->adjustSize();
}


void MainWindow::onCellClicked() {
    Cell *clickedCell = qobject_cast<Cell*>(sender());
    if (!clickedCell) return;

    //首次点击必为0
    if (firstClick) {
        firstClick = false;

        int startRow = clickedCell->getRow();
        int startCol = clickedCell->getCol();

        std::vector<std::pair<int, int>> safeZone;
        for (int i = std::max(0, startRow-1);
             i <= std::min(gridHeight-1, startRow+1);
             ++i) {

            for (int j=std::max(0, startCol-1);
                 j <= std::min(gridLength-1, startCol+1);
                 ++j) {
                safeZone.push_back({i, j});
            }
        }

        int safeZoneSize = 1 + safeZone.size();
        std::vector<bool> allMines(gridHeight*gridLength - safeZoneSize
                                   , false);

        for (int i=0; i < mineNumber; ++i) {
            allMines[i] = true;
        }

        std::random_device rd;
        std::mt19937 randomGenerator(rd());
        std::shuffle (allMines.begin(), allMines.end(), randomGenerator);

        int index = 0;
        for (int i=0; i<gridHeight; ++i) {
            for (int j=0; j<gridLength; ++j) {
                bool isSafe = false;
                for (auto safeCell : safeZone) {
                    if (i == safeCell.first && j == safeCell.second) {
                        isSafe = true;
                        break;
                    }
                }
                Cell* currentCell = qobject_cast<Cell*>
                    (gridLayout->itemAtPosition(i, j)->widget());
                if (!isSafe) {
                    currentCell->setMine(allMines[index]);
                    index++;
                }else{
                    currentCell->setMine(false);
                }
            }
        }

        for (int i=0; i<gridHeight; ++i) {
            for (int j=0; j<gridLength; ++j) {
                Cell* currentCell = qobject_cast<Cell*>
                    (gridLayout->itemAtPosition(i, j)->widget());

                int countArounds = 0;
                for (int m = std::max(0, i-1)
                     ; m <= std::min(gridHeight-1, i+1)
                     ;++m) {
                    for (int n = std::max(0, j-1)
                         ;n <= std::min(gridLength-1, j+1)
                         ; ++n) {
                        if (i == m && j == n) continue;
                        Cell* aroundCell = qobject_cast<Cell*>
                            (gridLayout->itemAtPosition(m, n)->widget());
                        if (aroundCell->isMine()) {
                            countArounds++;
                        }
                    }
                }

                currentCell->setMinesAround(countArounds);
            }
        }
    }



    if (clickedCell) {
        if (clickedCell->isOpened()) return ;
        clickedCell->setOpened(true);

        if (clickedCell->countMinesAround() == 0
            && !clickedCell->isMine()) {
            expand(clickedCell->getRow(), clickedCell->getCol());
        }
    }

}

void MainWindow::expand(int startRow, int startCol) {
    if (!gridLayout) return;

    std::queue<std::pair<int, int>> adjoins;
    adjoins.push({startRow, startCol});

    while(!adjoins.empty()){
        auto [curRow, curCol] = adjoins.front();
        adjoins.pop();

        Cell* currentCell = qobject_cast<Cell*>
            (gridLayout->itemAtPosition(curRow, curCol)->widget());

        currentCell->setFlagged_expanded(true);

        for (int i = std::max(0, curRow-1);
             i <= std::min(gridHeight-1, curRow+1);
             ++i){

            for (int j = std::max(0, curCol-1);
                 j <= std::min(gridLength-1, curCol+1);
                 ++j) {

                if (i == curRow && j == curCol) continue;

                Cell* newCell = qobject_cast<Cell*>
                    (gridLayout->itemAtPosition(i, j)->widget());
                newCell->setOpened(true);
                if (newCell->countMinesAround() == 0
                    && !newCell->isFlagged_expanded()
                    && !newCell->isMine()) {
                    adjoins.push({newCell->getRow(), newCell->getCol()});
                }
            }
        }
    }
}















