#include "mathlib.h"
#include "convertlib.h"
#include <stdio.h>
#include <string.h>

//maximum length+1 (for the null terminator) of any function name in the library
#define MAX_NAME    15

/*My own little set of file reading functions*/
int openFileLine(FILE **fp, const char filename[], int startLine);//opens a file from 'convert_test_data' for reading and moves the pointer to startLine
void skipLines(FILE *fp, int lines);//skips 'lines' many lines, stops at EOF if necessary

int main(int argc, char *argv[])
{
/*remember to adjust the number of loops in line 18 to match the number of names here*/
char names[][MAX_NAME] = {"rearth","kep2eci","eci2kep"}; //the name of every function being tested, must be IN ORDER!
int i, jumpto = -1, skip = -1; //normally we don't jump to or skip anything, so those are initialized out of range.
double error, adjerror,avgerror, minerror, maxerror, terror, tminerror, tmaxerror;
error = adjerror = avgerror = minerror = maxerror = terror = tminerror = tmaxerror = 0.;
i=0;
while (argc>1&&i<3) { //If arguments were entered: loop through the names array,...
    if (strcmp(argv[1], names[i]) == 0) { //and determine if we're to supposed to jump to a specific function,...
	jumpto = i;
	printf("\nFunctionality Test:\t%s\n\n", names[i]);
	break;
    } else if ((*argv[1] == '-')&&(strcmp(&argv[1][1], names[i]) == 0)) {//or skip a specific function.
	skip = i;
	jumpto = 0;
	printf("\nFunctionality Tests:\tconvertlib (without %s)\n\n", names[i]);
	break;
    }
    i++;
}
/*Tests can test a single function, or a small group of functions, but
must provide error values individually for every function within the test.
Each test is contained within a switch case, so they don't interfere with
eachother, and can be excluded or run alone.*/
switch (jumpto) {
case -1: //Default first line if no arguments were entered
    printf("\nFunctionality Tests:\tconvertlib\n\n");
case 0:
    if (skip!=0){ //if skip is the same as this case value, skip this function
    printf("Function: rearth()\n");
    FILE *LATvRdata;
    if (openFileLine(&LATvRdata, "LATvR.txt", 0)!=0) {
        i=0;
        int z = 30; //display one test out of every ___
        int numTests; //the number of tests (written at top of file)
        fscanf(LATvRdata, "%d", &numTests);
        skipLines(LATvRdata, 2);
        double lat, rad, maxErrorL, minErrorL, maxErrorR, minErrorR;
        error = adjerror = avgerror = 0.0;
        printf("Latitude:                \tRadius(function):\tRadius(correct):\t  Delta:\n");
        printf("(deg)\t(rad)             \t(m)\t\t\t(m)\t\t\t  (m)\n");//this is very nicely formatted, please don't break it.
        while (i<numTests&&!feof(LATvRdata)) {
            fscanf(LATvRdata, "%lf%lf", &lat, &rad);
            error = rearth(lat) - rad;
            if (i==0) {
                minerror = maxerror = error;
                maxErrorL = minErrorL = lat;
                maxErrorR = minErrorR = rad;
            } else if (fabs(error)>fabs(maxerror)) {
                maxerror = error;
                maxErrorL = lat;
                maxErrorR = rad;
            } else if (fabs(error)<fabs(minerror)) {
                minerror = error;
                minErrorL = lat;
                minErrorR = rad;
            }
            avgerror += fabs(error);
            adjerror = avgerror/rad;
            if (i%z==0||i==0) {
                printf("%0.0f \t%0.15f\t%0.9f  (-)  %0.9f (=) %11.5g\n", DEGOF(lat), lat, rearth(lat), rad, error);
            }
            i++;
        }
        fclose(LATvRdata);
        printf("%d tests total.  One out of every %d shown.\n",i,z);
        printf("\n");
        adjerror = adjerror/i;
        avgerror = avgerror/i;
        printf("Max Error:%0.0f \t%0.15f\t%0.9f  (-)  %0.9f (=) %11.5g\n", DEGOF(maxErrorL), maxErrorL, rearth(maxErrorL), maxErrorR, maxerror);
        printf("Min Error:%0.0f \t%0.15f\t%0.9f  (-)  %0.9f (=) %11.5g\n", DEGOF(minErrorL), minErrorL, rearth(minErrorL), minErrorR, minerror);
        printf("Average Error:%11.5g\tAdjusted Error:%11.5g\n", avgerror, adjerror);
        printf("\n");
        terror += adjerror;
        printf("rearth() Error: %11.5g\tTerror:%11.5g\n",adjerror, terror);
        printf("\n");
    }
    if (jumpto!=-1) break; //this function was jumped to, no other functions need to be tested.
    }
case 1: //this is the first of a group of functions tested together, drop through cases to the actuall test.
    if (skip!=1) {
    printf("Function: kep2eci()\n");
    if (jumpto!=-1) break;
    }
case 2:
    if (skip!=2) {
    printf("Function: eci2kep()\n");
    FILE *KeplerData;
    FILE *ECIData;
    if ((openFileLine(&KeplerData, "kepler_data.txt", 0)!=0)&&(openFileLine(&ECIData, "eci_data.txt", 2)!=0)) {
        int numTests;
        fscanf(KeplerData, "%d", &numTests);
        skipLines(KeplerData, 2);
        kepstruc kepcorrect, keptest, Kmaxerror, Kminerror;
        cartpos eciinput, Emaxerror, Eminerror;
        int z = 100; //display one test out of every ___
        i=0;
        printf("ECI vector input |#|Kepler element output\n");
        while (i<numTests&&(!feof(KeplerData)||!feof(ECIData))) {
            fscanf(KeplerData, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", &kepcorrect.beta, &kepcorrect.period, &kepcorrect.alat, &kepcorrect.ap, &kepcorrect.ea, &kepcorrect.e, &kepcorrect.h.col[0], &kepcorrect.fa, &kepcorrect.i, &kepcorrect.ma, &kepcorrect.mm, &kepcorrect.raan, &kepcorrect.a, &kepcorrect.ta);
            kepcorrect.fa -= RADOF(90.);
            fscanf(ECIData, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", &eciinput.s.col[0], &eciinput.s.col[1], &eciinput.s.col[2], &eciinput.v.col[0], &eciinput.v.col[1], &eciinput.v.col[2], &eciinput.a.col[0], &eciinput.a.col[1], &eciinput.a.col[2]);
            eciinput.utc = 55927;
            eci2kep(&eciinput, &keptest);
            error = fabs(keptest.beta - kepcorrect.beta)+fabs(keptest.period - kepcorrect.period)+fabs(keptest.alat - kepcorrect.alat)+fabs(keptest.ap - kepcorrect.ap)+fabs(keptest.ea - kepcorrect.ea)+fabs(keptest.e - kepcorrect.e)/*+fabs(keptest.h.col[0] - kepcorrect.h.col[0])*/+fabs(keptest.fa - kepcorrect.fa)+fabs(keptest.i - kepcorrect.i)+fabs(keptest.ma - kepcorrect.ma)+fabs(keptest.mm - kepcorrect.mm)+fabs(keptest.raan - kepcorrect.raan)+fabs(keptest.a - kepcorrect.a)+fabs(keptest.ta - kepcorrect.ta);
            if (i==0) {
                minerror = maxerror = error;
                Kmaxerror = Kminerror = kepcorrect;
                Emaxerror = Eminerror = eciinput;
            } else if (fabs(error)>fabs(maxerror)) {
                maxerror = error;
                Kmaxerror = kepcorrect;
                Emaxerror = eciinput;
            } else if (fabs(error)<fabs(minerror)) {
                minerror = error;
                Kminerror = kepcorrect;
                Eminerror = eciinput;
            }
            avgerror += fabs(error);
            if (i%z==0||i==0) {
                printf("%d______________________________________________________________________________________________________________\n",i);
                printf("pos x:%11.5g|#|         Beta Angle |Period     |alat       |arg per    |E anom     |e          |flight ang\n",eciinput.s.col[0]);
                printf("    y:%11.5g|#|function:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g\n",eciinput.s.col[1],keptest.beta,keptest.period,keptest.alat,keptest.ap,keptest.ea,keptest.e,keptest.fa);
                printf("    z:%11.5g|#| correct:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g\n",eciinput.s.col[2],kepcorrect.beta,kepcorrect.period,kepcorrect.alat,kepcorrect.ap,kepcorrect.ea,kepcorrect.e,kepcorrect.fa);
                printf("vel x:%11.5g|#|   delta:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g\n",eciinput.v.col[0],(keptest.beta-kepcorrect.beta),(keptest.period-kepcorrect.period),(keptest.alat-kepcorrect.alat),(keptest.ap-kepcorrect.ap),(keptest.ea-kepcorrect.ea),(keptest.e-kepcorrect.e),(keptest.fa-kepcorrect.fa));
                printf("    y:%11.5g|#|         i          |mean anom  |mean motion|raan       |a          |true anomaly           \n",eciinput.v.col[1]);
                printf("    z:%11.5g|#|function:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g            \n",eciinput.v.col[2],keptest.i,keptest.ma,keptest.mm,keptest.raan,keptest.a,keptest.ta);
                printf("acc x:%11.5g|#| correct:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g            \n",eciinput.a.col[0],kepcorrect.i,kepcorrect.ma,kepcorrect.mm,kepcorrect.raan,kepcorrect.a,kepcorrect.ta);
                printf("    y:%11.5g|#|   delta:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g            \n",eciinput.a.col[1],(keptest.i-kepcorrect.i),(keptest.ma-kepcorrect.ma),(keptest.mm-kepcorrect.mm),(keptest.raan-kepcorrect.raan),(keptest.a-kepcorrect.a),(keptest.ta-kepcorrect.ta));
                printf("    z:%11.5g|#|\n",eciinput.a.col[2]);
            }
            i++;
        }
        printf("________________________________________________________________________________________________________________\n");
        fclose(KeplerData);
        fclose(ECIData);
        printf("\n%d tests total, one out of every %d shown.\n",i,z);
        avgerror = avgerror/i;
        printf("\nMaximum Error:__________________________________________________________________________________________\n");
        eci2kep(&Emaxerror, &keptest);
        printf("pos x:%11.5g|#|         Beta Angle |Period     |alat       |arg per    |E anom     |e          |flight ang\n",Emaxerror.s.col[0]);
        printf("    y:%11.5g|#|function:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g\n",Emaxerror.s.col[1],keptest.beta,keptest.period,keptest.alat,keptest.ap,keptest.ea,keptest.e,keptest.fa);
        printf("    z:%11.5g|#| correct:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g\n",Emaxerror.s.col[2],Kmaxerror.beta,Kmaxerror.period,Kmaxerror.alat,Kmaxerror.ap,Kmaxerror.ea,Kmaxerror.e,Kmaxerror.fa);
        printf("vel x:%11.5g|#|   delta:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g\n",Emaxerror.v.col[0],(keptest.beta-Kmaxerror.beta),(keptest.period-Kmaxerror.period),(keptest.alat-Kmaxerror.alat),(keptest.ap-Kmaxerror.ap),(keptest.ea-Kmaxerror.ea),(keptest.e-Kmaxerror.e),(keptest.fa-Kmaxerror.fa));
        printf("    y:%11.5g|#|         i          |mean anom  |mean motion|raan       |a          |true anomaly           \n",Emaxerror.v.col[1]);
        printf("    z:%11.5g|#|function:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g            \n",Emaxerror.v.col[2],keptest.i,keptest.ma,keptest.mm,keptest.raan,keptest.a,keptest.ta);
        printf("acc x:%11.5g|#| correct:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g            \n",Emaxerror.a.col[0],Kmaxerror.i,Kmaxerror.ma,Kmaxerror.mm,Kmaxerror.raan,Kmaxerror.a,Kmaxerror.ta);
        printf("    y:%11.5g|#|   delta:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g            \n",Emaxerror.a.col[1],(keptest.i-Kmaxerror.i),(keptest.ma-Kmaxerror.ma),(keptest.mm-Kmaxerror.mm),(keptest.raan-Kmaxerror.raan),(keptest.a-Kmaxerror.a),(keptest.ta-Kmaxerror.ta));
        printf("    z:%11.5g|#|\n",Emaxerror.a.col[2]);
        printf("\nMinimum Error:__________________________________________________________________________________________\n");
        eci2kep(&Eminerror, &keptest);
        printf("pos x:%11.5g|#|         Beta Angle |Period     |alat       |arg per    |E anom     |e          |flight ang\n",Eminerror.s.col[0]);
        printf("    y:%11.5g|#|function:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g\n",Eminerror.s.col[1],keptest.beta,keptest.period,keptest.alat,keptest.ap,keptest.ea,keptest.e,keptest.fa);
        printf("    z:%11.5g|#| correct:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g\n",Eminerror.s.col[2],Kminerror.beta,Kminerror.period,Kminerror.alat,Kminerror.ap,Kminerror.ea,Kminerror.e,Kminerror.fa);
        printf("vel x:%11.5g|#|   delta:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g\n",Eminerror.v.col[0],(keptest.beta-Kminerror.beta),(keptest.period-Kminerror.period),(keptest.alat-Kminerror.alat),(keptest.ap-Kminerror.ap),(keptest.ea-Kminerror.ea),(keptest.e-Kminerror.e),(keptest.fa-Kminerror.fa));
        printf("    y:%11.5g|#|         i          |mean anom  |mean motion|raan       |a          |true anomaly           \n",Eminerror.v.col[1]);
        printf("    z:%11.5g|#|function:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g            \n",Eminerror.v.col[2],keptest.i,keptest.ma,keptest.mm,keptest.raan,keptest.a,keptest.ta);
        printf("acc x:%11.5g|#| correct:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g            \n",Eminerror.a.col[0],Kminerror.i,Kminerror.ma,Kminerror.mm,Kminerror.raan,Kminerror.a,Kminerror.ta);
        printf("    y:%11.5g|#|   delta:%11.5g|%11.5g|%11.5g|%11.5g|%11.5g|%11.5g            \n",Eminerror.a.col[1],(keptest.i-Kminerror.i),(keptest.ma-Kminerror.ma),(keptest.mm-Kminerror.mm),(keptest.raan-Kminerror.raan),(keptest.a-Kminerror.a),(keptest.ta-Kminerror.ta));
        printf("    z:%11.5g|#|\n",Eminerror.a.col[2]);
    }
    if (jumpto!=-1) break;
    }
default:
    printf("\nconvertlib total error");
    if (skip!=-1) printf(" (without %s)",names[skip]); //indicate whether any function was skipped.
    printf(":\t%11.5g\n",terror); //final error stats for entire library.
}
}

int openFileLine(FILE **fp, const char filename[], int startLine) {
    char filePath[40] = {"convert_test_data/"};//the folder containing the test files
    *fp = fopen(strcat(filePath, filename), "r"); //open the file
    if (fp == NULL) { //print error message if necessary.
        printf("\n\tError opening necessary data file!!!\n");
        printf("Please check that the text file '%s' exists and\n", filename);
        printf("is in the directory titled 'convert_test_data'.\n\n");
        return(0); //return 0 for failure....
    }
    skipLines(*fp, startLine); //move the pointer to startLine
    return(1); //return 1 for sucess!!!!
}

void skipLines(FILE *fp, int lines) {
    char c;
    for (lines; lines>0; lines--) { //skip 'lines' many lines
        c=0;
        do { //scan through file untill the end of a line is reached
            c = fgetc(fp);
            if (c==EOF) return; //We've gone too far!!!
        } while(c!='\n');
    }
}
