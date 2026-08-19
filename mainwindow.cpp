#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Cell.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <algorithm>
#include <random>
#include <queue>
#include <QString>
#include <QMenu>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , startButton(new QPushButton("New Game", this))
    , modeButton(new QPushButton("Change Mode", this))
{
    ui->setupUi(this);
    startButton->setFixedSize(120, 30);
    modeButton->setFixedSize(120, 30);

    onStartButtonClicked();

    connect(startButton, &QPushButton::clicked
            , this, &MainWindow::onStartButtonClicked);

    connect(modeButton, &QPushButton::clicked
            , this, &MainWindow::onModeButtonClicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onStartButtonClicked() {

    QString modeName = "Change Mode";
    switch (gamemode) {
    case 0:
        modeName = "Sudden Death";
        break;
    case 1:
        modeName = "Counter Strike";
        break;
    case 2:
        modeName = "Explosion Proof";
        break;
    case 3:
        modeName = "Chain Reaction";
        break;
    default:
        modeName = "Change Mode";
        break;
    }
    modeButton->setText(modeName);

    startButton->setText("New Game");
    gameover = false;
    gamewin = false;
    health = 100;
    minesRemain = mineNumber;
    flaggedCellNumber = 0;

    if (central) {
        startButton->setParent(this);
        modeButton->setParent(this);
        delete central;
        central = nullptr;
    }

    firstClick = true;

    central = new QWidget(this);
    QVBoxLayout *mainVerticalLayout = new QVBoxLayout(central);
    mainVerticalLayout->setContentsMargins(10, 5, 10, 5); //左上右下margin

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(modeButton);
    buttonsLayout->addWidget(startButton);
    buttonsLayout->setAlignment(Qt::AlignHCenter);
    mainVerticalLayout->addLayout(buttonsLayout);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(0);
    gridLayout->setVerticalSpacing(0);

    for (int i=0; i < gridHeight; ++i) {
        for (int j=0; j < gridLength; ++j) {
            Cell* cell = new Cell(i, j, this);
            connect(cell, &Cell::leftClicked, this, &MainWindow::onCellLeftClicked);
            connect(cell, &Cell::rightClicked, this, &MainWindow::onCellRightClicked);
            gridLayout->addWidget(cell, i, j);
        }
    }

    mainVerticalLayout->addLayout(gridLayout);
    setCentralWidget(central);
    central->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->adjustSize();
}

void MainWindow::onModeButtonClicked() {
    QMenu modeMenu(this);

    QAction *mode0 = modeMenu.addAction("Sudden Death");
    QAction *mode1 = modeMenu.addAction("Counter Strike");
    QAction *mode2 = modeMenu.addAction("Explosion Proof");
    QAction *mode3 = modeMenu.addAction("Chain Reaction");

    QAction *selected =
        modeMenu.exec(modeButton->mapToGlobal(QPoint(0, modeButton->height())));

    int oldMode = gamemode;

    if (selected == mode0)
        gamemode = 0;
    else if (selected == mode1)
        gamemode = 1;
    else if (selected == mode2)
        gamemode = 2;
    else if (selected == mode3)
        gamemode = 3;

    if (oldMode != gamemode)
        onStartButtonClicked();
}


void MainWindow::onCellLeftClicked() {
    if (gameover || gamewin) return;
    Cell *clickedCell = qobject_cast<Cell*>(sender());
    int clickedRow = clickedCell->getRow();
    int clickedCol = clickedCell->getCol();

    if (!clickedCell) return;

    //首次点击必为0，然后生成棋局
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
    //首次点击功能结束


    //点击已开格子快速扩张
    if (clickedCell->isOpened() && !clickedCell->isMine()){
        int countAround = 0;
        for (int i = std::max(0, clickedRow-1);
             i <= std::min(gridHeight-1, clickedRow+1)
             ; ++i) {
            for (int j=std::max(0, clickedCol-1)
                 ;j<=std::min(gridLength-1, clickedCol+1)
                 ; ++j) {
                if (i == clickedRow && j == clickedCol) continue;
                Cell* newCell = qobject_cast<Cell*>
                    (gridLayout->itemAtPosition(i, j)->widget());
                if (newCell->isFlagged()
                    || (newCell->isMine() && newCell->isOpened())) countAround++;
            }
        }

        if (countAround == clickedCell->countMinesAround()) {
            for (int i = std::max(0, clickedRow-1);
                 i <= std::min(gridHeight-1, clickedRow+1)
                 ; ++i) {
                for (int j=std::max(0, clickedCol-1)
                     ;j<=std::min(gridLength-1, clickedCol+1)
                     ; ++j) {
                    if (i == clickedRow && j == clickedCol) continue;
                    Cell* newCell = qobject_cast<Cell*>
                        (gridLayout->itemAtPosition(i, j)->widget());

                    if(!newCell->isFlagged()) {
                        newCell->setOpened(true);
                        if (newCell->isMine()) {
                            touchMineFunc(newCell->getRow(), newCell->getCol());
                        }
                    }

                    if(newCell->countMinesAround() == 0
                        && !newCell->isFlagged_expanded())
                        expand(i, j);
                }
            }
        }
    }

    //点击未开格子
    else if(!clickedCell->isOpened()){
        if (clickedCell->isFlagged()) {
            clickedCell->setFlagged(false);
        }else{
            clickedCell->setOpened(true);

            if (clickedCell->countMinesAround() == 0
                && !clickedCell->isMine()) {
                expand(clickedRow, clickedCol);
            }
            else if(clickedCell->isMine()) {
                touchMineFunc(clickedRow, clickedCol);
            }
        }
    }

    minesRemain = mineNumber - flaggedCellNumber - minesTouched;

    if (gameover) {
        gameoverFunc();
        return;
    }


    //如果没有gameover，进行gamewin检测
    gamewinTest();
    if (gamewin) gamewinFunc();

}


void MainWindow::onCellRightClicked() {
    if (gameover || gamewin) return;
    Cell* clickedCell = qobject_cast<Cell*>(sender());
    if (!clickedCell) return;
    if (clickedCell->isOpened()) return;
    //如果已经被插旗，则拔掉
    if (clickedCell->isFlagged()) {
        clickedCell->setFlagged(false);
        flaggedCellNumber--;
    } else if(minesRemain>=0){//否则在还有剩余雷数的情况下插上
        clickedCell->setFlagged(true);
        flaggedCellNumber++;
    }

    minesRemain = mineNumber - flaggedCellNumber - minesTouched;
}


void MainWindow::touchMineFunc(int row, int col){
    if (gamemode == 2){
        health -= 20;
        minesTouched++;
        if (health <= 0) {
            gameover = true;
        }
    }
    else if (gamemode == 0 || gamemode == 1) {
        minesTouched++;
        gameover = true;
    }
    else if (gamemode == 3) {
        expandExplode(row, col);
    }
    Cell* curCell = qobject_cast<Cell*>
        (gridLayout->itemAtPosition(row, col)->widget());

    curCell->setMineTouched();

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

void MainWindow::expandExplode(int Row, int Col) {
    if (!gridLayout) return ;

    std::queue<std::pair<int, int>> adjoins;
    adjoins.push({Row, Col});

    while(!adjoins.empty()) {
        auto [startRow, startCol] = adjoins.front();
        adjoins.pop();
        Cell* startCell = qobject_cast<Cell*>
            (gridLayout->itemAtPosition(startRow, startCol)->widget());
        startCell->setFlagged_expanded(true);

        for (int i = std::max(0, startRow-1)
             ;i <= std::min(gridHeight-1, startRow+1)
             ; ++i) {
            for (int j = std::max(0, startCol-1)
                 ; j <= std::min(gridLength-1, startCol+1)
                 ; ++j) {
                if (i == startRow && j == startCol) continue;

                Cell* newCell = qobject_cast<Cell*>
                    (gridLayout->itemAtPosition(i, j)->widget());
                newCell->setOpened(true);

                if (newCell->isMine()
                    && !newCell->isFlagged_expanded()) {
                    adjoins.push({i, j});
                }
            }
        }


    }
}

void MainWindow::gameoverFunc() {
    startButton->setText("Game Over!");
    for (int i=0; i<gridHeight; ++i) {
        for (int j=0; j<gridLength; ++j) {
            Cell* cell = qobject_cast<Cell*>
                (gridLayout->itemAtPosition(i, j)->widget());
            if (cell->isMine() && !cell->isOpened()) {
                cell->setOpened(true);
            }
            else if(cell->countMinesAround() == 8) {
                cell->setOpened(true);
            }
        }
    }
}

void MainWindow::gamewinFunc() {
    startButton->setText("You Win!!!");
    for (int i=0; i<gridHeight; ++i) {
        for (int j=0; j<gridLength; ++j) {
            Cell* curCell = qobject_cast<Cell*>
                (gridLayout->itemAtPosition(i, j)->widget());

            if (!curCell->isOpened()) {
                curCell->setFlagged(true);
            }
        }
    }
}

void MainWindow::gamewinTest() {
    int allPossibleMines = 0;
    for (int i=0; i<gridHeight; ++i) {
        for (int j=0; j<gridLength; ++j) {
            Cell* curCell = qobject_cast<Cell*>
                (gridLayout->itemAtPosition(i, j)->widget());

            if ((curCell->isMine() && curCell->isOpened())
                || !curCell->isOpened()) {
                allPossibleMines++;
            }
        }
    }

    if (allPossibleMines == mineNumber) {
        gamewin = true;
    }
}












