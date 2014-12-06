#include "rearth_test.h"

rearth_test::rearth_test(TestContainer* parent) :
    QWidget(parent),
    ui(new Ui::rearth_test)
{
    ui->setupUi(this);
    QListWidget *testlist = ui->TestList;
    FILE *LATvRdata;
    if (TestContainer::openFileLine(&LATvRdata, "LATvR.txt", 0)!=0) { //For some reason it says this isn't declared.
        int i, numTests;
        double lat, radFunction, radCorrect, delta, minDelta, minDeltaRadius, maxDelta, maxDeltaRadius, averageDelta=0.0, averageRadius;
        fscanf(LATvRdata, "%d", &numTests);
        TestContainer::skipLines(LATvRdata, 2);
        for (i=0; i<numTests; i++) {
            fscanf(LATvRdata, "%lf%lf", &lat, &radCorrect);
            radFunction = rearth(lat);
            delta = radFunction-radCorrect;
            averageDelta += delta;
            averageRadius +=radCorrect;
            if (i==0) {
                maxDelta = minDelta = delta;
                maxDeltaRadius = minDeltaRadius = radCorrect;
                MaxErrIndex = MinErrIndex = i;
            } else if (fabs(delta)>fabs(maxDelta)) {
                maxDelta = delta;
                maxDeltaRadius = radCorrect;
                MaxErrIndex = i;
            } else if (fabs(delta)<fabs(minDelta)) {
                minDelta = delta;
                minDeltaRadius = radCorrect;
                MinErrIndex = i;
            }
            QListWidgetItem *newTest = new QListWidgetItem(QString("lat: %1°, rearth: %2, correct: %3, delta: %4").arg(DEGOF(lat)).arg(radFunction).arg(radCorrect).arg(delta), testlist);
            QVariant latitude(lat);
            newTest->setData(Qt::UserRole, latitude);
            QVariant functionOut(radFunction);
            newTest->setData(Qt::UserRole+1, functionOut);
            QVariant correct(radCorrect);
            newTest->setData(Qt::UserRole+2, correct);
            QVariant error(delta);
            newTest->setData(Qt::UserRole+3, error);
            //add custom data for each test, helpful url: http://stackoverflow.com/questions/7136818/can-i-store-some-user-data-in-every-item-of-a-qlistwidget
        }
        fclose(LATvRdata);
        parent->setError((minDelta*100)/minDeltaRadius, (maxDelta*100)/maxDeltaRadius, (averageDelta*100)/averageRadius);
        averageDelta /= numTests;
        averageRadius /= numTests;
        ui->TestCount->setText(QString("%1 Tests:").arg(numTests));
        ui->testsOverview->setText(QString("Max error in #%1 Min error in #%2").arg(MaxErrIndex+1).arg(MinErrIndex+1));
        testlist->item(MaxErrIndex)->setBackground(Qt::red);
        testlist->item(MinErrIndex)->setBackground(Qt::darkGreen);
    }
}

rearth_test::~rearth_test()
{
    delete ui;
}

void rearth_test::on_TestList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    ui->TestInfo->setTitle(QString("Test %1").arg(int(ui->TestList->currentRow())+1));
    double lat=(current->data(Qt::UserRole)).toDouble(), output=(current->data(Qt::UserRole+1)).toDouble();
    double correct=(current->data(Qt::UserRole+2)).toDouble(), error=(current->data(Qt::UserRole+3)).toDouble();
    double percentError = (error/correct)*100.0;
    ui->TestInfo->setEnabled(true);
    ui->LatDegrees->display(DEGOF(lat));
    ui->LatPiRadians->display(lat/DPI);
    ui->LatNormalRadians->display(lat);
    ui->RadiusOut->display(output/1000);
    ui->RadiusCorrect->display(correct/1000);
    ui->DeltaDisplay->display(error);
    ui->PercentError->display(percentError);
}

void rearth_test::on_SelectMax_clicked()
{
    ui->TestList->setCurrentRow(MaxErrIndex);
}

void rearth_test::on_SelectMin_clicked()
{
    ui->TestList->setCurrentRow(MinErrIndex);
}
