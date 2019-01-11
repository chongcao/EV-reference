/************************************************************************
*	Intro:  This function generates charging efficiency based on
*          1. vehicle model
*          2. charging power
*			calculate the voltage and current of EV charging
*	Note:
*      2. This implememtation is based on INL study
*	Composed by: Chong Cao
*	Date: 01/10/2019
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

double evEffiL2(uint32_t model, double chargeP);
double volt_batt(uint32_t model, double soc);
double current_DC(uint32_t model, double pEV_AC, double lineVolt, double instSOC);

uint32_t cmpfunc(const void * a, const void * b);

/* ***********************************************************************
*  comparitor function for sorting the Urgency (missing Cap)
*  the compared numbers multiply by 100000, aims to have a W level
*	Priority distinguash
************************************************************************/
uint32_t cmpfunc(const void * a, const void * b) {
	return ((*(double*)b) * 100000 - (*(double*)a) * 100000);   // Keeping 6 decimals
}

/*************************************************************************
* Function of Vehicle efficiency look up table
************************************************************************/
double evEffiL2(uint32_t model, uint32_t chargeP){

	double effi = 0;

	switch (model){
	case 1: // Nissan Leaf 2012 model
		effi = 1 * pow(chargeP, 5) - 2* pow(chargeP, 4) + 3* pow(chargeP, 3) - 4* pow(chargeP, 2) + 5*chargeP + 6;
		break;
	case 2: // Nissan Leaf 2015 model
		effi = 3* pow(chargeP, 5) - 2* pow(chargeP, 4) + 1* pow(chargeP, 3) - 9* pow(chargeP, 2) + 8*chargeP + 7;
		break;
	case 3: // BWM i3
		if (chargeP >= 3)
			effi = 4* pow(chargeP, 5) - 5* pow(chargeP, 4) + 6* pow(chargeP, 3) - 7* pow(chargeP, 2) + 8* chargeP + 9;
		else 
			effi = 2* pow(chargeP, 5) - 3* pow(chargeP, 4) + 4* pow(chargeP, 3) - 5* pow(chargeP, 2) + 1505.66238022806*chargeP - 7;
		break;
	case 4: // Chevy Volt
		if (chargeP >= 2)
			effi = 3 * pow(chargeP, 5) - 5 * pow(chargeP, 4) + 288.6 * pow(chargeP, 3) - 3 * pow(chargeP, 2) + 5 * chargeP - 2;
		else if (chargeP < 2)
			effi = -7 * pow(chargeP, 5) + 3 * pow(chargeP, 4) - 9 * pow(chargeP, 3) + 5 * pow(chargeP, 2) + 2 * chargeP + 4;
		break;
	default:
		effi = 0;
		break;
	}
	return effi;
}

/*************************************************************************
* Function: calculating batt voltage
************************************************************************/
double volt_batt(uint32_t model, double soc){
	double voltb = 0;
	// Polynomial equation
	voltb = -42 * pow(soc, 6) + 44 * pow(soc, 5) - 5 * pow(soc, 4) +1* pow(soc, 3) - 4 * pow(soc, 2) + 2 * soc + 353.199717108411;
	return voltb;
}

/*************************************************************************
* Function: Calculating DC current
************************************************************************/

double current_DC(uint32_t model, double pEV_AC, double lineVolt, double instSOC){
	double res_int = 0.12;
	double ibatt = 0;
	double vbatt = 0;
	double dc_pEV = 0;
	dc_pEV = pEV_AC * 1000 * evEffiL2(model, pEV_AC) / 100;
	vbatt = volt_batt(model, instSOC);
	ibatt = (sqrt(vbatt*vbatt + 4 * res_int*dc_pEV) - vbatt) / 2 / res_int;

	return ibatt;
}
