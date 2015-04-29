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

#ifndef ECI2KEP_TEST_H
#define ECI2KEP_TEST_H

#include <QWidget>
#include "testcontainer.h"
#include "ui_eci2kep_test.h"

namespace Ui {
class eci2kep_test;
}

class eci2kep_test : public QWidget
{
    Q_OBJECT
    
public:
    explicit eci2kep_test(TestContainer *parent = 0);
    ~eci2kep_test();
    
private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_SelectMax_clicked();

    void on_SelectMin_clicked();

private:
    Ui::eci2kep_test *ui;

    int test_maxErr, period_maxErr, a_maxErr, e_maxErr, h_maxErr, beta_maxErr, eta_maxErr, i_maxErr, raan_maxErr, ap_maxErr, alat_maxErr, ma_maxErr, ta_maxErr, ea_maxErr, mm_maxErr, fa_maxErr;
    int test_minErr, period_minErr, a_minErr, e_minErr, h_minErr, beta_minErr, eta_minErr, i_minErr, raan_minErr, ap_minErr, alat_minErr, ma_minErr, ta_minErr, ea_minErr, mm_minErr, fa_minErr;
};

#endif // ECI2KEP_TEST_H
