#include <advanlys.h>
#include <utility.h>
#include <formatio.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "MainProiect.h"

#define SAMPLE_RATE		0
#define NPOINTS			1

// Global variables
int waveInfo[2]; //waveInfo[0] = sampleRate
				 //waveInfo[1] = number of elements
double sampleRate = 0.0;
int npoints = 0;
double *waveData = 0;
double *filter = 0;
double *derivative = 0;
double *envelope = 0;
double *aux = 0;
double *Filter_2 = 0;
double *bssl1 = 0;
double *bw1 =0;
double *autoSpectrum = 0;
double *convertedSpectrum = 0;

double min, max;
double mean, dispersion, median;
int min_index, max_index;
int zero_crossing = 0;
int Type = 0;
int next = 0, prev = 0;
int winN = 0;
//numar de puncte (N = 2048 sau 4096 sau 8192 sau 16384)
static unsigned int nOfPoints = 2048;
int GraphType = 0;

double df=0.0; //pasul in domeniul frecventei
double freqPeak=0.0; //valoarea maxima din spectrul de putere
double powerPeak=0.0; //frecventa estimata pentru spectrum de putere
char unit[32]="V";  //voltage semnal
	
int histogram[100];
double axis[100];
	
static int panelHandle = 0;
static int panelFreq = 0;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "MainProiect.uir", PANEL)) < 0)
		return -1;
	if((panelFreq = LoadPanel(0, "MainProiect.uir", PANEL_2)) < 0)
		return -1;
	
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK OnPanel (int panel, int event, void *callbackData,
						 int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);
			
			break;
	}
	return 0;
}

int CVICALLBACK OnPanel_2 (int panel, int event, void *callbackData,
						   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface (0);

			break;
	}
	return 0;
}

double* FiltruDeMediere(int nSamples)
{
	int sum = 0;
	double *Filter1;
	Filter1 = (double *) calloc(npoints, sizeof(double));
	
	for(int i = 0; i < npoints; i++)
	{
		sum = 0;
		for(int k = 0; k <= nSamples - 1; k++)
		{
			if(i - k < 0)
				sum = 0;
			else
				sum += waveData[i - k];
		}
		Filter1[i] = sum / nSamples;
	}
	
	return Filter1;
}

double* FiltruAlpha(double alpha)	//functie pentru filtru de ordin 1
{
	double *Filter;
	Filter = (double *) calloc(npoints, sizeof(double));
	
	for(int i = 1; i < npoints; i++)
		Filter[i] = (1 - alpha)*Filter[i - 1] + alpha * waveData[i];
	
	return Filter;
}

