#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , startButton(new QPushButton("!!!", this))
    , central(new QWidget(this))
    , mainVerticalLayout(new QVBoxLayout(central))
    , buttonsLayout(new QHBoxLayout)
    , gridLayout(new QGridLayout)
{
    ui->setupUi(this);

    startButton->setFixedSize(30, 30);

    //设置左上右下边距
    mainVerticalLayout->setContentsMargins(10, 5, 10, 5);

    //添加startButton到控件
    buttonsLayout->addWidget(startButton);
    buttonsLayout->setAlignment(Qt::AlignHCenter);//水平居中
    //将水平布局插入到竖直布局
    mainVerticalLayout->addLayout(buttonsLayout);

    generateGrid();

    //在竖直布局里加入伸展，把startButton顶上去
    mainVerticalLayout->addStretch();

    setCentralWidget(central);

    connect(startButton, &QPushButton::clicked
            , this, &MainWindow::generateGrid);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateGrid(){
    if (gridLayout) {
        delete gridLayout;
        gridLayout = nullptr;
    }

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(3);

    for (int i=0; i<gridHeight; ++i) {
        for (int j=0; j<gridLength; ++j) {
            QPushButton *cell = new QPushButton();
            cell->setFixedSize(10, 10);
            cell->setText("");
            gridLayout->addWidget(cell, i, j);
        }
    }
    mainVerticalLayout->insertLayout(1, gridLayout);
}
