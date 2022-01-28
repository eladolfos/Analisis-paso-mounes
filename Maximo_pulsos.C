// root script file
/* Programa para extraer los datos y realizar histogramas
    Elser Lopez, 23/05/2020
    Realiza el histograma del Minimo  del pulso y energia depositada
*/

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"

#include <cstdlib>
#include <iostream>

void EnergyDe_MaxPulse(const char* FileName)
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

    //if(EventNumber >= NEntries)
    //{
      //  std::cout<< "Invalid event number" << std::endl;
      //  return;
    //}



//Double_t Area_por_pulso[(Int_t)NEntries]; // Vector para guardar todas la areas de los pulsos
Double_t Minimo_por_pulso[(Int_t)NEntries]; //Vector para guardos todos los minimos de los pulso
// Con esto saca los datos de todos los pulso
Int_t EventNumber;
for(Int_t i=0; i< NEntries; i++)
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


    /* Para obtener el Base-Line*/
    Double_t Dmean[(Int_t)ADC_Pre_Trigger_Samples];
    Double_t Mean=0; //Media de los primeros puntos antes del cambio puntos analizados del eje y, para cada pulso
    for(Int_t i=0; i<(Int_t)ADC_Pre_Trigger_Samples; i++)
    {
        Dmean[i]=d_array[i];
    }

    //Calculo la media
    for(Int_t i=0; i<(Int_t)ADC_Pre_Trigger_Samples; i++)
    {
        Mean=Mean+Dmean[i]/(Int_t)ADC_Pre_Trigger_Samples;
    }



    Double_t Valuey_Min_P;
    // Minimo del pulso
    Int_t Min_P = TMath::LocMin((Int_t)ADC_Samples_per_Pulse, d_array);
    Valuey_Min_P=Event_Data->at(Min_P);



    

    Minimo_por_pulso[EventNumber]=Valuey_Min_P; //Agrego al vector





}

    //Creo un histograma
    //TH2D *EDepvsArea= new TH2D("EdvsArea", " ", 100,0,0,100,0,0);
    TH2D *EDepvsMax= new TH2D("MaxvsEd", " ", 100,0,0,100,0,0);
    //Creo el perfil
    auto *EDepvsMaxP= new TProfile("MaxvsEd", "", 100,0,1000,0,16000); //Perfil
    //EDepvsArea->GetXaxis()->SetTitle("#acute{A}rea del pulso");
    //EDepvsArea->GetYaxis()->SetTitle("Energ#acute{i}a Depositada [MeV]");

    EDepvsMax->GetYaxis()->SetTitle("Punto m#acute{a}ximo del pulso");
    EDepvsMax->GetXaxis()->SetTitle("Energ#acute{i}a Depositada [MeV]");

    EDepvsMaxP->GetYaxis()->SetTitle("Punto m#acute{a}ximo del pulso");
    EDepvsMaxP->GetXaxis()->SetTitle("Energ#acute{i}a Depositada [MeV]");


    //Obtengo los valores de la energia depositada y el area de los puntos
    for(Long_t i=0; i<NEntries; i++)
    {
        Data->GetEntry(i);
        EDepvsMax->Fill(Deposited_Energy,Minimo_por_pulso[i]);
        EDepvsMaxP->Fill(Deposited_Energy,Minimo_por_pulso[i]);
    }

//Hago el Fit

    TF1 * f1 = new TF1("f1","pol1");
    std::cout<< "Parametros energia Depositada vs Maxiomo del pulso" << std::endl;
    EDepvsMaxP->Fit(f1,"S");
    TFitResultPtr r = EDepvsMaxP->Fit(f1,"S");
    //TMatrixDSym C = r->GetCorrelationMatrix();
    r->Print("V");

    
    
    //Creo la grafica 

    TCanvas *a0Canvas = new TCanvas();
    //EDepvsArea->Draw();
    //a0Canvas->SaveAs("His_EDepvsArea.pdf");
    EDepvsMax->Draw();
    a0Canvas->SaveAs("His_EDepvsMax.pdf");
    EDepvsMaxP->Draw();
    a0Canvas->SaveAs("His_EDepvsMax_P.pdf");

}
