#include <crsRead/MCorsikaReader.h>

#include <crs/TSubBlock.h>
#include <crs/MRunHeader.h>
#include <crs/MEventHeader.h>
#include <crs/MEventEnd.h>
#include <crs/MParticleBlock.h>
#include <crs/MLongitudinalBlock.h>
#include <crs/MParticle.h>

#include <TFile.h>
#include <TH1D.h>
#include <TNtupleD.h>

#include "ROOT/RDataFrame.hxx"

#include <iostream>
#include <sstream>
#include <map>
#include <filesystem>
#include <string>

using namespace std;
// namespace fs = std::filesystem;

#define DAT_PATH "/home/crmusr/cosmicraySimulation/corsika-77500/run/CorsikaMuon/fakeMPI_run1_N1000/";

struct ObsLevel
{
    TNtupleD* data;
    double x;
    double y;
    double x2;
    double y2;
    double w;
    double theta;
    int showerid;
    int runnr;
};


int main (int argc, char **argv) 
{  
    if (argc<2)
    {
        std::cout << "  please specify Corsika file " << endl;
        return 1;
    }
    
    int runMarker = 0;

    string fname(argv[1]);
    crsRead::MCorsikaReader cr(fname, 3);

    string inputFile;
    if (fname.rfind('/')==string::npos)
    {
        inputFile = fname;
    }
    else
    {
        inputFile = fname.substr(fname.rfind('/')+1, fname.size()-fname.rfind('/'));
    }


    int ShowerCounter = 0;
    int totalMuonNum = 0;
    crs::MRunHeader Run;

    while (cr.GetRun (Run))
    {
        const int nRun = Run.GetRunID();
        std::cout << "---------------------------------\n"
            << " Run info:\n"
            << "  run number = " << nRun << "\n"; // runnr
        ostringstream oFileName;
        if (nRun % 2 == 1)
            oFileName << "./rootfiles/p" << inputFile.c_str() << "_Muon" << ".root";
        if (nRun % 2 == 0)
            oFileName << "./rootfiles/He" << inputFile.c_str() << "_Muon" << ".root";
        TFile *oFile = new TFile(oFileName.str().c_str(), "RECREATE");
        TNtupleD *ntuple = new TNtupleD("data","data at sea level","id:ek:px:py:pz:x:y:t:w:theta:PDGCode:runnr:showerid");


        crs::MEventHeader Shower;
        // 遍历每个shower
        // ObsLevel emptyLevel;
        // ostringstream tTitle, tName;
        // tTitle << "Data at level";
        // tName << "data";
        // emptyLevel.data = new TNtupleD(tName.str().c_str(), tTitle.str().c_str(), "id:ek:px:py:pz:x:y:t:w:theta:PDGCode:runnr:showerid");

        long particlenum;
        while (cr.GetShower(Shower))
        {
            ++ShowerCounter; // showerid
            particlenum = 0;

            // std::cout << " Writing summary to output file: " << oFileName.str() << endl;

            
            const int nObsLevel = Shower.GetNObservationLevels();
            map<int, ObsLevel> obsLevel;
            
            // 获得一个shower中的obLevel的个数，并初始化一个空的TNtupleD
            // for (int iObsLevel=1; ShowerCounter == 1 && iObsLevel<=nObsLevel; ++iObsLevel)
            // { 
            //     double height = Shower.GetObservationHeight(iObsLevel-1);            
            //     cout << " init obs-level " << iObsLevel << " at h=" << height << "cm" << endl;
            //     ObsLevel emptyLevel;
            //     ostringstream tTitle, tName;
            //     tTitle << "Data at level " << iObsLevel;
            //     tName << "data_" << iObsLevel;

            //     emptyLevel.data = new TNtupleD(tName.str().c_str(), tTitle.str().c_str(), "id:ek:px:py:pz:x:y:t:w:theta:PDGCode:runnr:showerid");
                
            //     emptyLevel.x  = 0;
            //     emptyLevel.y  = 0;
            //     emptyLevel.w  = 0;
            //     emptyLevel.x2 = 0;
            //     emptyLevel.y2 = 0;
            //     emptyLevel.theta = 0;
            //     emptyLevel.showerid = ShowerCounter;
            //     emptyLevel.runnr = nRun;
                
            //     obsLevel[iObsLevel] = emptyLevel;

            // } // end loop observation levels

            const double zenith = Shower.GetTheta();
            const double azimuth = Shower.GetPhi();
            const double Zfirst = Shower.GetZFirst();
            // std::cout << "---------------------------------\n" 
            //     << " Shower with:\n" << "  Zfirst = " << Zfirst << "  Theta = " << zenith << "  Phi = " << azimuth << "\n";
            
            // 数据读取 TSubBlock
            crs::TSubBlock Data;
            while (cr.GetData (Data))
            {
                switch (Data.GetBlockType ())
                {
                    case crs::TSubBlock::ePARTDATA:
                    {
                        const crs::MParticleBlock& ParticleData = Data;
                        crs::MParticleBlock::ParticleListConstIterator iEntry;
                        for (iEntry = ParticleData.FirstParticle(); iEntry != ParticleData.LastParticle(); ++iEntry) 
                        {
                            if (iEntry->IsParticle())
                            {
                                crs::MParticle iPart(*iEntry);
                                
                                const int id    = iPart.GetParticleID();
                                const int level = iPart.GetObservationLevel();
                                const double w  = iPart.GetWeight();
                                const double ek  = iPart.GetKinEnergy();
                                const double px = iPart.GetPx();
                                const double py = iPart.GetPy();
                                const double pz = iPart.GetPz();
                                const double x  = iPart.GetX();
                                const double y  = iPart.GetY();
                                const double t  = iPart.GetTime();
                                const double theta = iPart.GetTheta();
                                const int PDGCode = iPart.GetPDGCode();
                                const int showerid = ShowerCounter;
                                const int runnr = nRun;
                                
                                // if (std::abs(PDGCode) != 13)
                                // if (id != 5 && id != 6) // mu+ 5; mu- 6
                                std::set<int> neededPDGCodes = {13, 12, 14, 16, 18}; // muon PDGcodes and neutrino PDGcodes
                                if (neededPDGCodes.find(std::abs(PDGCode)) == neededPDGCodes.end())
                                {
                                    continue;
                                }
                                particlenum++;
                         
                                ntuple->Fill(id,ek,px,py,pz,x,y,t,w,theta,PDGCode,runnr,showerid);

                            }
                            
                        } // end particle loop
                        // std::cout << "put particle into obsLevelMuon, end particle loop!" << std::endl;
                        
                        break;
                    }
                    
                    case crs::TSubBlock::eLONG:
                        break;
                        
                    default:
                        break;
                } // end data block
                
            } // loop data
            
            crs::MEventEnd ShowerSummary;
            cr.GetShowerSummary(ShowerSummary);
            const double Xmax = ShowerSummary.GetXmax();
            
            
            // std::cout << "---------------------------------\n"
            //     << " Shower info:\n"
            //     << "  Xmax = " << Xmax << "\n" << "number of particles = " << particlenum << "\n";

            totalMuonNum += particlenum;
            // TFile oFile (oFileName.str ().c_str (), "UPDATE");
            // oFile.cd();
            // for (map<int, ObsLevel>::iterator iLevel = obsLevel.begin(); iLevel != obsLevel.end(); ++iLevel)
            // {

            //     // iLevel->second.data->Write();
            
            //     double npart=iLevel->second.w;
            //     if(npart>0)
            //     cout << "   observation level: " << iLevel->first 
            //          << " with " << npart << " particles "
            //          << "    <x>= " << iLevel->second.x / npart
            //          << " +/- " << sqrt(iLevel->second.x2/npart-pow(iLevel->second.x/npart,2))/max(1.,npart-1.)
            //          << " cm <y>= " << iLevel->second.y /npart
            //          << " +/- " << sqrt(iLevel->second.y2/npart-pow(iLevel->second.y/npart,2))/max(1.,npart-1.)
            //          << " cm \n"; 
            // } // loop observation levels
            // oFile.Close();


        } // loop shower
        oFile->Write();
        oFile->Close();

        delete oFile;
        runMarker++;
        std::cout << "Run number " << runMarker << std::endl;

    } // loop runs (usually just 1)    

    std::cout << " Read " << ShowerCounter << " showers from file, total number of muons = " << totalMuonNum << endl;
    // delete ntuple;

    return 0;
}
