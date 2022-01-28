// root script file
/* Programa para extraer los datos y realizar histogramas
    Elser Lopez, 23/05/2020
*/

#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TH1D.h"

#include <cstdlib>
#include <iostream>

void HistogramasP1(const char* FileName)
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

    //Energia depositada vs inicial
    TH2D *EDepvsEi= new TH2D("EdvsEi", " ", 100,0,0,100,0,0);
    TH2D *EDepvsAng= new TH2D("EdvsAng", " ", 100,0,0,100,0,0);
    TH2D *EDepvsNPCh= new TH2D("EdvsNPCh", " ", 100,0,0,100,0,0);
    TH2D *EDepvsNPPMT= new TH2D("EdvsNPPMT", " ", 100,0,0,100,0,0);
    TH2D *EDepvsTL= new TH2D("EdvsTL", " ", 100,0,0,100,0,0);

    //Nombres de los ejes 
    EDepvsEi->GetYaxis()->SetTitle("Energ#acute{i}a Inicial [MeV]");
    EDepvsEi->GetXaxis()->SetTitle("Energ#acute{i}a Depositada [MeV]");
    
    EDepvsAng->GetYaxis()->SetTitle("#acute{A}ngulo Cenital [rad]");
    EDepvsAng->GetXaxis()->SetTitle("Energ#acute{i}a Depositada [MeV]");

    EDepvsNPCh->GetYaxis()->SetTitle("Fotones Cherenkov emitidos");
    EDepvsNPCh->GetXaxis()->SetTitle("Energ#acute{i}a Depositada [MeV]");

    EDepvsNPPMT->GetYaxis()->SetTitle("Fotones que producen efecto fotoel#acute{e}ctrico en el PMT");
    EDepvsNPPMT->GetXaxis()->SetTitle("Energ#acute{i}a Depositada [MeV]");

    EDepvsTL->GetYaxis()->SetTitle("Distancia viajada");
    EDepvsTL->GetXaxis()->SetTitle("Energ#acute{i}a Depositada [MeV]");

    for(Long_t i=0; i<NEntries; i++)
    {
        Data->GetEntry(i);
        EDepvsEi->Fill(Deposited_Energy,Energy);
        EDepvsAng->Fill(Deposited_Energy,Zenith_angle);
        EDepvsNPCh->Fill(Deposited_Energy,Cherenkov_Photon_Count);
        EDepvsNPPMT->Fill(Deposited_Energy,PMT_Photon_Count);
        EDepvsTL->Fill(Deposited_Energy,Track_Length );
    }
    
    

    TCanvas *a0Canvas = new TCanvas();

    /* Dibuja y guarda los histogramas de datos */
    EDepvsAng->Draw();
    a0Canvas->SaveAs("His_EDepvsAng.pdf");
    EDepvsNPCh->Draw();
    a0Canvas->SaveAs("His_EDepvsNfotonesCh.pdf");
    EDepvsNPPMT->Draw();
    a0Canvas->SaveAs("His_EDepvsNfotonesPMT.pdf");
    EDepvsTL->Draw();
    a0Canvas->SaveAs("His_EDepvsDistancia.pdf");
    EDepvsEi->Draw();
    a0Canvas->SaveAs("His_EDepvsEi.jpg");

}