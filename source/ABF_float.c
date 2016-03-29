/****************************************************************
* FILENAME:     ABF_float.c
* DESCRIPTION:  Alfa-Beta filter
* AUTHOR:       David Kavreèiè
* START DATE:   16.1.2009
****************************************************************/

#include "ABF_float.h"


#pragma CODE_SECTION(ABF_float_calc, "ramfuncs");
void ABF_float_calc(ABF_float *v)
{

    // PREDIKCIJA
	v->ThetaOutK      = v->ThetaOutK_1 + (v->ThetaDotOutK_1 * v->Sampling_period);
	v->ThetaDotOutK   = v->ThetaDotOutK_1;

	// POPRAVEK KOTA KO PRESEZE 1
	if (v->ThetaOutK > 1.0)
	{
		v->ThetaOutK = v->ThetaOutK - 1.0;
	}
	else if (v->ThetaOutK < 0.0)
	{
		v->ThetaOutK = v->ThetaOutK + 1.0;
	}

	// NAPAKA
	v->ErrorThetaK    = v->ThetaInK - v->ThetaOutK;

	// POPRAVEK ERRORJA ZARADI KOTA
	if (v->ErrorThetaK  > 0.5)
	{
		v->ErrorThetaK  = v->ErrorThetaK  - 1.0;
	}
	else if (v->ErrorThetaK  < - 0.5)
	{
		v->ErrorThetaK  = v->ErrorThetaK  + 1.0;
	}

	// KOREKCIJA
	v->ThetaOutK      = v->ThetaOutK    + (v->Alpha * v->ErrorThetaK);
	v->ThetaDotOutK   = v->ThetaDotOutK + ((v->Beta  * v->ErrorThetaK) / v->Sampling_period);

	// POPRAVEK KOTA KO PRESEZE 1
	if (v->ThetaOutK > 1.0)
	{
		v->ThetaOutK = v->ThetaOutK - 1.0;
	}
	else if (v->ThetaOutK < 0.0)
	{
		v->ThetaOutK = v->ThetaOutK + 1.0;
	}

	// SHRANI VREDNOSTI ZA NASLEDNJO ITERACIJO
	v->ThetaOutK_1    = v->ThetaOutK;
	v->ThetaDotOutK_1 = v->ThetaDotOutK;

}
