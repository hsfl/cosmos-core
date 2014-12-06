#ifndef DEVICEFCTNS_H
#define DEVICEFCTNS_H

#include "cssl_lib.h"

void FindBbPort(int *BbPortNum);
bool OpenBb(cssl_t **SerialBb, int BbPortNum);
void runBbUnit1(float DesiredTemperature, cssl_t *SerialBb);

#endif // DEVICEFCTNS_H