int CVICALLBACK OnLoad (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
	{
		case EVENT_COMMIT:
			//executa script python pentru conversia unui fisierului .wav in .txt
			LaunchExecutable("python main.py");
			
			//astept sa fie generate cele doua fisiere (modificati timpul daca este necesar
			//Delay(4);
			
			//incarc informatiile privind rata de esantionare si numarul de valori
			FileToArray("wafeInfo.txt", waveInfo, VAL_INTEGER, 2, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			sampleRate = waveInfo[SAMPLE_RATE];
			npoints = waveInfo[NPOINTS];
			
			//alocare memorie pentru numarul de puncte
			waveData = (double *) calloc(npoints, sizeof(double));
			
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			//afisare pe grapf
			PlotY(panel, PANEL_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
			
			//calcul maxim, minim, max_index si min_index
			MaxMin1D(waveData, npoints, &max, &max_index, &min, &min_index);
			
			SetCtrlVal(panel, PANEL_NUMERIC, min);
			SetCtrlVal(panel, PANEL_NUMERIC_2, max);
			SetCtrlVal(panel, PANEL_NUMERIC_3, min_index);
			SetCtrlVal(panel, PANEL_NUMERIC_4, max_index);
			
			//Calcul dispersie si medie
			StdDev(waveData, npoints, &mean, &dispersion);
			
			SetCtrlVal(panel, PANEL_NUMERIC_5, dispersion);
			SetCtrlVal(panel, PANEL_NUMERIC_6, mean);
			
			//calcul mediana
			Median(waveData, npoints, &median);
			
			SetCtrlVal(panel, PANEL_NUMERIC_7, median);
			
			//calcul zero crossing
			for(int i = 0; i < npoints; i++)
				if((waveData[i] > 0 && waveData[i+1] < 0) || (waveData[i] < 0 && waveData[i+1] > 0) || waveData[i] == 0)
					zero_crossing ++;
			
			SetCtrlVal(panel, PANEL_NUMERIC_8, zero_crossing);
			
			//Histograma
			Histogram(waveData, npoints, min, max, histogram, axis, 10);
			//afisare histograma
			PlotXY(panel, PANEL_GRAPH_3, axis, histogram, 10, VAL_DOUBLE, VAL_SSIZE_T, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);
			break;
			
	}
	return 0;
}

int CVICALLBACK On_Next (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(next >= 0 && next < 5)
			{
				DeleteGraphPlot(panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, PANEL_GRAPH_2, filter + (int)(next*sampleRate), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				if(next == 0)
					SetCtrlVal(panel, PANEL_NUMERIC_9, 0);
				else
				{
					SetCtrlVal(panel, PANEL_NUMERIC_10, next);
					SetCtrlVal(panel, PANEL_NUMERIC_9, prev);
				}
				next++;
				prev = next - 1;
			}else
				if(next == 5)
				{
					DeleteGraphPlot(panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, PANEL_GRAPH_2, filter + (int)(next* sampleRate), npoints - next*sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					
					SetCtrlVal(panel, PANEL_NUMERIC_10, next);
					SetCtrlVal(panel, PANEL_NUMERIC_9, prev);
					next++;
					prev = next - 1;
				}
			break;
	}
	return 0;
}

int CVICALLBACK On_Prev (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(prev >= 1 && prev <= 5)
			{
				DeleteGraphPlot(panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				PlotY(panel, PANEL_GRAPH_2, filter + (int)((prev-1)*sampleRate), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				SetCtrlVal(panel, PANEL_NUMERIC_10, next-1);
				SetCtrlVal(panel, PANEL_NUMERIC_9, prev-1);
				prev--;
				next--;
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnFilter (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	double alpha;

	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, PANEL_RING, &Type);
			//alocare memorie pentru numarul de puncte din semnalul filtrat
			filter = (double *) calloc(npoints, sizeof(double));
			
			switch (Type)
			{
				case 0:	//pentru none
					
					break;
				case 1:
					//filtru de mediere pe 16 esantioane
					filter = FiltruDeMediere(16);
					
					break;
				case 2:
					//filtru de mediere pe 32 de esantioane
					filter = FiltruDeMediere(32);
					
					break;
				case 3:
					//atribuire valoare pentru alpha
					GetCtrlVal(panel, PANEL_NUMERIC_11, &alpha);
					
					//filtru ce element de ordin 1
					filter = FiltruAlpha(alpha);
					
					break;
			}
			
			break;
	}
	
	return 0;
}

int CVICALLBACK OnApply (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(Type == 0)	//pentru None 
			{
				DeleteGraphPlot(panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				
				//resetarea pozitiei
				next = 0;
				prev = 0;
				SetCtrlVal(panel, PANEL_NUMERIC_10, next);
				SetCtrlVal(panel, PANEL_NUMERIC_9, prev);
			}
			else	//pentru filtre
			{
				DeleteGraphPlot(panel, PANEL_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
				
				//afisare pe graf a semnalului filtrat
				PlotY(panel, PANEL_GRAPH_2, filter, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
				
				//resetarea pozitiei
				next = 0;
				prev = 0;
				SetCtrlVal(panel, PANEL_NUMERIC_10, next);
				SetCtrlVal(panel, PANEL_NUMERIC_9, prev);
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnDerivative (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT: 
			//derivata semnalului
			derivative = (double *) calloc(npoints, sizeof(double));
			Difference(filter, npoints, 1.0, 0.0, 0.0, derivative);
			
			//afisarea derivatei pentru intregul semnal filtrat
			if(next == 0 && prev == 0)
				PlotY(panel, PANEL_GRAPH_2, derivative, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLACK);
			
			//afisare, pe esantioane, a derivatei semnalului filtrat
			if(next-1 >= 0 && next-1 < 5)
			{
				PlotY(panel, PANEL_GRAPH_2, derivative + (int)((next-1)*sampleRate), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLACK);
			}else
				if(next-1 == 5)
				{
					PlotY(panel, PANEL_GRAPH_2, derivative + (int)((next-1)*sampleRate), npoints - (next-1)*sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLACK);
				}
			
			break;
	}
	return 0;
}

int CVICALLBACK OnEnvelope (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//alocare memorie pentru anvelopa
			envelope = (double *) calloc(npoints, sizeof(double));
			
			//incarcare din fisierul .txt in memorie (vector)
			FileToArray("anvelopa.txt", envelope, VAL_DOUBLE, npoints, 1, VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
			
			//afisare pe grapf
			PlotY(panel, PANEL_GRAPH, envelope, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_CIRCLE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_BLUE);
			
			break;
	}
	return 0;
}

int CVICALLBACK OnSwitchPanel (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel == panelHandle)
			{
				SetCtrlVal(panelFreq, PANEL_2_BINARYSWITCH, 1);
				DisplayPanel(panelFreq);
				HidePanel(panel);
				
				next = 0;
				prev = 0;
				SetCtrlVal(panel, PANEL_2_NUMERIC, next);
				SetCtrlVal(panel, PANEL_2_NUMERIC_2, prev);
			}
			else
			{
				SetCtrlVal(panelHandle, PANEL_BINARYSWITCH, 0);
				DisplayPanel(panelHandle);
				HidePanel(panel);
				
				next = 0;
				prev = 0;
				SetCtrlVal(panel, PANEL_NUMERIC_10, next);
				SetCtrlVal(panel, PANEL_NUMERIC_9, prev);
			}

			break;
	}
	return 0;
}

int CVICALLBACK OnWindowType (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	int winType;
	
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, PANEL_2_BINARYSWITCH_2, &winType);
			
			GetCtrlVal(panel, PANEL_2_RING_3, &nOfPoints);
			//afisam pe interfata numarul de ferestre necesare reprezentarii intregului spectru
			SetCtrlVal(panel, PANEL_2_NUMBER_OF_WINDOWS, ((npoints/6)/nOfPoints)+1);
			
			
			//alocare memorie pentru numarul de puncte
			autoSpectrum=(double*)calloc(nOfPoints/2,sizeof(double));//spectrul de putere
			convertedSpectrum=(double*)calloc(nOfPoints/2,sizeof(double));//vectorul utilizat pentru reprezentarea spectrului
						

			WindowConst winConst;
			
			aux = (double *) calloc(sampleRate, sizeof(double));
			
			switch (GraphType)
			{
				case 0:	//None
					break;
				case 2:	//Raw Signal
					if(winN >= 0 && winN < 5)
					{
						Copy1D(waveData + (int)(winN*sampleRate), sampleRate, aux);
						if(winType == 1)
						{ 
							ScaledWindowEx(aux, sampleRate, FLATTOP, 0.0, &winConst);
							DeleteGraphPlot(panel, PANEL_2_GRAPH, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, PANEL_2_GRAPH, aux, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
							
						}
						else
						{
							ScaledWindowEx(aux, sampleRate, TRIANGLE, 0.0, &winConst);
							DeleteGraphPlot(panel, PANEL_2_GRAPH, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, PANEL_2_GRAPH, aux, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
							
						}
						
						double *Seconds_f;
						Seconds_f = (double*)calloc(nOfPoints,sizeof(double));
							
						int timer_f = 0;
						
						while(timer_f < npoints/6)
						{
							if(timer_f + nOfPoints <= npoints/6)	
							{
								
								for(int i=0; i < nOfPoints; ++i)
								{
									Seconds_f[i] = waveData[i+timer_f];//copiez in Seconds o fereastra de puncte din temp
									
								}
								
							}
							else
							{
								for(int i=0; i < npoints/6-timer_f; ++i)
								{
									Seconds_f[i] = waveData[i+timer_f];
								}
								for(int i=npoints/6-timer_f; i<nOfPoints; ++i)
								{
									Seconds_f[i]=0;
								}
							}
							char unit[32]="V";  //voltage semnal
							
							
							
							if(winType == 1)	//Flat Top Window
							{ 
								ScaledWindowEx(Seconds_f, nOfPoints, FLATTOP, 0, &winConst);	
							}
							else	//Triangle Window
							{
								ScaledWindowEx(Seconds_f, nOfPoints, TRIANGLE, 0, &winConst);
							}
							
								//partea pozitiva a spectrului scalat de putere pentru un semnal esantionat
							AutoPowerSpectrum(Seconds_f, nOfPoints, 1.0/sampleRate, autoSpectrum, &df);
								//calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului  
							PowerFrequencyEstimate(autoSpectrum, nOfPoints/2, -1.0, winConst, df, 7, &freqPeak, &powerPeak);
							
								//Functia converteste spectrul de intrare (care este puterea, amplitudinea sau amplificarea) 
								//în formate alternative (linear, logarithmic, dB) ce permit o reprezentare grafica mai convenabila.
							SpectrumUnitConversion (autoSpectrum, nOfPoints/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, convertedSpectrum, unit);
								//afisez pe interfata valorile determinate
							
								//plotez spectrul semnalului
					    	PlotWaveform( panel, PANEL_2_GRAPH_4, convertedSpectrum, nOfPoints/2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_DK_GREEN);
						
							
							SetCtrlVal( panel, PANEL_2_FREQUENCY_PEAK, freqPeak);
							SetCtrlVal( panel, PANEL_2_POWER_PEAK, powerPeak);
						    
							//afisam pe interfata numarul ferestrei curente
							SetCtrlVal(panel, PANEL_2_CURENT_WINDOW, (timer_f/nOfPoints)+1);
							
							timer_f += nOfPoints;
							
							Delay(1);
							//DeleteGraphPlot (panel, PANEL_2_GRAPH_4, -1, VAL_IMMEDIATE_DRAW);
							
						}
						realloc(Seconds_f, 0);
						realloc(convertedSpectrum, 0);
						realloc(autoSpectrum, 0);
					}
					else
					{
						Copy1D(waveData + (int)(winN*sampleRate), npoints - winN*sampleRate, aux);
						if(winType == 1)
						{ 
							ScaledWindowEx(aux, npoints - winN*sampleRate, FLATTOP, 0.0, &winConst);
							DeleteGraphPlot(panel, PANEL_2_GRAPH, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, PANEL_2_GRAPH, aux, npoints - winN*sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
							
						}
						else
						{
							ScaledWindowEx(aux, npoints - winN*sampleRate, TRIANGLE, 0.0, &winConst);
							DeleteGraphPlot(panel, PANEL_2_GRAPH, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, PANEL_2_GRAPH, aux, npoints - winN*sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
							
						}
						
						double *Seconds_f;
						Seconds_f = (double*)calloc(nOfPoints,sizeof(double));
							
						int timer_f = 0;
						
						while(timer_f < npoints/6)
						{
							if(timer_f + nOfPoints <= npoints/6)	
							{
								
								for(int i=0; i < nOfPoints; ++i)
								{
									Seconds_f[i] = (waveData + (int)(winN*sampleRate))[i+timer_f];//copiez in Seconds o fereastra de puncte din temp
									
								}
								
							}
							else
							{
								for(int i=npoints/6-timer_f; i<nOfPoints; ++i)
								{
									Seconds_f[i]=0;
								}
							}
							char unit[32]="V";  //voltage semnal
							
							
							
							if(winType == 1)	//Flat Top Window
							{ 
								ScaledWindowEx(Seconds_f, nOfPoints, FLATTOP, 0, &winConst);	
							}
							else	//Triangle Window
							{
								ScaledWindowEx(Seconds_f, nOfPoints, TRIANGLE, 0, &winConst);
							}
							
								//partea pozitiva a spectrului scalat de putere pentru un semnal esantionat
							AutoPowerSpectrum(Seconds_f, nOfPoints, 1.0/sampleRate, autoSpectrum, &df);
								//calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului  
							PowerFrequencyEstimate(autoSpectrum, nOfPoints/2, -1.0, winConst, df, 7, &freqPeak, &powerPeak);
							
								//Functia converteste spectrul de intrare (care este puterea, amplitudinea sau amplificarea) 
								//în formate alternative (linear, logarithmic, dB) ce permit o reprezentare grafica mai convenabila.
							SpectrumUnitConversion (autoSpectrum, nOfPoints/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, convertedSpectrum, unit);
								//afisez pe interfata valorile determinate
							
								//plotez spectrul semnalului
					    	PlotWaveform( panel, PANEL_2_GRAPH_4, convertedSpectrum, nOfPoints/2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_DK_GREEN);
						
							
							SetCtrlVal( panel, PANEL_2_FREQUENCY_PEAK, freqPeak);
							SetCtrlVal( panel, PANEL_2_POWER_PEAK, powerPeak);
						    
							//afisam pe interfata numarul ferestrei curente
							SetCtrlVal(panel, PANEL_2_CURENT_WINDOW, (timer_f/nOfPoints)+1);
							
							timer_f += nOfPoints;
							
							Delay(1);
							DeleteGraphPlot (panel, PANEL_2_GRAPH_4, -1, VAL_IMMEDIATE_DRAW);
							
						}
						realloc(Seconds_f, 0);
						realloc(convertedSpectrum, 0);
						realloc(autoSpectrum, 0);
					}
					
					break;
				default:
					double *aux1=0;
					aux1 = (double*)calloc(sampleRate,sizeof(double));
					
					
					if(GraphType == 3)	//Bessel Filtered Signal
					{
						Copy1D(bssl1, sampleRate, aux);
						if(winType == 1)	//Flat Top Window
						{ 
							ScaledWindowEx(aux, sampleRate, FLATTOP, 0.0, &winConst);
							DeleteGraphPlot(panel, PANEL_2_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, PANEL_2_GRAPH_2, aux, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
								
						}
						else	//Triangle Window
						{
							ScaledWindowEx(aux, sampleRate, TRIANGLE, 0.0, &winConst);
							DeleteGraphPlot(panel, PANEL_2_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, PANEL_2_GRAPH_2, aux, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
								
						}
					}
					else if(GraphType == 4)	//Butterworth Filtered Signal
					{
						Copy1D(bw1, sampleRate, aux);
						if(winType == 1)	//Flat Top Window
						{ 
							ScaledWindowEx(aux, sampleRate, FLATTOP, 0.0, &winConst);
							DeleteGraphPlot(panel, PANEL_2_GRAPH_3, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, PANEL_2_GRAPH_3, aux, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
								
						}
						else	//Triangle Window
						{
							ScaledWindowEx(aux, sampleRate, TRIANGLE, 0.0, &winConst);
							DeleteGraphPlot(panel, PANEL_2_GRAPH_3, -1, VAL_IMMEDIATE_DRAW);
							PlotY(panel, PANEL_2_GRAPH_3, aux, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
								
						}
					}
					
					
					
					if(GraphType == 3)	//Bessel Filtered Signal
					{
						Copy1D(bssl1, sampleRate, aux1);
					}
					else if(GraphType == 4)	//Butterworth Filtered Signal
					{
						Copy1D(bw1, sampleRate, aux1);
					}
					
					double *Seconds_f;
					Seconds_f = (double*)calloc(nOfPoints,sizeof(double));
						
					int timer_f = 0;
					
					while(timer_f < npoints/6)
					{
						if(timer_f + nOfPoints <= npoints/6)	
						{
							
							for(int i=0; i < nOfPoints; ++i)
							{
								Seconds_f[i] = aux1[i+timer_f];//copiez in Seconds o fereastra de puncte din temp
								
							}
							
						}
						else
						{
							for(int i=0; i < npoints/6-timer_f; ++i)
							{
								Seconds_f[i] = aux1[i+timer_f];
							}
							for(int i=npoints/6-timer_f; i<nOfPoints; ++i)
							{
								Seconds_f[i]=0;
							}
						}
						char unit[32]="V";  //voltage semnal
						
						
						
						if(winType == 1)	//Flat Top Window
						{ 
							ScaledWindowEx(Seconds_f, nOfPoints, FLATTOP, 0, &winConst);	
						}
						else	//Triangle Window
						{
							ScaledWindowEx(Seconds_f, nOfPoints, TRIANGLE, 0, &winConst);
						}
						
							//partea pozitiva a spectrului scalat de putere pentru un semnal esantionat
						AutoPowerSpectrum(Seconds_f, nOfPoints, 1.0/sampleRate, autoSpectrum, &df);
							//calculeaza puterea si frecventa corespunzatoare varfului din spectrul semnalului  
						PowerFrequencyEstimate(autoSpectrum, nOfPoints/2, -1.0, winConst, df, 7, &freqPeak, &powerPeak);
						
							//Functia converteste spectrul de intrare (care este puterea, amplitudinea sau amplificarea) 
							//în formate alternative (linear, logarithmic, dB) ce permit o reprezentare grafica mai convenabila.
						SpectrumUnitConversion (autoSpectrum, nOfPoints/2, 0, SCALING_MODE_LINEAR, DISPLAY_UNIT_VPK, df, winConst, convertedSpectrum, unit);
							//afisez pe interfata valorile determinate
							
						if(GraphType == 3)	//Bessel Filtered Signal
						{
							
							//plotez spectrul semnalului
					    	PlotWaveform( panel, PANEL_2_GRAPH_5, convertedSpectrum, nOfPoints/2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_DK_GREEN);
							
						}
						else if(GraphType == 4)	//Butterworth Filtered Signal
						{
							//plotez spectrul semnalului
				    		PlotWaveform( panel, PANEL_2_GRAPH_6, convertedSpectrum, nOfPoints/2 ,VAL_DOUBLE, 1.0, 0.0, 0.0, df, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,  VAL_CONNECTED_POINTS, VAL_DK_GREEN);
							
						}	
						SetCtrlVal( panel, PANEL_2_FREQUENCY_PEAK, freqPeak);
						SetCtrlVal( panel, PANEL_2_POWER_PEAK, powerPeak);
					    
						//afisam pe interfata numarul ferestrei curente
						SetCtrlVal(panel, PANEL_2_CURENT_WINDOW, (timer_f/nOfPoints)+1);
						
						timer_f += nOfPoints;
						
						Delay(1);
						//DeleteGraphPlot (panel, PANEL_2_GRAPH_6, -1, VAL_IMMEDIATE_DRAW);
						//DeleteGraphPlot (panel, PANEL_2_GRAPH_5, -1, VAL_IMMEDIATE_DRAW);
						
					}
					realloc(Seconds_f, 0);
					realloc(convertedSpectrum, 0);
					realloc(autoSpectrum, 0);
					break;
				
			}
			break;
	}
	return 0;
}

int CVICALLBACK OnPrevButton (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			switch (GraphType)
			{
				case 2:	//Raw Signal
					if(prev >= 1 && prev <= 5)
					{
						DeleteGraphPlot(panel, PANEL_2_GRAPH, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH, waveData + (int)((prev-1)*sampleRate), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						SetCtrlVal(panel, PANEL_2_NUMERIC_2, next-1);
						SetCtrlVal(panel, PANEL_2_NUMERIC, prev-1);
						
						winN = next-2;
						
						prev--;
						next--;
					}
					break;
				case 3:	//Bessel Filtered Signal
					if(prev >= 1 && prev <= 5)
					{
						Bssl_LPF(waveData + (int)((prev-1)*sampleRate), sampleRate, sampleRate, sampleRate/6, 4, bssl1);
					
						DeleteGraphPlot(panel, PANEL_2_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH_2, bssl1, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						SetCtrlVal(panel, PANEL_2_NUMERIC_2, next-1);
						SetCtrlVal(panel, PANEL_2_NUMERIC, prev-1);
						
						prev--;
						next--;
					}
					break;
				case 4:	//Butterworth Filtered Signal
					if(prev >= 1 && prev <= 5)
					{
						Bw_LPF(waveData + (int)((prev-1)*sampleRate), sampleRate, sampleRate, sampleRate/6, 4, bw1);
					
						DeleteGraphPlot(panel, PANEL_2_GRAPH_3, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH_3, bw1, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						SetCtrlVal(panel, PANEL_2_NUMERIC_2, next-1);
						SetCtrlVal(panel, PANEL_2_NUMERIC, prev-1);
						
						prev--;
						next--;
					}
					break;
				default:	//None
					break;
			}

			break;
	}
	return 0;
}

int CVICALLBACK OnNextButton (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch (GraphType)
			{
				case 2:	//Raw Signal
					if(next >= 0 && next < 5)
					{
						DeleteGraphPlot(panel, PANEL_2_GRAPH, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH, waveData + (int)(next*sampleRate), sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						if(next == 0)
							SetCtrlVal(panel, PANEL_2_NUMERIC, 0);
						else
						{
							SetCtrlVal(panel, PANEL_2_NUMERIC_2, next);
							SetCtrlVal(panel, PANEL_2_NUMERIC, prev);
						}
						winN = next;
						
						next++;
						prev = next - 1;
					}else
					if(next == 5)
					{
						DeleteGraphPlot(panel, PANEL_2_GRAPH, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH, waveData + (int)(next* sampleRate), npoints - next*sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						SetCtrlVal(panel, PANEL_2_NUMERIC_2, next);
						SetCtrlVal(panel, PANEL_2_NUMERIC, prev);
						
						winN = next;
						
						next++;
						prev = next - 1;
					}
					break;
				case 3:	//Bessel Filtered Signal
					if(next >= 0 && next < 5)
					{
						Bssl_LPF(waveData + (int)(next*sampleRate), sampleRate, sampleRate, sampleRate/6, 4, bssl1);
					
						DeleteGraphPlot(panel, PANEL_2_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH_2, bssl1, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						if(next == 0)
							SetCtrlVal(panel, PANEL_2_NUMERIC, 0);
						else
						{
							SetCtrlVal(panel, PANEL_2_NUMERIC_2, next);
							SetCtrlVal(panel, PANEL_2_NUMERIC, prev);
						}
						
						winN = next;
						
						next++;
						prev = next - 1;
					}else
					if(next == 5)
					{
						Bssl_LPF(waveData + (int)(next*(npoints - next*sampleRate)), sampleRate, sampleRate, sampleRate/6, 4, bssl1);
					
						DeleteGraphPlot(panel, PANEL_2_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH_2, bssl1, npoints - next*sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						SetCtrlVal(panel, PANEL_2_NUMERIC_2, next);
						SetCtrlVal(panel, PANEL_2_NUMERIC, prev);
						
						winN = next;
						
						next++;
						prev = next - 1;
					}
					break;
				case 4:	//Butterworth Filtered Signal
					if(next >= 0 && next < 5)
					{
						Bw_LPF(waveData + (int)(next*sampleRate), sampleRate, sampleRate, sampleRate/6, 4, bw1);
					
						DeleteGraphPlot(panel, PANEL_2_GRAPH_3, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH_3, bw1, sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						if(next == 0)
							SetCtrlVal(panel, PANEL_2_NUMERIC, 0);
						else
						{
							SetCtrlVal(panel, PANEL_2_NUMERIC_2, next);
							SetCtrlVal(panel, PANEL_2_NUMERIC, prev);
						}
						
						winN = next;
						
						next++;
						prev = next - 1;
					}else
					if(next == 5)
					{
						Bw_LPF(waveData + (int)(next*(npoints - next*sampleRate)), sampleRate, sampleRate, sampleRate/6, 4, bw1);
					
						DeleteGraphPlot(panel, PANEL_2_GRAPH_3, -1, VAL_IMMEDIATE_DRAW);
						PlotY(panel, PANEL_2_GRAPH_3, bw1, npoints - next*sampleRate, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
						
						SetCtrlVal(panel, PANEL_2_NUMERIC_2, next);
						SetCtrlVal(panel, PANEL_2_NUMERIC, prev);
						
						winN = next;
						
						next++;
						prev = next - 1;
					}
					break;
				default:	//None
					break;
			}

			break;
	}
	return 0;
}

int CVICALLBACK OnSelectGraph (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel, PANEL_2_RING_2, &GraphType);
			
			Filter_2 = (double *) calloc(npoints, sizeof(double));
			
			switch (GraphType)
			{
				
				case 2:	//Raw Signal
					DeleteGraphPlot(panel, PANEL_2_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, PANEL_2_GRAPH, waveData, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					
					break; 
				case 3: //Bessel Filtered Signal
					Bssl_LPF(waveData, npoints, sampleRate, sampleRate/6, 4, Filter_2);
					
					DeleteGraphPlot(panel, PANEL_2_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, PANEL_2_GRAPH_2, Filter_2, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					
					bssl1 = (double *) calloc(sampleRate, sizeof(double));
					
					break;
				case 4: //Butterworth Filtered Signal
					Bw_LPF(waveData, npoints, sampleRate, sampleRate/6, 6, Filter_2);
						
					DeleteGraphPlot(panel, PANEL_2_GRAPH_3, -1, VAL_IMMEDIATE_DRAW);
					PlotY(panel, PANEL_2_GRAPH_3, Filter_2, npoints, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
					
					bw1 = (double *) calloc(sampleRate, sizeof(double));
			
					break;
				default:	//None
					DeleteGraphPlot(panel, PANEL_2_GRAPH, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(panel, PANEL_2_GRAPH_2, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(panel, PANEL_2_GRAPH_3, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(panel, PANEL_2_GRAPH_4, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(panel, PANEL_2_GRAPH_5, -1, VAL_IMMEDIATE_DRAW);
					DeleteGraphPlot(panel, PANEL_2_GRAPH_6, -1, VAL_IMMEDIATE_DRAW);
					
					break;
			}
			next = 0;
			prev = 0;
			SetCtrlVal(panel, PANEL_2_NUMERIC, next);
			SetCtrlVal(panel, PANEL_2_NUMERIC_2, prev);
			break;
	}
	return 0;
}

int CVICALLBACK OnSave (int panel, int control, int event,
						void *callbackData, int eventData1, int eventData2)
{
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int second = 0;
	int imghandle;  
	char fileName[256] = {0}, fileName1[256]={0}, fileName2[256]={0}, fileName3[256]={0}, fileName4[256]={0};
	switch (event)
	{
		case EVENT_COMMIT:
			//luam data si ora
			GetSystemDate(&month, &day, &year);
			GetSystemTime(&hour, &minute, &second);
			
			//setam numele cu care salvam imaginea
			sprintf(fileName, "ROW_DATA_%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imaginea
			GetCtrlDisplayBitmap(panel,PANEL_GRAPH,1,&imghandle);
			SaveBitmapToJPEGFile(imghandle,fileName,JPEG_PROGRESSIVE,100);
			
			
			//setam numele cu care salvam imaginea
			sprintf(fileName1, "FILTERED_DATA__%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imagine
			GetCtrlDisplayBitmap(panel,PANEL_GRAPH_2,1,&imghandle);
			SaveBitmapToJPEGFile(imghandle,fileName1,JPEG_PROGRESSIVE,100);
			
			//setam numele cu care salvam imaginea
			sprintf(fileName2, "WINDOWING_RAW_SIGNAL_%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imaginea
			GetCtrlDisplayBitmap(panelFreq,PANEL_2_GRAPH,1,&imghandle);
			SaveBitmapToJPEGFile(imghandle,fileName2,JPEG_PROGRESSIVE,100);
			
			//setam numele cu care salvam imaginea
			sprintf(fileName3, "BESSEL_4_%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imaginea
			GetCtrlDisplayBitmap(panelFreq,PANEL_2_GRAPH_2,1,&imghandle);
			SaveBitmapToJPEGFile(imghandle,fileName3,JPEG_PROGRESSIVE,100);
			
			//setam numele cu care salvam imaginea
			sprintf(fileName4, "BUTTERWORTH_6_%4d.%2d.%2d_%2d-%2d-%2d.jpg", year, month, day, hour, minute, second);
			//salvam imaginea
			GetCtrlDisplayBitmap(panelFreq,PANEL_2_GRAPH_3,1,&imghandle);
			SaveBitmapToJPEGFile(imghandle,fileName4,JPEG_PROGRESSIVE,100);
			
		
			DiscardBitmap(imghandle);
			
			break;
	}
	return 0;
}
