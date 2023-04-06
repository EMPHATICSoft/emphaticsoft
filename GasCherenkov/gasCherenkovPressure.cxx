//Program to calculate ideal pressures for particle detection in Cherenkov Detectors
//Authors: Christian Perez, Kenzi Waddell
//Institute: Kennesaw State University
//Date: 1/7/2022

//READ ME: To compile on linux systems, place this file in desired directory and run terminal command "g++ -std=c++0x  -o cherenkovExec cherenkov.cpp",
//then execute using ./cherenkovExec which will run code and produce a text file with data inside the same directory. 

#include <iostream>

#include <cmath>

#include <math.h>

#include <fstream>

using namespace std;

//Function that finds beta value based on gamma value of particle

double betaCalculator(double momentum, double particleMass) {
  double beta = momentum / sqrt((momentum * momentum) + (particleMass * particleMass));
  return beta;
}

//Function that finds index of refraction for given beta and desired angle

double nCalculator(double beta, double angle) {
  double n = 1 / (beta * cos(angle));
  return n;
}

//Function that finds corresponing pressure to given index of refraction, and gas' index of refraction value at 1 atm.

double pCalculator(double nCalculated, double nKnown) {
  double pCalculated = ((nCalculated - 1) / (nKnown - 1)) * 14.695948803581; // constant number at end converts from atm to PSIA
  return pCalculated;
}

int main()

