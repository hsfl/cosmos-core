#include <QWidget>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dataview.h"

QWidget* dataCanvas;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dataCanvas = new DataView();
    //QVBoxLayout* canvasLayout = new QVBoxLayout;
    //canvasLayout->addWidget(dataCanvas);
    //canvasLayout->setContentsMargins(0, 0, 0, 0);
    //dataCanvas->setFixedSize(dataCanvas->baseSize().width(), dataCanvas->baseSize().height());
    ui->scrollArea->setWidget(dataCanvas);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    //Possibly do other stuff too later.
    exit(0);
}
