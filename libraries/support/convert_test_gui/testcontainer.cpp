#include "testcontainer.h"
#include "ui_testcontainer.h"
#include "mainwindow.h"
#include <QSizePolicy>
#include "rearth_test.h"
#include "eci2kep_test.h"

TestContainer::TestContainer(MainWindow* parent, int testNumber) :
    QWidget(parent),
    ui(new Ui::TestContainer)
{
    ui->setupUi(this);
    hidden = true;
    number = testNumber;
    isSelected = true;
    completed = false;
    toggleHidden(!hidden);
    QString name = QString::fromAscii(parent->names[number], -1);
    ui->nameLabel->setText(name);
    fadeBanner(true);
    ui->mainpanel->setLayout(new QVBoxLayout);
    ui->mainpanel->layout()->setContentsMargins(0, 0, 0, 0);
}

TestContainer::~TestContainer()
{
    delete ui;
}

void TestContainer::runTest() {
    if (completed&&number!=2) { //get rid of number!=2, just needed to prevent crashing by empty testcontainers.
        delete ui->mainpanel->layout()->takeAt(0)->widget();
        delete ui->mainpanel->layout()->takeAt(0);
        delete ui->mainpanel->layout();
        ui->mainpanel->setLayout(new QVBoxLayout);
        ui->mainpanel->layout()->setContentsMargins(0, 0, 0, 0);
    }
    switch(number) {
    case(0):
    {
        rearth_test* test = new rearth_test(this);
        ui->mainpanel->layout()->addWidget(test);
        break;
    }
    case(1):
    {
        eci2kep_test* test = new eci2kep_test(this);
        ui->mainpanel->layout()->addWidget(test);
        break;
    }
    default:
        break;
    }
    fadeBanner(false);
    if (!completed) {
        ui->Expand->setText("Expand");
        completed = true;
    }
    emit selectionChanged();
}

void TestContainer::on_Expand_clicked()
{
    if (completed) toggleHidden(hidden);
    else {
        runTest();
        toggleHidden(hidden);
    }
}

void TestContainer::toggleHidden(bool show) {
    ui->mainpanel->setEnabled(show);
    if (show&&completed) {
        ui->mainpanel->setMaximumHeight(16777215);
        ui->mainpanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        ui->Expand->setText("Collapse");
    } else {
        ui->mainpanel->setMaximumHeight(0);
        ui->mainpanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        if (completed) ui->Expand->setText("Expand");
    }
    hidden = !show;
}

void TestContainer::setError(double min, double max, double avg) {
    MinError = min;
    MaxError = max;
    AvgError = avg;
    ui->MinErr->display(MinError);
    ui->MaxErr->display(MaxError);
    ui->AvgErr->display(AvgError);
}

void TestContainer::on_checkBox_stateChanged(int arg1)
{
    if (arg1==0) isSelected = false;
    else if(arg1==2) isSelected = true;
    emit selectionChanged();
}

void TestContainer::setSelected(bool selected) {
    ui->checkBox->setChecked(selected);
}

void TestContainer::fadeBanner(bool faded) {
    ui->nameLabel->setEnabled(!faded);
    ui->MinLabel->setEnabled(!faded);
    ui->MinErr->setEnabled(!faded);
    ui->MaxLabel->setEnabled(!faded);
    ui->MaxErr->setEnabled(!faded);
    ui->AvgLabel->setEnabled(!faded);
    ui->AvgErr->setEnabled(!faded);
}

void TestContainer::skipLines(FILE *fp, int lines) {
    char c;
    for (; lines>0; lines--) { //skip 'lines' many lines
        c=0;
        do { //scan through file untill the end of a line is reached
            c = fgetc(fp);
            if (c==EOF) return; //We've gone too far!!!
        } while(c!='\n');
    }
}

int TestContainer::openFileLine(FILE **fp, const char filename[], int startLine) {
    char filePath[40] = {"../convert_test_data/"};//the folder containing the test files
    *fp = fopen(strcat(filePath, filename), "r"); //open the file
    if (fp == NULL) return(0); //return 0 for failure....
    skipLines(*fp, startLine); //move the pointer to startLine
    return(1); //return 1 for sucess!!!!
}

void TestContainer::listWriteRVec(QListWidgetItem *listItem, rvector vect, int *roleIndex) {
    QVariant vecX(vect.col[0]);
    listItem->setData(Qt::UserRole+*roleIndex, vecX);
    QVariant vecY(vect.col[1]);
    listItem->setData(Qt::UserRole+*roleIndex+1, vecY);
    QVariant vecZ(vect.col[2]);
    listItem->setData(Qt::UserRole+*roleIndex+2, vecZ);
    *roleIndex += 3;
}

void TestContainer::listWriteCartpos(QListWidgetItem *listItem, cartpos pos, int *roleIndex) {
    listWriteRVec(listItem, pos.s, roleIndex);
    listWriteRVec(listItem, pos.v, roleIndex);
    listWriteRVec(listItem, pos.a, roleIndex);
    QVariant UTC(pos.utc);
    listItem->setData(Qt::UserRole+*roleIndex, UTC);
    *roleIndex += 1;
}

void TestContainer::listWriteKepstruc(QListWidgetItem *listItem, kepstruc kep, int *roleIndex) {
    QVariant doubleVal(kep.utc);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.period);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.a);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.e);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    listWriteRVec(listItem, kep.h, roleIndex);
    doubleVal = QVariant(kep.beta);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.eta);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.i);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.raan);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.ap);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.alat);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.ma);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.ta);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.ea);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.mm);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
    doubleVal = QVariant(kep.fa);
    listItem->setData(Qt::UserRole+*roleIndex, doubleVal);
    *roleIndex += 1;
}

double TestContainer::angleDelta(double angle1, double angle2) { //compares the two angles (radians) returns angle1-angle2
    if (angle1>DPI) angle1 -= D2PI;
    else if (angle1<-DPI) angle1 += D2PI;
    if (angle2>DPI) angle2 -= D2PI;
    else if (angle2<-DPI) angle2 += D2PI;
    double delta = angle1 - angle2;
    if (delta>DPI) delta -= D2PI;
    else if (delta<-DPI) delta += D2PI;
    return(delta);
}

kepstruc TestContainer::keplerDelta(kepstruc kep1, kepstruc kep2) { //compares two kepstrucs and returns kep1-kep2
    kepstruc delta;
    delta.utc = kep1.utc - kep2.utc;
    delta.period = kep1.period - kep2.period;
    delta.a = kep1.a - kep2.a;
    delta.e = kep1.e - kep2.e;
    delta.h = rv_sub(kep1.h, kep2.h);
    delta.beta = angleDelta(kep1.beta, kep2.beta);
    delta.eta = angleDelta(kep1.eta, kep2.eta);
    delta.i = angleDelta(kep1.i, kep2.i);
    delta.raan = angleDelta(kep1.raan, kep2.raan);
    delta.ap = angleDelta(kep1.ap, kep2.ap);
    delta.alat = angleDelta(kep1.alat, kep2.alat);
    delta.ma = angleDelta(kep1.ma, kep2.ma);
    delta.ta = angleDelta(kep1.ta, kep2.ta);
    delta.ea = angleDelta(kep1.ea, kep2.ea);
    delta.mm = angleDelta(kep1.mm, kep2.mm);
    delta.fa = angleDelta(kep1.fa, kep2.fa);
    return (delta);
}