{

  cout.precision(100000000000000);

  //Variables containing masses of particles
  double protonMass = 0.938; //GeV/c*c
  double pionMass = 0.139; //GeV/c*c
  double muonMass = 0.106; //GeV/c*c
  double electronMass = 0.000511; //GeV/c*c
  double kaonMass = 0.493; // GeV//c*c

  //Variables that will contain Index of Refraction (n) values for particles
  double nProton;
  double nPion;
  double nMuon;
  double nElectron;
  double nKaon;

  //Variables that will contain beta values for particles
  double betaProton;
  double betaPion;
  double betaMuon;
  double betaElectron;
  double betaKaon;

  //Variables for pressure values specific to particle and beam momenta
  double pProton;
  double pPion;
  double pMuon;
  double pElectron;
  double pKaon;

  //Variables for user input parameters
  double thresholdAngle = 0; //Angle for production of cherenkov radiation
  double angleSmall = 0.007; //PMT1 angle in radians
  double angleLarge = 0.030; //PMT2 angle in radians
  double indexGasOne = 1.000297; //Refractive index of Nitrogen at 1atm
  double indexGasTwo = 1.00045; //Refractive index of Carbon Dioxide at 1atm
  double RIHighGel = 1.027;
  double RIMedGel = 1.007;
  double RILowGel = 1.003;
  double beamMomentum;
  double beamMomenta[7] = {
    120,
    60,
    31,
    20,
    12,
    8,
    4
  };

  betaProton = betaCalculator(beamMomentum, protonMass);
  betaPion = betaCalculator(beamMomentum, pionMass);
  betaMuon = betaCalculator(beamMomentum, muonMass);
  betaElectron = betaCalculator(beamMomentum, electronMass);
  betaKaon = betaCalculator(beamMomentum, kaonMass);

  nProton = nCalculator(betaProton, angleSmall);
  nPion = nCalculator(betaPion, angleSmall);
  nMuon = nCalculator(betaMuon, angleSmall);
  nElectron = nCalculator(betaElectron, angleSmall);
  nKaon = nCalculator(betaKaon, angleSmall);

  pProton = pCalculator(nProton, indexGasOne);
  pPion = pCalculator(nPion, indexGasOne);
  pMuon = pCalculator(nMuon, indexGasOne);
  pElectron = pCalculator(nElectron, indexGasOne);
  pKaon = pCalculator(nKaon, indexGasOne);

  //Code for printing data to text document
  //This will save the file to the directory the executable is located in
  ofstream TextFile("cherenkov_pressure_data.txt");

  TextFile << "Particle Masses (GeV / c*c):" << "\nProtons: " << protonMass << "\nKaons: " << kaonMass << "\nPions: " << pionMass << "\nMuons: " << muonMass << "\nElectrons: " << electronMass << endl;
  TextFile << "\nIndex of Refraction Nitrogen Gas (1 atm): " << indexGasOne << "\nIndex of Refraction Carbon Dioxide Gas(1 atm): " << indexGasTwo << endl;
  TextFile << "\nAngle of PMT1 (radians): " << angleSmall << "\nAngle of PMT2 (radians): " << angleLarge << endl;

  TextFile << "\nPressures for Nitrogen Gas at threshold: " << endl;
  for (int i = 0; i <= 6; i++) {

    beamMomentum = beamMomenta[i];
    betaProton = betaCalculator(beamMomentum, protonMass);
    betaPion = betaCalculator(beamMomentum, pionMass);
    betaMuon = betaCalculator(beamMomentum, muonMass);
    betaElectron = betaCalculator(beamMomentum, electronMass);
    betaKaon = betaCalculator(beamMomentum, kaonMass);

    nProton = nCalculator(betaProton, thresholdAngle);
    nPion = nCalculator(betaPion, thresholdAngle);
    nMuon = nCalculator(betaMuon, thresholdAngle);
    nElectron = nCalculator(betaElectron, thresholdAngle);
    nKaon = nCalculator(betaKaon, thresholdAngle);

    pProton = pCalculator(nProton, indexGasOne);
    pPion = pCalculator(nPion, indexGasOne);
    pMuon = pCalculator(nMuon, indexGasOne);
    pElectron = pCalculator(nElectron, indexGasOne);
    pKaon = pCalculator(nKaon, indexGasOne);

    TextFile << "Beam Momentum: " << beamMomentum << " (GeV/c)" << "\nProton (PSIA): " << pProton << ", (n - 1): " << nProton - 1 <<
      "\nKaon (PSIA): " << pKaon << ", (n - 1): " << nKaon - 1 << "\nPion (PSIA): " << pPion <<
      ", (n - 1): " << nPion - 1 << "\nMuon (PSIA): " << pMuon << ", (n - 1): " << nMuon - 1 <<
      "\nElectron (PSIA): " << pElectron << ", (n - 1): " << nElectron - 1 << "\n" << endl;

  }

  TextFile << "\nPressures for Nitrogen Gas at 7 mradians: " << endl;
  for (int i = 0; i <= 6; i++) {

    beamMomentum = beamMomenta[i];
    betaProton = betaCalculator(beamMomentum, protonMass);
    betaPion = betaCalculator(beamMomentum, pionMass);
    betaMuon = betaCalculator(beamMomentum, muonMass);
    betaElectron = betaCalculator(beamMomentum, electronMass);
    betaKaon = betaCalculator(beamMomentum, kaonMass);

    nProton = nCalculator(betaProton, angleSmall);
    nPion = nCalculator(betaPion, angleSmall);
    nMuon = nCalculator(betaMuon, angleSmall);
    nElectron = nCalculator(betaElectron, angleSmall);
    nKaon = nCalculator(betaKaon, angleSmall);

    pProton = pCalculator(nProton, indexGasOne);
    pPion = pCalculator(nPion, indexGasOne);
    pMuon = pCalculator(nMuon, indexGasOne);
    pElectron = pCalculator(nElectron, indexGasOne);
    pKaon = pCalculator(nKaon, indexGasOne);

    TextFile << "Beam Momentum: " << beamMomentum << " (GeV/c)" << "\nProton (PSIA): " << pProton << ", (n - 1): " << nProton - 1 <<
      "\nKaon (PSIA): " << pKaon << ", (n - 1): " << nKaon - 1 << "\nPion (PSIA): " << pPion <<
      ", (n - 1): " << nPion - 1 << "\nMuon (PSIA): " << pMuon << ", (n - 1): " << nMuon - 1 <<
      "\nElectron (PSIA): " << pElectron << ", (n - 1): " << nElectron - 1 << "\n" << endl;

  }

  TextFile << "\nPressures for Nitrogen Gas at 30 mradians: " << endl;
  for (int i = 0; i <= 6; i++) {

    beamMomentum = beamMomenta[i];
    betaProton = betaCalculator(beamMomentum, protonMass);
    betaPion = betaCalculator(beamMomentum, pionMass);
    betaMuon = betaCalculator(beamMomentum, muonMass);
    betaElectron = betaCalculator(beamMomentum, electronMass);
    betaKaon = betaCalculator(beamMomentum, kaonMass);

    nProton = nCalculator(betaProton, angleLarge);
    nPion = nCalculator(betaPion, angleLarge);
    nMuon = nCalculator(betaMuon, angleLarge);
    nElectron = nCalculator(betaElectron, angleLarge);
    nKaon = nCalculator(betaKaon, angleLarge);

    pProton = pCalculator(nProton, indexGasOne);
    pPion = pCalculator(nPion, indexGasOne);
    pMuon = pCalculator(nMuon, indexGasOne);
    pElectron = pCalculator(nElectron, indexGasOne);
    pKaon = pCalculator(nKaon, indexGasOne);

    TextFile << "Beam Momentum: " << beamMomentum << " (GeV/c)" << "\nProton (PSIA): " << pProton << ", (n - 1): " << nProton - 1 <<
      "\nKaon (PSIA): " << pKaon << ", (n - 1): " << nKaon - 1 << "\nPion (PSIA): " << pPion <<
      ", (n - 1): " << nPion - 1 << "\nMuon (PSIA): " << pMuon << ", (n - 1): " << nMuon - 1 <<
      "\nElectron (PSIA): " << pElectron << ", (n - 1): " << nElectron - 1 << "\n" << endl;

  }

  TextFile << "\nPressures for Carbon Dioxide Gas at threshold: " << endl;
  for (int i = 0; i <= 6; i++) {

    beamMomentum = beamMomenta[i];
    betaProton = betaCalculator(beamMomentum, protonMass);
    betaPion = betaCalculator(beamMomentum, pionMass);
    betaMuon = betaCalculator(beamMomentum, muonMass);
    betaElectron = betaCalculator(beamMomentum, electronMass);
    betaKaon = betaCalculator(beamMomentum, kaonMass);

    nProton = nCalculator(betaProton, thresholdAngle);
    nPion = nCalculator(betaPion, thresholdAngle);
    nMuon = nCalculator(betaMuon, thresholdAngle);
    nElectron = nCalculator(betaElectron, thresholdAngle);
    nKaon = nCalculator(betaKaon, thresholdAngle);

    pProton = pCalculator(nProton, indexGasTwo);
    pPion = pCalculator(nPion, indexGasTwo);
    pMuon = pCalculator(nMuon, indexGasTwo);
    pElectron = pCalculator(nElectron, indexGasTwo);
    pKaon = pCalculator(nKaon, indexGasTwo);

    TextFile << "Beam Momentum: " << beamMomentum << " (GeV/c)" << "\nProton (PSIA): " << pProton << ", (n - 1): " << nProton - 1 <<
      "\nKaon (PSIA): " << pKaon << ", (n - 1): " << nKaon - 1 << "\nPion (PSIA): " << pPion <<
      ", (n - 1): " << nPion - 1 << "\nMuon (PSIA): " << pMuon << ", (n - 1): " << nMuon - 1 <<
      "\nElectron (PSIA): " << pElectron << ", (n - 1): " << nElectron - 1 << "\n" << endl;

  }

  TextFile << "\nPressures for Carbon Dioxide Gas at 7 mradians: " << endl;
  for (int i = 0; i <= 6; i++) {

    beamMomentum = beamMomenta[i];
    betaProton = betaCalculator(beamMomentum, protonMass);
    betaPion = betaCalculator(beamMomentum, pionMass);
    betaMuon = betaCalculator(beamMomentum, muonMass);
    betaElectron = betaCalculator(beamMomentum, electronMass);
    betaKaon = betaCalculator(beamMomentum, kaonMass);

    nProton = nCalculator(betaProton, angleSmall);
    nPion = nCalculator(betaPion, angleSmall);
    nMuon = nCalculator(betaMuon, angleSmall);
    nElectron = nCalculator(betaElectron, angleSmall);
    nKaon = nCalculator(betaKaon, angleSmall);

    pProton = pCalculator(nProton, indexGasTwo);
    pPion = pCalculator(nPion, indexGasTwo);
    pMuon = pCalculator(nMuon, indexGasTwo);
    pElectron = pCalculator(nElectron, indexGasTwo);
    pKaon = pCalculator(nKaon, indexGasTwo);

    TextFile << "Beam Momentum: " << beamMomentum << " (GeV/c)" << "\nProton (PSIA): " << pProton << ", (n - 1): " << nProton - 1 <<
      "\nKaon (PSIA): " << pKaon << ", (n - 1): " << nKaon - 1 << "\nPion (PSIA): " << pPion <<
      ", (n - 1): " << nPion - 1 << "\nMuon (PSIA): " << pMuon << ", (n - 1): " << nMuon - 1 <<
      "\nElectron (PSIA): " << pElectron << ", (n - 1): " << nElectron - 1 << "\n" << endl;

  }

  TextFile << "\nPressures for Carbon Dioxide Gas at 30 mradians: " << endl;
  for (int i = 0; i <= 6; i++) {

    beamMomentum = beamMomenta[i];
    betaProton = betaCalculator(beamMomentum, protonMass);
    betaPion = betaCalculator(beamMomentum, pionMass);
    betaMuon = betaCalculator(beamMomentum, muonMass);
    betaElectron = betaCalculator(beamMomentum, electronMass);
    betaKaon = betaCalculator(beamMomentum, kaonMass);

    nProton = nCalculator(betaProton, angleLarge);
    nPion = nCalculator(betaPion, angleLarge);
    nMuon = nCalculator(betaMuon, angleLarge);
    nElectron = nCalculator(betaElectron, angleLarge);
    nKaon = nCalculator(betaKaon, angleLarge);

    pProton = pCalculator(nProton, indexGasTwo);
    pPion = pCalculator(nPion, indexGasTwo);
    pMuon = pCalculator(nMuon, indexGasTwo);
    pElectron = pCalculator(nElectron, indexGasTwo);
    pKaon = pCalculator(nKaon, indexGasTwo);

    TextFile << "Beam Momentum: " << beamMomentum << " (GeV/c)" << "\nProton (PSIA): " << pProton << ", (n - 1): " << nProton - 1 <<
      "\nKaon (PSIA): " << pKaon << ", (n - 1): " << nKaon - 1 << "\nPion (PSIA): " << pPion <<
      ", (n - 1): " << nPion - 1 << "\nMuon (PSIA): " << pMuon << ", (n - 1): " << nMuon - 1 <<
      "\nElectron (PSIA): " << pElectron << ", (n - 1): " << nElectron - 1 << "\n" << endl;

  }

  TextFile << "\nBACkov Detector: " << endl;
  for (int i = 3; i <= 6; i++) {

    beamMomentum = beamMomenta[i];
    betaProton = betaCalculator(beamMomentum, protonMass);
    betaPion = betaCalculator(beamMomentum, pionMass);
    betaMuon = betaCalculator(beamMomentum, muonMass);
    betaElectron = betaCalculator(beamMomentum, electronMass);
    betaKaon = betaCalculator(beamMomentum, kaonMass);

    // TextFile << "Beam Momentum: " << beamMomentum << " (GeV/c)" << 

    TextFile.close();
  }
}
