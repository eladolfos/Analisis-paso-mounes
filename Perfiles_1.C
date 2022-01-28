// root script file
// Programa para crear los perfiles y realizar fits
#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TH1D.h"

#include <cstdlib>
#include <iostream>

#include <math.h>


/* Programa para extraer los datos y realizar histogramas
    Elser Lopez, 23/05/2020
    Realiza el histograma del Minimo  del pulso y energia depositada
*/


/*  FUNCION PARA CALCULAR EL COEFICIENTE DE CORRELACION LINEAL DE PEARSON */
    Double_t CorrelacionLineal(Double_t xces[], Double_t yces[], Int_t n )
{
    Double_t Numerador=0;
    Double_t Denominador=0;
    Double_t Mediax=0;
    Double_t Mediay=0;
    Double_t Sum1=0;
    Double_t Sum2=0;
    //Calcula la media en x
    for( Int_t i=0; i<n; i++)
    {
        Mediax=Mediax+xces[i]/n;
    }
    //Calcula la media en y
    for( Int_t i=0; i<n; i++)
    {
        Mediay=Mediay+yces[i]/n;
    }

    for( Int_t i=0; i<n; i++)
    {
        Numerador=Numerador+(xces[i]-Mediax)*(yces[i]-Mediay);
        Sum1=Sum1+(xces[i]-Mediax)*(xces[i]-Mediax);
        Sum2=Sum2+(yces[i]-Mediay)*(yces[i]-Mediay);
    }

    Denominador=sqrt(Sum1)*sqrt(Sum2);
    // Debuelve el coeficiente de correlacion
    return Numerador/Denominador;
}


void Perfiles_1(const char* FileName)
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

    //Hace un histograma
    //TH1D *EDep= new TH1D("HEdep", "Energya Depositada", 100,0,0); 
    //Energia depositada vs inicial
    auto *EDepvsEi= new TProfile("EivsEd", " ", 100,0,1000,0,4500);
    auto *EDepvsAng= new TProfile("AngvsEd", " ", 100,0,1000,0,1.7);
    auto *EDepvsNPCh= new TProfile("NPChvsEd", " ", 100,0,1000,0,30000);
    auto *EDepvsNPPMT= new TProfile("NPPMTvsEd", " ", 100,0,1000,0,70500);
    auto *EDepvsTL= new TProfile("TLvsEd", " ", 100,0,1000,0,5500); 

    //TH2D *EivsEDep = new TH2D("H", "Energia inicial vs Depositadas", 100,0,0,100,0,0);
    //Nombres de los ejes Energia depositada vs energia inicial
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
        EDepvsAng->Fill(Deposited_Energy,Zenith_angle );
        EDepvsNPCh->Fill(Deposited_Energy, Cherenkov_Photon_Count );
        EDepvsNPPMT->Fill(Deposited_Energy,PMT_Photon_Count );
        EDepvsTL->Fill(Deposited_Energy,Track_Length);
    }
    
    

    
   


    //TGraph *aGraph = new TGraph((Int_t)ADC_Samples_per_Pulse, t_array, d_array);
    //aGraph->SetTitle("Digitalized signal");
    gStyle->SetOptFit(0000);
    //mode = pcev(default = 0111)
    TCanvas *a0Canvas = new TCanvas("C1");

    
    // Hago el fit de cada perfil
    //Energia depositada vs distancia viajada

    TF1 * f1 = new TF1("f1","pol1");

    std::cout<< "Parametros energia Depositada vs Distancia" << std::endl;
    EDepvsTL->Fit(f1,"S");
    TFitResultPtr r = EDepvsTL->Fit(f1,"S");
    //TMatrixDSym C = r->GetCorrelationMatrix();
    r->Print("V");
    
    /*
    
    
    TMatrixD matrix0(2,2);
    gMinuit->mnemat(matrix0.GetMatrixArray(),2);
    matrix0.Print();
    gMinuit->mnmatu(1); */
    std::cout<< "Parametros energia Depositada vs Fotones en el PMT" << std::endl;
    //Energia depositada vs fotones en el PMT
    EDepvsNPPMT->Fit("pol1");
    TFitResultPtr r2 = EDepvsNPPMT->Fit(f1,"S");
    //TMatrixDSym C = r->GetCorrelationMatrix();
    r2->Print("V");
    /* TF1 *fit2 = EDepvsNPPMT->GetFunction("pol1");
    TMatrixD matrix1(2,2);
    gMinuit->mnemat(matrix1.GetMatrixArray(),2);
    matrix1.Print();
    gMinuit->mnmatu(1);
    */

    //std::cout<< "correalacion " <<  p1 << std::endl;

    //RFile->Close();
    /* Dibuja y guarda los histogramas de datos */
    EDepvsEi->Draw();
    a0Canvas->SaveAs("His_EDepvsEi_P.pdf");
    EDepvsAng->Draw();
    a0Canvas->SaveAs("His_EDepvsAng_P.pdf");

    EDepvsNPCh->Draw();
    a0Canvas->SaveAs("His_EDepvsNfotonesCh_P.pdf");
    EDepvsNPPMT->Draw();
    a0Canvas->SaveAs("His_EDepvsNfotonesPMT_P.pdf");
    EDepvsTL->Draw();
    a0Canvas->SaveAs("His_EDepvsDistancia_P.pdf");
    
     //use a TProfile to convert the 2-d to 1-d problem
    //TProfile *prof = EDepvsTL->ProfileX();


}