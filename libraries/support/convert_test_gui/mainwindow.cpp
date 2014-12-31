#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filepathdialog.h"
#include "testcontainer.h"

#define DATFOLD_SIZE    256
TestContainer* tests[NUM_TESTS];
char dataFolder[DATFOLD_SIZE] = {"convert_test_data/"};
int totalSelected = 0;
int totalRun = 0;
char MainWindow::names[NUM_TESTS][11] = {"rearth", "eci2kep", "tl;dr"}; //name of every function being tested, must be IN ORDER!

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->scrollArea);
    QVBoxLayout* testzone = new QVBoxLayout;
    for (int i=0; i<NUM_TESTS; i++) {
        tests[i] = new TestContainer(this, i);
        testzone->addWidget(tests[i]);
        connect(tests[i], SIGNAL(selectionChanged()), this, SLOT(recountTests()));
    }
    testzone->setContentsMargins(0, 0, 0, 0);
    ui->frame->setLayout(testzone);
    recountTests();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ChangeFolder_clicked()
{
    FilepathDialog inputPath;
    inputPath.setModal(true);

    if(inputPath.exec()) {
        QString fpNew = inputPath.getFilepath();
        if (fpNew.size()<=DATFOLD_SIZE-1) strcpy(dataFolder, fpNew.toAscii().constData());
        //need a function to sanitize user input and add extra backslashes.
        //(also need a function to get rid of extra backslashes so the filepath can be displayed properly)
        QString newfp = QString(QLatin1String(dataFolder));
        ui->fpLabel->setText(newfp);
    }
}

void MainWindow::on_CollapseAll_clicked()
{
    for (int i=0; i<NUM_TESTS; i++) tests[i]->toggleHidden(false);
}

void MainWindow::on_SelectAll_clicked()
{
    for (int i=0; i<NUM_TESTS; i++) tests[i]->setSelected(true);
}

void MainWindow::on_DeselectAll_clicked()
{
    for (int i=0; i<NUM_TESTS; i++) tests[i]->setSelected(false);
}

void MainWindow::recountTests() {
    totalSelected = totalRun = 0;
    for (int i=0; i<NUM_TESTS; i++) {
        if (tests[i]->isSelected) totalSelected++;
        if (tests[i]->completed) totalRun++;
    }
    QString readout = QString("%1 functions total, %2 selected, %3 tested.").arg(NUM_TESTS).arg(totalSelected).arg(totalRun);
    ui->testCounter->setText(readout);
}

void MainWindow::on_RunAll_clicked()
{
    for (int i=0; i<NUM_TESTS; i++) tests[i]->runTest();
}

void MainWindow::on_RunSelected_clicked()
{
    for (int i=0; i<NUM_TESTS; i++) if (tests[i]->isSelected) tests[i]->runTest();
}
