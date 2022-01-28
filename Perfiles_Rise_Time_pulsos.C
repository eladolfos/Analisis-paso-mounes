// root script file
// Programa para calcular el perfiles del Rise time

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"

#include <cstdlib>
#include <iostream>

/* PGCode =11 Electron
   PGCode = 13 Muon
*/

void Perfiles_Rise_Time_pulse(const char* FileName)
{
    TFile *RFile = new TFile(FileName);
    if( !RFile )
    {
        std::cout<<"Can't open " << FileName << std::endl;
        return;
    }

    TTree *Config;
    TTree *Data;

    Float_t ADC_Bits;
    Float_t ADC_Vref;
    Float_t ADC_Sample_Rate;
    Float_t ADC_VSignal_Offset;
    Float_t ADC_Samples_per_Pulse;
    Float_t ADC_Trigger_Voltage;
    Float_t ADC_Pre_Trigger_Samples;

    Config = (TTree*)RFile->Get("ElecSim_info");
    Config->SetBranchAddress("ADC_Bits",            &ADC_Bits);
    Config->SetBranchAddress("ADC_Vref",            &ADC_Vref);
    Config->SetBranchAddress("ADC_Vin_Offset",      &ADC_VSignal_Offset);
    Config->SetBranchAddress("Sample_Rate",         &ADC_Sample_Rate);
    Config->SetBranchAddress("Trigger_Voltage",     &ADC_Trigger_Voltage);
    Config->SetBranchAddress("Samples_per_Pulse",   &ADC_Samples_per_Pulse);
    Config->SetBranchAddress("Pre_Trigger_Samples", &ADC_Pre_Trigger_Samples);

    Config->GetEntry(0);
	
	Int_t 		PDG_Code;
    Double_t 	Energy;
	Double_t 	Zenith_angle;
	Int_t 		Direction;
	Double_t 	Deposited_Energy;
	Double_t 	Track_Length;
	Int_t	 	Cherenkov_Photon_Count;
	Int_t	 	PMT_Photon_Count;

    std::vector< Double_t > *Event_Data = 0;

    Data = (TTree*)RFile->Get("ElecSim_Output");
    Data->SetBranchAddress("PDG_Code",                  &PDG_Code);
    Data->SetBranchAddress("Energy",                    &Energy);
	Data->SetBranchAddress("Zenith_angle",              &Zenith_angle);
	Data->SetBranchAddress("Direction",                 &Direction);
	Data->SetBranchAddress("Deposited_Energy",          &Deposited_Energy);
	Data->SetBranchAddress("Track_Length",              &Track_Length);
	Data->SetBranchAddress("Cherenkov_Photon_Count",    &Cherenkov_Photon_Count);
	Data->SetBranchAddress("PMT_Photon_Count",          &PMT_Photon_Count);
    Data->SetBranchAddress("Digitalized_Data",          &Event_Data);

    Long_t NEntries = Data->GetEntries();

auto *AmpvsRTe= new TProfile("AmpvsRT-e", " ", 100,0,7,0,16500); // Histograma muon
auto *AmpvsRTmu= new TProfile("AmpvsRT-mu", " ", 100,0,7,0,16500); //Histograma Electron

AmpvsRTe->GetXaxis()->SetTitle("Rise Time del pulso [ns]");
AmpvsRTe->GetYaxis()->SetTitle("Amplitud del pulso [A]");

AmpvsRTmu->GetXaxis()->SetTitle("Rise Time del pulso [ns]");
AmpvsRTmu->GetYaxis()->SetTitle("Amplitud del pulso [A]");


// Con esto saca los datos de todos los pulsos
Int_t EventNumber;
for(Int_t i=0; i<NEntries; i++)
 {
    EventNumber=i;
    Data->GetEntry(EventNumber);

    Double_t t_array[(Int_t)ADC_Samples_per_Pulse];
    Double_t *d_array = Event_Data->data();

    //std::cout<< "yes" << *d_array << std::endl;

    //Genera los puntos en x
    for(Int_t i=0; i<ADC_Samples_per_Pulse; i++ )
    {
        t_array[i] = i;
       // std::cout<< "i=" << i << 'y' << *d_array++ << std::endl;
    }



    Double_t Mean= TMath::Mean((Int_t)ADC_Pre_Trigger_Samples-1,d_array); //Base line

    Double_t Valuey_Min_P;
    // Minimo del pulso
    Int_t Min_P = TMath::LocMin((Int_t)ADC_Samples_per_Pulse, d_array);
 
    Valuey_Min_P=Event_Data->at(Min_P);


    Double_t Amplitud=0; //Amplitud del pulso
    Amplitud=Mean-Valuey_Min_P;

    Int_t ipxR=(Int_t)ADC_Pre_Trigger_Samples-1; //Indice del punto en ex justo donde empieza a subir el pulso que coincide con el valor en el punto
    Int_t ID=PDG_Code; //Codigo del pulso, 11 es un electron 13 es un muon

    //Calculo el RISE TIME de cada pulso
    Int_t RiseTime=0;

    for (Int_t i = ipxR; i <= Min_P; i++)
    {
        if ( d_array[i] <=(Mean-0.1*Amplitud) && d_array[i]>=  (Mean-0.9*Amplitud) ) //para hacer los calculos donde empieza y termina el rise time
        {
            RiseTime=RiseTime+(t_array[i+1]-t_array[i]); //Calculo el RISE TIME
            //std::cout << "Entro al ciclo del Rise-Time" << std::endl;
        }
        
    }

    
    
   
    
    if (ID==11) // Es un electron
    {
        AmpvsRTe->Fill(RiseTime,Amplitud);
    }
    else // Es un muon
    {
        AmpvsRTmu->Fill(RiseTime,Amplitud);
    }
    
}


    // Hago el fit
   // AmpvsRTe->Fit("pol1");

    //Creo la grafica 

    TCanvas *a0Canvas = new TCanvas();
    AmpvsRTe->Draw();
    a0Canvas->SaveAs("His_AmpvsRTelectrones_P.pdf");
    AmpvsRTe->Draw();
    a0Canvas->SaveAs("His_AmpvsRTmuones_P.pdf");
    //EDepvsMaxP->Draw();
    //a0Canvas->SaveAs("His_EDepvsMax_P.pdf");

}
