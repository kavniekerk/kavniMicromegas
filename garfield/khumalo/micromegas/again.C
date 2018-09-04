#include <iostream>
#include <TCanvas.h>
#include <TROOT.h>
#include <TApplication.h>
#include <TH1F.h>

#include "MediumMagboltz.hh"
#include "SolidBox.hh"
#include "SolidTube.hh"
#include "GeometrySimple.hh"
#include "ComponentConstant.hh"
#include "ComponentAnalyticField.hh"
#include "Sensor.hh"
#include "TrackHeed.hh"
#include "Plotting.hh"
#include "Random.hh"
#include "ViewGeometry.hh"
#include "ViewField.hh"
#include "ViewSignal.hh"
#include "ViewCell.hh"
#include "AvalancheMicroscopic.hh"
#include "AvalancheMC.hh"
#include "ViewDrift.hh"
#include "DriftLineRKF.hh"
using namespace Garfield;

int main(int argc, char * argv[]) {

  randomEngine.Seed(123456);
  TApplication app("app", &argc, argv);
  plottingEngine.SetDefaultStyle();


   // Histograms that for cluster-size probability distribution 
  //TH1::StatOverflows(kTRUE); 
  TH1F* hElectrons = new TH1F("hElectrons", "Number of electrons",
                              200, 0, 200);
  TH1F* hEdep = new TH1F("hEdep", "Energy Loss",
                         100, 0., 10.);



//Defining variables
//---------------------------------------------------------- 
const double gaspress = 760;
const double  T = 293.15;

//Dimensions of the detector geometry
const double x = 10;
const double y = 80;
const double z = 40;

//Voltages for planes and wires 
const double HV = -3500;
const double v1 = 0;
const double Vwire = 600;
 const double rWire = 25.e-4; 
//Wire Specifications
const double diam = 0.003;
const double length = 80;


//Area of interest specifications
const double xmin = 4;
const double ymin =10;
const double zmin = 3;
const double xmax = 10;
const double ymax = 70;
const double zmax = 10;

  // Make a medium ---------------------------------------------------------------------
  MediumMagboltz* gas= new MediumMagboltz();
  gas->SetComposition("ar",90,"co2",10);
  gas->SetPressure(gaspress);
  gas->SetTemperature(T);
  gas->EnableDrift();
  gas->LoadGasFile("argon_90_Co2_10.gas");
  const std::string garfpath = std::getenv("GARFIELD_HOME");
  gas->LoadIonMobility(garfpath + "/Data/IonMobility_Ar+_Ar.txt");
  gas->PrintGas();


//Defining the detector geometry
  GeometrySimple* geom = new GeometrySimple();
  SolidBox* box = new SolidBox(0.,0.,0.,5.,40.,20);
  geom->AddSolid(box,gas);


//Component class for defining the electric field and adding the components of the detector 
   ComponentAnalyticField* comp = new ComponentAnalyticField();
   comp->AddPlaneY(10.,HV,"plane1");
   comp->AddPlaneY(0.,v1,"plane2");
   comp->AddStripOnPlaneY('x',0.,0.,0.3,"strip1");
   comp->AddStripOnPlaneY('x',0.,0.4,0.6,"strip2");
   comp->AddStripOnPlaneY('x',0.,0.7,0.9,"strip3");
   comp->AddWire(0.,6.,0.03,Vwire,"wire1",100.,50.,19.3);
   comp->SetGeometry(geom);
   comp->AddReadout("strip1");
   comp->AddReadout("strip2");
   comp->AddReadout("strip3");
   //comp->AddReadout("plane2");


//The sensor

   Sensor* sens = new Sensor();
   sens->AddComponent(comp);
   sens->AddElectrode(comp,"strip1");


  Sensor* sensor = new Sensor();
  sensor->AddComponent(comp);
  sensor->AddElectrode(comp,"strip2");

  Sensor* sensor_i = new Sensor();
  sensor_i->AddComponent(comp);
  sensor_i->AddElectrode(comp,"strip3");

/* This section allows one to set the time window for how long the electrons should be collected 
const double tMin = 0.;
const double tMax = 500.;
const double tStep = 0.04;
const int nTimeBins = int((tMax - tMin) / tStep);
sens->SetTimeWindow(0., tStep, nTimeBins);
sensor->SetTimeWindow(0., tStep, nTimeBins);
sensor_i->SetTimeWindow(0., tStep, nTimeBins);
*/




//Preparing the driftlines 
  DriftLineRKF* driftline = new DriftLineRKF();
  driftline->SetSensor(sens);
  driftline->DriftElectron(0.,0.2,1,0); 
  

DriftLineRKF* driftlin = new DriftLineRKF();
  driftlin->SetSensor(sensor);
  driftlin->DriftElectron(0.1,0.5,0,0); 

DriftLineRKF* drift= new DriftLineRKF();
  drift->SetSensor(sensor_i);
  drift->DriftElectron(-0.1,0.9,1,0); 


 //Tracking the particles
 TrackHeed *track = new TrackHeed();
 track->SetParticle("p");
 track->SetKineticEnergy(20.e8);
 track->SetSensor(sens);
 track->SetSensor(sensor);
 track->SetSensor(sensor_i);
 //track->NewTrack(2.,8.,2.,0.,10.,3.,0.);
 
 
 //defines the start and endpoint of a track, this loop is for getting the cluster-size and its distribution from the energy loss spectra for 10000 events
 const int nEvents = 10000;
  track->EnableDebugging();
  for (int i = 0; i < nEvents; ++i) {
    if (i == 1) track->DisableDebugging();
    if (i % 1000 == 0) std::cout << i << "/" << nEvents << "\n";
    // Initial position and direction 
    double x0 = 0., y0 = 0., z0 = 0., t0 = 0.;
    double dx0 = 1., dy0 = 0., dz0 = 0.; 
     track->NewTrack(2.,8.,2.,0.,0.,0.,1.);
    // Cluster coordinates
    double xc = 0., yc = 0., zc = 0., tc = 0.;
    // Number of electrons produced in a collision
    int nc = 0;
    // Energy loss in a collision
    double ec = 0.;
    // Dummy variable (not used at present)
    double extra = 0.;
    // Total energy loss along the track
    double esum = 0.;
    // Total number of electrons produced along the track
    int nsum = 0;
    // Loop over the clusters.
    while (track->GetCluster(xc, yc, zc, tc, nc, ec, extra)) {
      esum += ec;
      nsum += nc;

   
    //track->EnableDeltaElectronTransport();
    hElectrons->Fill(nsum);
    hEdep->Fill(esum * 1.e-3);
  }
    }



const bool holes = false;
AvalancheMicroscopic* aval = new AvalancheMicroscopic();
aval->SetSensor(sensor);
//aval->SetTimeSteps(0.05);
aval->AvalancheElectron(0.,0.0125,0.,0,0.1,0.,1.,0);
aval->EnableSignalCalculation();
//aval->SetCollisionSteps(10.);

//Plotting Section------------------------------------------------------------------

 bool plotCell = true;
  //bool plotCell = false;
  // Create a viewer for cell
  if (plotCell){  
  	ViewCell* viewc = new ViewCell();
  	viewc->SetComponent(comp);  		// Set the pointer to the component.
  	TCanvas* showcell = new TCanvas();
  	viewc->SetCanvas(showcell);
  	viewc->Plot3d();  // Make a two-dimensional plot of the cell layout.
  }
 bool plotField = true;
  //bool plotField = false;
  // Now I want to visualize the field of the component
  if (plotField){  
	ViewField* viewf = new ViewField();
	viewf->SetComponent(comp);
	TCanvas* showfield = new TCanvas();
	viewf->SetCanvas(showfield);
	viewf->SetElectricFieldRange(-500., 1000.);
	//viewf->PlotSurface("ey");  //ex,v
	//viewf->PlotProfile(-10,0,0,10,0,0,"e");
	viewf->PlotContour("ey");    	 	  
  }

 bool plotSignal = true;
  //bool plotSignal = false;
  // Now I want to visualize the signal
  if (plotSignal){  
	ViewSignal* signalView = new ViewSignal();
	signalView->SetSensor(sens);
	signalView->PlotSignal("strip1");
  }

 bool plotsignal = true;
  //bool plotSignal = false;
  // Now I want to visualize the signal
  if (plotsignal){  
	ViewSignal* signal = new ViewSignal();
	signal->SetSensor(sensor);
	signal->PlotSignal("strip2");
  }

 bool PlotSignal = true;
  //bool plotSignal = false;
  // Now I want to visualize the signal
  if (PlotSignal){  
	ViewSignal* signalV = new ViewSignal();
	signalV->SetSensor(sensor_i);
	signalV->PlotSignal("strip3");
  }

TCanvas* c1 = new TCanvas();
  hElectrons->GetXaxis()->SetTitle("number of electrons"); 
  hElectrons->Draw();
  c1->SaveAs("ne.pdf");

  TCanvas* c2 = new TCanvas();
  hEdep->GetXaxis()->SetTitle("energy loss [keV]");
  hEdep->Draw();
  c2->SaveAs("edep.pdf");


const bool plotDrift = true;
     ViewDrift* driftView = new ViewDrift();
     if (plotDrift) {
         driftView->SetArea(-0.5,8.,0,0.5,10,0.1);
         driftlin->EnablePlotting(driftView);
         aval->EnablePlotting(driftView);
         driftView ->Plot();    
     }
/*
TCanvas* canvas = new TCanvas();
ViewSignal* signalView = new ViewSignal();
signalView->SetSensor(sens);
//signalView->SetSensor(sensor);
//signalView->SetSensor(sensor_i);
//signalView->PlotSignal("strip1");
//signalView->PlotSignal("strip2");
signalView->PlotSignal("plane2");
canvas->Modified();
canvas->Update();
*/
app.Run(kTRUE);

}
