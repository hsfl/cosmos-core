/********************************************************************
* Copyright (C) 2015 by Interstel Technologies, Inc.
*   and Hawaii Space Flight Laboratory.
*
* This file is part of the COSMOS/core that is the central
* module for COSMOS. For more information on COSMOS go to
* <http://cosmos-project.com>
*
* The COSMOS/core software is licenced under the
* GNU Lesser General Public License (LGPL) version 3 licence.
*
* You should have received a copy of the
* GNU Lesser General Public License
* If not, go to <http://www.gnu.org/licenses/>
*
* COSMOS/core is free software: you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* as published by the Free Software Foundation, either version 3 of
* the License, or (at your option) any later version.
*
* COSMOS/core is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* Refer to the "licences" folder for further information on the
* condititons and terms to use this software.
********************************************************************/

#include "eci2kep_test.h"
#include <QEvent>

enum element{overall, period, a, e, h, beta, eta, i, raan, ap, alat, ma, ta, ea, mm, fa};
int eciinput_address, keptest_address, kepcorrect_address, kepdelta_address;
//NOTE TO SELF: go here: http://doc.trolltech.com/qq/qq05-generating-xml.html
//and also check out lines 229 on in satlib.cpp for xml writing goodness.
eci2kep_test::eci2kep_test(TestContainer *parent) :
    QWidget(parent),
    ui(new Ui::eci2kep_test)
{
    ui->setupUi(this);
    FILE *KeplerData;
    FILE *ECIData;
    QListWidget *testlist = ui->TestList;
    if ((TestContainer::openFileLine(&KeplerData, "kepler_data.txt", 0)!=0)&&(TestContainer::openFileLine(&ECIData, "eci_data.txt", 0)!=0)) {
        int numTests, idx;
        test_maxErr = period_maxErr = a_maxErr = e_maxErr = h_maxErr = beta_maxErr = eta_maxErr = i_maxErr = raan_maxErr = ap_maxErr = alat_maxErr = ma_maxErr = ta_maxErr = ea_maxErr = mm_maxErr = fa_maxErr =
        test_minErr = period_minErr = a_minErr = e_minErr = h_minErr = beta_minErr = eta_minErr = i_minErr = raan_minErr = ap_minErr = alat_minErr = ma_minErr = ta_minErr = ea_minErr = mm_minErr = fa_minErr = 0;
        double aveTestErr = 0.0, maxTestErr = 0.0, minTestErr = 0.0, testErr = 0.0, correctSum = 0.0, aveTestSum = 0.0;
        fscanf(KeplerData, "%d", &numTests);
        fscanf(ECIData, "%d", &idx);
        TestContainer::skipLines(KeplerData, 2);
        TestContainer::skipLines(ECIData, 2);
        numTests = (numTests>idx)?idx:numTests;
        kepstruc kepcorrect, keptest, kepdelta, kepMaxErr, kepMinErr;
        cartpos eciinput;
        idx = 0;
        while (idx<numTests&&(!feof(KeplerData)||!feof(ECIData))) {
            fscanf(KeplerData, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", &kepcorrect.beta, &kepcorrect.period, &kepcorrect.alat, &kepcorrect.ap, &kepcorrect.ea, &kepcorrect.e, &kepcorrect.h.col[0], &kepcorrect.fa, &kepcorrect.i, &kepcorrect.ma, &kepcorrect.mm, &kepcorrect.raan, &kepcorrect.a, &kepcorrect.ta);
            kepcorrect.fa -= RADOF(90.);
            kepcorrect.h.col[0] = kepcorrect.h.col[1] = kepcorrect.h.col[2] = 1.;//for now... (but bear in mind it's not correct
            fscanf(ECIData, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", &eciinput.s.col[0], &eciinput.s.col[1], &eciinput.s.col[2], &eciinput.v.col[0], &eciinput.v.col[1], &eciinput.v.col[2], &eciinput.a.col[0], &eciinput.a.col[1], &eciinput.a.col[2]);
            eciinput.utc = 55927;
            eciinput.a.col[3] = eciinput.s.col[3] = eciinput.v.col[3] = 0.0;
            eci2kep(&eciinput, &keptest); //make sure to copy the files: unxp2000.405, wmm_2010.cof & egm2008_coef.txt from MOSTexport/resources/static into current build folder.
            kepdelta = TestContainer::keplerDelta(keptest, kepcorrect);
            //TEMPORARY UNTILL h vector DATA CAN BE OBTAINED FROM STK:
            kepdelta.h = rv_zero();
            //-------
            testErr = (100*(fabs(kepdelta.period)+fabs(kepdelta.a)+fabs(kepdelta.e)+length_rv(kepdelta.h)+fabs(kepdelta.beta)+fabs(kepdelta.i)+fabs(kepdelta.raan)+fabs(kepdelta.ap)
            +fabs(kepdelta.alat)+fabs(kepdelta.ma)+fabs(kepdelta.ta)+fabs(kepdelta.ea)+fabs(kepdelta.mm)+fabs(kepdelta.fa)))/(fabs(kepcorrect.period)+fabs(kepcorrect.a)
            +fabs(kepcorrect.e)+length_rv(kepcorrect.h)+fabs(kepcorrect.beta)+fabs(kepcorrect.i)+fabs(kepcorrect.raan)+fabs(kepcorrect.ap)+fabs(kepcorrect.alat)+fabs(kepcorrect.ma)
            +fabs(kepcorrect.ta)+fabs(kepcorrect.ea)+fabs(kepcorrect.mm)+fabs(kepcorrect.fa));
            QListWidgetItem *newTest = new QListWidgetItem(QString("Test %1: total error: %2 %").arg(idx+1).arg(testErr), testlist);
            int j = 0;
            newTest->setData(Qt::UserRole+j, QVariant(testErr));
            j++;
            eciinput_address = j;
            TestContainer::listWriteCartpos(newTest, eciinput, &j);
            keptest_address = j;
            TestContainer::listWriteKepstruc(newTest, keptest, &j);
            kepcorrect_address = j;
            TestContainer::listWriteKepstruc(newTest, kepcorrect, &j);
            kepdelta_address = j;
            TestContainer::listWriteKepstruc(newTest, kepdelta, &j);
            if (idx==0) {
                kepMaxErr = kepMinErr = kepdelta;
                maxTestErr = minTestErr = testErr;
            } else {
                if (maxTestErr<testErr) {
                    maxTestErr = testErr;
                    test_maxErr = idx;
                } else if (minTestErr>testErr) {
                    minTestErr = testErr;
                    test_minErr = idx;
                }
                if (fabs(kepMaxErr.period)<fabs(kepdelta.period)) {
                    kepMaxErr.period = kepdelta.period;
                    period_maxErr = idx;
                } else if (fabs(kepMinErr.period)>fabs(kepdelta.period)) {
                    kepMinErr.period = kepdelta.period;
                    period_minErr = idx;
                }
                if (fabs(kepMaxErr.a)<fabs(kepdelta.a)) {
                    kepMaxErr.a = kepdelta.a;
                    a_maxErr = idx;
                } else if (fabs(kepMinErr.a)>fabs(kepdelta.a)) {
                    kepMinErr.a = kepdelta.a;
                    a_minErr = idx;
                }
                if (fabs(kepMaxErr.e)<fabs(kepdelta.e)) {
                    kepMaxErr.e = kepdelta.e;
                    e_maxErr = idx;
                } else if (fabs(kepMinErr.e)>fabs(kepdelta.e)) {
                    kepMinErr.e = kepdelta.e;
                    e_minErr = idx;
                }
                if (length_rv(kepMaxErr.h)<length_rv(kepdelta.h)) {
                    kepMaxErr.h = kepdelta.h;
                    h_maxErr = idx;
                } else if (length_rv(kepMinErr.h)>length_rv(kepdelta.h)) {
                    kepMinErr.h = kepdelta.h;
                    h_minErr = idx;
                }
                if (fabs(kepMaxErr.beta)<fabs(kepdelta.beta)) {
                    kepMaxErr.beta = kepdelta.beta;
                    beta_maxErr = idx;
                } else if (fabs(kepMinErr.beta)>fabs(kepdelta.beta)) {
                    kepMinErr.beta = kepdelta.beta;
                    beta_minErr = idx;
                }
                if (fabs(kepMaxErr.eta)<fabs(kepdelta.eta)) {
                    kepMaxErr.eta = kepdelta.eta;
                    eta_maxErr = idx;
                } else if (fabs(kepMinErr.eta)>fabs(kepdelta.eta)) {
                    kepMinErr.eta = kepdelta.eta;
                    eta_minErr = idx;
                }
                if (fabs(kepMaxErr.i)<fabs(kepdelta.i)) {
                    kepMaxErr.i = kepdelta.i;
                    i_maxErr = idx;
                } else if (fabs(kepMinErr.i)>fabs(kepdelta.i)) {
                    kepMinErr.i = kepdelta.i;
                    i_minErr = idx;
                }
                if (fabs(kepMaxErr.raan)<fabs(kepdelta.raan)) {
                    kepMaxErr.raan = kepdelta.raan;
                    raan_maxErr = idx;
                } else if (fabs(kepMinErr.raan)>fabs(kepdelta.raan)) {
                    kepMinErr.raan = kepdelta.raan;
                    raan_minErr = idx;
                }
                if (fabs(kepMaxErr.alat)<fabs(kepdelta.alat)) {
                    kepMaxErr.alat = kepdelta.alat;
                    alat_maxErr = idx;
                } else if (fabs(kepMinErr.alat)>fabs(kepdelta.alat)) {
                    kepMinErr.alat = kepdelta.alat;
                    alat_minErr = idx;
                }
                if (fabs(kepMaxErr.ma)<fabs(kepdelta.ma)) {
                    kepMaxErr.ma = kepdelta.ma;
                    ma_maxErr = idx;
                } else if (fabs(kepMinErr.ma)>fabs(kepdelta.ma)) {
                    kepMinErr.ma = kepdelta.ma;
                    ma_minErr = idx;
                }
                if (fabs(kepMaxErr.ta)<fabs(kepdelta.ta)) {
                    kepMaxErr.ta = kepdelta.ta;
                    ta_maxErr = idx;
                } else if (fabs(kepMinErr.ta)>fabs(kepdelta.ta)) {
                    kepMinErr.ta = kepdelta.ta;
                    ta_minErr = idx;
                }
                if (fabs(kepMaxErr.ea)<fabs(kepdelta.ea)) {
                    kepMaxErr.ea = kepdelta.ea;
                    ea_maxErr = idx;
                } else if (fabs(kepMinErr.ea)>fabs(kepdelta.ea)) {
                    kepMinErr.ea = kepdelta.ea;
                    ea_minErr = idx;
                }
                if (fabs(kepMaxErr.mm)<fabs(kepdelta.mm)) {
                    kepMaxErr.mm = kepdelta.mm;
                    mm_maxErr = idx;
                } else if (fabs(kepMinErr.mm)>fabs(kepdelta.mm)) {
                    kepMinErr.mm = kepdelta.mm;
                    mm_minErr = idx;
                }
                if (fabs(kepMaxErr.fa)<fabs(kepdelta.fa)) {
                    kepMaxErr.fa = kepdelta.fa;
                    fa_maxErr = idx;
                } else if (fabs(kepMinErr.fa)>fabs(kepdelta.fa)) {
                    kepMinErr.fa = kepdelta.fa;
                    fa_minErr = idx;
                }
            }
            aveTestErr += testErr;
            idx++;
        }
        fclose(KeplerData);
        fclose(ECIData);
        aveTestErr = aveTestErr/idx;
        parent->setError(minTestErr, maxTestErr, aveTestErr);
        ui->TestCount->setText(QString("%1 Tests:").arg(numTests));
        ui->comboBox->setCurrentIndex(overall);
        ui->testsOverview->setText(QString("Max error in #%1 Min error in #%2").arg(test_maxErr+1).arg(test_minErr+1));
        ui->TestList->item(test_maxErr)->setBackground(Qt::red);
        ui->TestList->item(test_minErr)->setBackground(Qt::darkGreen);
    }
}

eci2kep_test::~eci2kep_test()
{
    delete ui;
}

void eci2kep_test::on_comboBox_currentIndexChanged(int index)
{
    int maxErr, minErr, idx;
    switch (index) {
    case (overall):
        maxErr = test_maxErr;
        minErr = test_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: total error: %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole).toDouble()));
        }
        break;
    case (period):
        maxErr = period_maxErr;
        minErr = period_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: period (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+1).toDouble()));
        }
        break;
    case (a):
        maxErr = a_maxErr;
        minErr = a_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: a (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+2).toDouble()));
        }
        break;
    case (e):
        maxErr = e_maxErr;
        minErr = e_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: e (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+3).toDouble()));
        }
        break;
    case (h):
        maxErr = h_maxErr;
        minErr = h_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: h (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+4).toDouble()));
        }
        break;
    case (beta):
        maxErr = beta_maxErr;
        minErr = beta_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: beta (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+7).toDouble()));
        }
        break;
    case (eta):
        maxErr = eta_maxErr;
        minErr = eta_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: eta (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+8).toDouble()));
        }
        break;
    case (i):
        maxErr = i_maxErr;
        minErr = i_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: i (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+9).toDouble()));
        }
        break;
    case (raan):
        maxErr = raan_maxErr;
        minErr = raan_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: raan (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+10).toDouble()));
        }
        break;
    case (ap):
        maxErr = ap_maxErr;
        minErr = ap_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: ap (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+11).toDouble()));
        }
        break;
    case (alat):
        maxErr = alat_maxErr;
        minErr = alat_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: alat (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+12).toDouble()));
        }
        break;
    case (ma):
        maxErr = ma_maxErr;
        minErr = ma_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: ma (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+13).toDouble()));
        }
        break;
    case (ta):
        maxErr = ta_maxErr;
        minErr = ta_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: ta (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+14).toDouble()));
        }
        break;
    case (ea):
        maxErr = ea_maxErr;
        minErr = ea_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: ea (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+15).toDouble()));
        }
        break;
    case (mm):
        maxErr = mm_maxErr;
        minErr = mm_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: mm (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+16).toDouble()));
        }
        break;

    case (fa):
        maxErr = fa_maxErr;
        minErr = fa_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: fa (error): %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+17).toDouble()));
        }
        break;
    default:
        maxErr = test_maxErr;
        minErr = test_minErr;
        for (idx = 0; idx<ui->TestList->count(); idx++) {
            ui->TestList->item(idx)->setBackground(Qt::NoBrush);
            ui->TestList->item(idx)->setText(QString("Test %1: total error: %2 %").arg(idx+1).arg(ui->TestList->item(idx)->data(Qt::UserRole+kepdelta_address+18).toDouble()));
        }
        break;
    }
    ui->testsOverview->setText(QString("Max error in #%1 Min error in #%2").arg(maxErr+1).arg(minErr+1));
    ui->TestList->item(maxErr)->setBackground(Qt::red);
    ui->TestList->item(minErr)->setBackground(Qt::darkGreen);
}

