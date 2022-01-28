// root script file
// Programa para crear los perfiles y realizar fits 
// para el area por pulso

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"

#include <cstdlib>
#include <iostream>

void Perfiles_2(const char* FileName)
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



Double_t Area_por_pulso[(Int_t)NEntries]; // Vector para guardar todas la areas de los pulsos
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

    //TGraph *aGraph = new TGraph((Int_t)ADC_Samples_per_Pulse, t_array, d_array);
    //aGraph->SetTitle("Digitalized signal");
    //TCanvas *aCanvas = new TCanvas("C1","Simulated WCD output", 800,600);
    //aGraph->Draw();
    //aCanvas->SaveAs("pulso.pdf");
    //RFile->Close();


/* Calculando el area del pulso */


    //Double_t Yes=[ADC_Samples_per_Pulse];
    /* agrego los primeros 15 puntos del eje y para calcular la media
    * asi obtener el Base-Line*/
    Double_t Dmean[(Int_t)ADC_Pre_Trigger_Samples];
    Double_t Mean=0; //Media de los primeros 15 puntos analizados del eje y, para cada pulso
    for(Int_t i=0; i<(Int_t)ADC_Pre_Trigger_Samples; i++)
    {
        Dmean[i]=d_array[i];
    }

    //Calculo la media
    for(Int_t i=0; i<(Int_t)ADC_Pre_Trigger_Samples; i++)
    {
        Mean=Mean+Dmean[i]/(Int_t)ADC_Pre_Trigger_Samples;
    }


    //Calculo el area del pulso
    Double_t h; //Altura en la formula del trapecio
    Double_t Base;
    Double_t AreaB=0; //Area bajo la curva
    Double_t AreaS=0; //Area del rectangulo
    Double_t AreaP=0; //Area del pulso
    for(Int_t i=0; i<ADC_Samples_per_Pulse-20; i++ )
        {//Con esto calculo el area bajo la curva
            h=(t_array[i+1]-t_array[i]);
            Base=d_array[i]+d_array[i+1];
            AreaB=AreaB+h*(Base)/2.0;
            //std::cout << "Area bajo la curva=" << AreaB << " valores en y=" << d_array[i] << std::endl;
        }
    AreaS=Mean*( (Int_t)ADC_Samples_per_Pulse-20 );

    AreaP=Mean*((Int_t)ADC_Samples_per_Pulse-20 )-AreaB;
    

    Area_por_pulso[EventNumber]=AreaP; //Agrego al vector 

    std::cout << "Pulso No. " << EventNumber << " Area= " <<  Area_por_pulso[EventNumber] << " Area bajo la curva "<< AreaB << " Media=" << Mean << std::endl;
}

    //Creo un perfil con los datos obtenidos
    auto *EDepvsArea= new TProfile("EdvsArea", " ", 100,0,350000,0,1000);
    EDepvsArea->GetXaxis()->SetTitle("#acute{A}rea del pulso");
    EDepvsArea->GetYaxis()->SetTitle("Energ#acute{i}a Depositada [MeV]");




    //Obtengo los valores de la energia depositada y el area de los puntos
    for(Long_t i=0; i<NEntries; i++)
    {
        Data->GetEntry(i);
        EDepvsArea->Fill(Area_por_pulso[i],Deposited_Energy);
    }


    // Hago un fit lineal

    EDepvsArea->Fit("pol1");


    //Creo la grafica 

    TCanvas *a0Canvas = new TCanvas();
    EDepvsArea->Draw();
    a0Canvas->SaveAs("His_EDepvsArea_P.pdf");

}
