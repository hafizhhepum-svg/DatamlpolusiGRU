#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "TCanvas.h"
#include "TGraph.h"
#include "TF1.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TPaveText.h"
#include "TLegend.h"

void grafik1(){

    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);

    std::ifstream infile("PM25_ALL.txt");

    if(!infile.is_open()){
        std::cout<<"Error opening file"<<std::endl;
        return;
    }

    std::vector<double> x_vals;
    std::vector<double> y_vals;

    double x,y;

    while(infile>>x>>y){

        if(x==0 || y==0) continue;

        x_vals.push_back(x);
        y_vals.push_back(y);
    }

    infile.close();

    int n = x_vals.size();

    if(n<2){
        std::cout<<"Not enough data"<<std::endl;
        return;
    }

    // CANVAS
    TCanvas *c1 = new TCanvas("c1","Scatter Plot",800,800);

    c1->SetLeftMargin(0.15);
    c1->SetBottomMargin(0.15);

    // GRAPH
    TGraph *gr = new TGraph(n,&x_vals[0],&y_vals[0]);

    gr->SetTitle(";PM2.5 Observed [AQI];PM2.5 Predicted [AQI]");

    gr->SetMarkerStyle(24);
    gr->SetMarkerSize(0.7);
    gr->SetMarkerColor(kBlue);

    // RANGE
    gr->GetXaxis()->SetLimits(0,500);
    gr->SetMinimum(0);
    gr->SetMaximum(500);

    // INTERVAL 100
    gr->GetXaxis()->SetNdivisions(505);
    gr->GetYaxis()->SetNdivisions(505);

    // Hilangkan label 0 di sumbu Y
    gr->GetYaxis()->ChangeLabel(1,-1,0);

    gr->GetXaxis()->CenterTitle();
    gr->GetYaxis()->CenterTitle();

    gr->Draw("AP");

    // FIT LINEAR
    TF1 *fit = new TF1("fit","pol1",0,500);

    fit->SetLineColor(kRed);
    fit->SetLineWidth(4);

    gr->Fit(fit,"R");

    double m = fit->GetParameter(1);

    // HITUNG R2
    double y_mean=0;

    for(int i=0;i<n;i++)
        y_mean += y_vals[i];

    y_mean/=n;

    double SS_tot=0;
    double SS_res=0;

    for(int i=0;i<n;i++){

        double y_fit = fit->Eval(x_vals[i]);

        SS_tot += pow(y_vals[i]-y_mean,2);
        SS_res += pow(y_vals[i]-y_fit,2);

    }

    double R2 = 1-(SS_res/SS_tot);

    // LEGEND
    TLegend *leg = new TLegend(0.15,0.78,0.45,0.88);
    leg->AddEntry(gr,"Data","p");
    leg->AddEntry(fit,Form("Fit line (slope = %.3f)",m),"l");

    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);


    // TEXT R2
    TPaveText *pt = new TPaveText(0.65,0.78,0.85,0.88,"NDC");
    pt->AddText(Form("R^{2} = %.3f",R2));
    pt->SetFillStyle(0);
    pt->SetFillColor(0);
    pt->SetBorderSize(0);

    c1->cd(1);
    gr->Draw("AP");
    leg->Draw();
    pt->Draw();

    c1->SaveAs("./output/scatter_PM25_fit.png");
    c1->SaveAs("./output/scatter_PM25_fit.pdf");
}