void eci2kep_test::on_SelectMax_clicked()
{
    int idx;
    switch (ui->comboBox->currentIndex()) {
    case (overall): idx = test_maxErr; break;
    case (period): idx = period_maxErr; break;
    case (a): idx = a_maxErr; break;
    case (e): idx = e_maxErr; break;
    case (h): idx = h_maxErr; break;
    case (beta): idx = beta_maxErr; break;
    case (eta): idx = eta_maxErr; break;
    case (i): idx = i_maxErr; break;
    case (raan): idx = raan_maxErr; break;
    case (ap): idx = ap_maxErr; break;
    case (alat): idx = alat_maxErr; break;
    case (ma): idx = ma_maxErr; break;
    case (ta): idx = ta_maxErr; break;
    case (ea): idx = ea_maxErr; break;
    case (mm): idx = mm_maxErr; break;
    case (fa): idx = fa_maxErr; break;
    default: idx = test_maxErr; break;
    }
    ui->TestList->setCurrentRow(idx);
}

void eci2kep_test::on_SelectMin_clicked()
{
    int idx;
    switch (ui->comboBox->currentIndex()) {
    case (overall): idx = test_minErr; break;
    case (period): idx = period_minErr; break;
    case (a): idx = a_minErr; break;
    case (e): idx = e_minErr; break;
    case (h): idx = h_minErr; break;
    case (beta): idx = beta_minErr; break;
    case (eta): idx = eta_minErr; break;
    case (i): idx = i_minErr; break;
    case (raan): idx = raan_minErr; break;
    case (ap): idx = ap_minErr; break;
    case (alat): idx = alat_minErr; break;
    case (ma): idx = ma_minErr; break;
    case (ta): idx = ta_minErr; break;
    case (ea): idx = ea_minErr; break;
    case (mm): idx = mm_minErr; break;
    case (fa): idx = fa_minErr; break;
    default: idx = test_minErr; break;
    }
    ui->TestList->setCurrentRow(idx);
}
