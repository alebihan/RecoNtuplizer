// -*- C++ -*-
//
// Package:    TreeMaker/Ntuplzr
// Class:      Validator
// 
/**\class Validator Validator.cc TreeMaker/Validator/plugins/Validator.cc
/
/Description: [one line class summary]
/
/Implementation:
/    [Notes on implementation]
*/
//
// Original Author:  Sandhya Jain
//         Created:  Wed, 29 Mar 2017 05:29:37 GMT
//
//


// system include files
#include <memory>
#include <iostream>
// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "RecoEgamma/EgammaTools/interface/ConversionTools.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include "PhysicsTools/SelectorUtils/interface/PFJetIDSelectionFunctor.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "RecoVertex/VertexPrimitives/interface/TransientVertex.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include "SimTracker/Records/interface/TrackAssociatorRecord.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/ME0EtaPartitionSpecs.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/ParticleMass.h"
#include <RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h>
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicVertex.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleFitter.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "Math/GenVector/VectorUtil.h"

using namespace std;
using namespace reco;
//
/// class declaration
///
//
/// If the analyzer does not use TFileService, please remove
/// the template argument to the base class so the class inherits
/// from  edm::one::EDAnalyzer<> and also remove the line from
/// constructor "usesResource("TFileService");"
/// This will improve performance in multithreaded jobs.
const Int_t kMaxVertices = 300;
const Int_t kMaxWeights = 1500;
const Int_t kMaxParticle = 10000;
const Int_t kMaxGenJet = 800;
const Int_t kMaxPhoton = 800;
const Int_t kMaxElectron = 200;
const Int_t kMaxMuonLoose = 200;
const Int_t kMaxJet = 800;
const Int_t kMaxTau = 200;
const Int_t kMaxMissingET = 1;


class Validator : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit Validator(const edm::ParameterSet&);
  ~Validator();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

  
private:
  virtual void beginJob() override;
  virtual void beginRun(edm::Run const&, edm::EventSetup const&) ;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endRun(edm::Run const&, edm::EventSetup const&) ;
  virtual void endJob() override;
  
  bool isME0MuonSelNew(reco::Muon, double, double, double, edm::EventSetup const& );
  float calculate_demetraIsolation(const pat::Tau&) const;
  bool debug_;
  edm::Service<TFileService> fs_;
  edm::EDGetTokenT<std::vector<reco::Vertex>>      verticesToken_     ;
  edm::EDGetTokenT<std::vector<reco::GenParticle>> genPartsToken_     ;
  edm::EDGetTokenT<std::vector<reco::GenJet>>      genJetsToken_      ;
  edm::EDGetTokenT<std::vector<pat::Photon>>       photnsToken_       ; 
  edm::EDGetTokenT<std::vector<pat::Electron>>     elecsToken_        ;
  edm::EDGetTokenT<std::vector<pat::Muon>>         muonsToken_        ;
  edm::EDGetTokenT<std::vector<pat::Tau>>          tausToken_         ;
  edm::EDGetTokenT<std::vector<pat::Jet>>          jetsToken_         ;
  edm::EDGetTokenT<std::vector<pat::MET>>          metToken_          ;
  //edm::EDGetTokenT<std::vector<reco::Conversion>>  convToken_         ;
  
  const ME0Geometry*      ME0Geometry_;
  
  TTree* mytree;
  int evt_size;
  
  int vtx_size;
  float vtx_pt2[kMaxVertices];
  
  int genpart_size;
  float genpart_pt[kMaxParticle],genpart_eta[kMaxParticle],genpart_phi[kMaxParticle],genpart_mass[kMaxParticle];
  int genpart_pid[kMaxParticle], genpart_status[kMaxParticle], genpart_m1[kMaxParticle], genpart_m2[kMaxParticle], genpart_d1[kMaxParticle], genpart_d2[kMaxParticle] ;
  
  int genjet_size;
  float genjet_pt[kMaxGenJet], genjet_eta[kMaxGenJet], genjet_phi[kMaxGenJet], genjet_mass[kMaxGenJet];
  
  int gamma_size;
  float MVAgamma_[kMaxPhoton],gamma_pt[kMaxPhoton], gamma_eta[kMaxPhoton], gamma_phi[kMaxPhoton],gamma_mass[kMaxPhoton],gamma_reliso[kMaxPhoton], gamma_idvar[kMaxPhoton];
  uint32_t gamma_isopass[kMaxPhoton], gamma_idpass[kMaxPhoton];
  
  int elec_size, elec_charge[kMaxElectron];
  float elec_pt[kMaxElectron], elec_eta[kMaxElectron], elec_phi[kMaxElectron], elec_reliso[kMaxElectron], elec_mass[kMaxElectron], elec_idvar[kMaxElectron];
  uint32_t elec_isopass[kMaxElectron],elec_idpass[kMaxElectron];
  
  int muon_size, muon_charge[kMaxMuonLoose];
  float muon_pt[kMaxMuonLoose], muon_eta[kMaxMuonLoose], muon_phi[kMaxMuonLoose], muon_reliso[kMaxMuonLoose], muon_mass[kMaxMuonLoose], muon_idvar[kMaxMuonLoose];
  uint32_t muon_isopass[kMaxMuonLoose], muon_idpass[kMaxMuonLoose] ;
  
  
  int tau_size, tau_charge[kMaxTau];
  float tau_decaymode[kMaxTau], tau_combinediso[kMaxTau], tau_pt[kMaxTau], tau_eta[kMaxTau], tau_phi[kMaxTau], tau_mass[kMaxTau], tau_isofunction[kMaxTau], tau_neutraliso[kMaxTau], tau_chargediso[kMaxTau];
  uint32_t tau_combinedisopass[kMaxTau];
  
  int jet_size; 
  float jet_pt[kMaxJet], jet_eta[kMaxJet], jet_phi[kMaxJet], jet_mass[kMaxJet];
  uint32_t jet_idpass[kMaxJet];
  float jet_bMVA[kMaxJet];
  float jet_DeepCSV[kMaxJet];
  float jet_DeepJET[kMaxJet];
  
  int met_size;
  float met_pt[kMaxMissingET],met_phi[kMaxMissingET];
};



Validator::Validator(const edm::ParameterSet& iConfig):
  debug_(iConfig.getParameter<bool>("debug")),
  verticesToken_(consumes<std::vector<reco::Vertex>>(iConfig.getParameter<edm::InputTag>("vertices"))),
  genPartsToken_(consumes<std::vector<reco::GenParticle>>(iConfig.getParameter<edm::InputTag>("genParts"))),
  genJetsToken_(consumes<std::vector<reco::GenJet>>(iConfig.getParameter<edm::InputTag>("genJets"))),
  photnsToken_(consumes<std::vector<pat::Photon>>(iConfig.getParameter<edm::InputTag>("photons"))),
  elecsToken_(consumes<std::vector<pat::Electron>>(iConfig.getParameter<edm::InputTag>("electrons"))),
  muonsToken_(consumes<std::vector<pat::Muon>>(iConfig.getParameter<edm::InputTag>("muons"))),
  tausToken_(consumes<std::vector<pat::Tau>>(iConfig.getParameter<edm::InputTag>("taus"))),
  jetsToken_(consumes<std::vector<pat::Jet>>(iConfig.getParameter<edm::InputTag>("jets"))),
  metToken_(consumes<std::vector<pat::MET>>(iConfig.getParameter<edm::InputTag>("met")))
  { 
  
  
    if(debug_)  std::cout<<"Here I am : in constructor "<<std::endl;
    
    ME0Geometry_ = 0;
    evt_size     = 0;
    
    usesResource("TFileService");
    mytree   = fs_->make<TTree>("mytree","TestTree");
    mytree->Branch("evt_size",&evt_size, "evt_size/I");
    mytree->Branch("vtx_size",&vtx_size, "vtx_size/I");
    mytree->Branch("vtx_pt2",vtx_pt2, "vtx_pt2[vtx_size]/F");
    
    mytree->Branch("genpart_size",&genpart_size, "genpart_size/I");
    mytree->Branch("genpart_pid", genpart_pid, "genpart_pid[genpart_size]/I");
    mytree->Branch("genpart_status",genpart_status, "genpart_status[genpart_size]/I");
    mytree->Branch("genpart_pt",genpart_pt, "genpart_pt[genpart_size]/F");
    mytree->Branch("genpart_eta",genpart_eta, "genpart_eta[genpart_size]/F");
    mytree->Branch("genpart_phi",genpart_phi, "genpart_phi[genpart_size]/F");
    mytree->Branch("genpart_mass",genpart_mass, "genpart_mass[genpart_size]/F");
    mytree->Branch("genpart_m1",genpart_m1, "genpart_m1[genpart_size]/I");
    mytree->Branch("genpart_m2",genpart_m2, "genpart_m2[genpart_size]/I");
    mytree->Branch("genpart_d1",genpart_d1, "genpart_d1[genpart_size]/I");
    mytree->Branch("genpart_d2",genpart_d2, "genpart_d2[genpart_size]/I");
    
    mytree->Branch("genjet_size",&genjet_size, "genjet_size/I");
    mytree->Branch("genjet_pt",genjet_pt, "genjet_pt[genjet_size]/F");
    mytree->Branch("genjet_eta",genjet_eta, "genjet_eta[genjet_size]/F");
    mytree->Branch("genjet_phi",genjet_phi, "genjet_phi[genjet_size]/F");
    mytree->Branch("genjet_mass",genjet_mass, "genjet_mass[genjet_size]/F");
    
    mytree->Branch("gamma_size",&gamma_size, "gamma_size/I");
    mytree->Branch("gamma_pt",gamma_pt, "gamma_pt[gamma_size]/F");
    mytree->Branch("gamma_eta",gamma_eta, "gamma_eta[gamma_size]/F");
    mytree->Branch("gamma_phi",gamma_phi, "gamma_phi[gamma_size]/F");
    mytree->Branch("gamma_mass",gamma_mass, "gamma_mass[gamma_size]/F");
    mytree->Branch("gamma_idvar", gamma_idvar, "gamma_idvar[gamma_size]/F");
    mytree->Branch("gamma_reliso",gamma_reliso, "gamma_reliso[gamma_size]/F");
    mytree->Branch("gamma_idpass", gamma_idpass, "gamma_idpass[gamma_size]/i");
    mytree->Branch("gamma_isopass", gamma_isopass, "gamma_isopass[gamma_size]/i");
    
    
    mytree->Branch("elec_size",&elec_size, "elec_size/I");
    mytree->Branch("elec_pt",elec_pt, "elec_pt[elec_size]/F");
    mytree->Branch("elec_eta",elec_eta, "elec_eta[elec_size]/F");
    mytree->Branch("elec_phi",elec_phi, "elec_phi[elec_size]/F");
    mytree->Branch("elec_mass",elec_mass, "elec_mass[elec_size]/F");
    mytree->Branch("elec_charge",elec_charge, "elec_charge[elec_size]/I");
    mytree->Branch("elec_idvar", elec_idvar, "elec_idvar[elec_size]/F");
    mytree->Branch("elec_reliso",elec_reliso, "elec_reliso[elec_size]/F");
    mytree->Branch("elec_idpass",elec_idpass, "elec_idpass[elec_size]/i");
    mytree->Branch("elec_isopass", elec_isopass, "elec_isopass[elec_size]/i");
    
    mytree->Branch("muon_size",&muon_size, "muon_size/I");
    mytree->Branch("muon_pt",muon_pt, "muon_pt[muon_size]/F");
    mytree->Branch("muon_eta",muon_eta, "muon_eta[muon_size]/F");
    mytree->Branch("muon_phi",muon_phi, "muon_phi[muon_size]/F");
    mytree->Branch("muon_mass",muon_mass, "muon_mass[muon_size]/F");
    mytree->Branch("muon_charge",muon_charge, "muon_charge[muon_size]/I");
    mytree->Branch("muon_idvar", muon_idvar, "muon_idvar[muon_size]/F");
    mytree->Branch("muon_reliso",muon_reliso, "muon_reliso[muon_size]/F");
    mytree->Branch("muon_idpass", muon_idpass, "muon_idpass[muon_size]/i");
    mytree->Branch("muon_isopass", muon_isopass, "muon_isopass[muon_size]/i");
    
    mytree->Branch("tau_size",&tau_size, "tau_size/I");
    mytree->Branch("tau_pt",tau_pt, "tau_pt[tau_size]/F");
    mytree->Branch("tau_eta",tau_eta, "tau_eta[tau_size]/F");
    mytree->Branch("tau_phi",tau_phi, "tau_phi[tau_size]/F");
    mytree->Branch("tau_mass",tau_mass, "tau_mass[tau_size]/F");
    mytree->Branch("tau_charge",tau_charge, "tau_charge[tau_size]/I");
    mytree->Branch("tau_decaymode",tau_decaymode, "tau_decaymode[tau_size]/F");
    mytree->Branch("tau_combinediso",tau_combinediso, "tau_combinediso[tau_size]/F");
    mytree->Branch("tau_chargediso",tau_chargediso, "tau_chargediso[tau_size]/F");
    mytree->Branch("tau_neutraliso",tau_neutraliso, "tau_neutraliso[tau_size]/F");
    mytree->Branch("tau_isofunction",tau_isofunction, "tau_isofunction[tau_size]/F");
    mytree->Branch("tau_combinedisopass", tau_combinedisopass, "tau_combinedisopass[tau_size]/i");
    
    
    mytree->Branch("jet_size",&jet_size, "jet_size/I");
    mytree->Branch("jet_pt",jet_pt, "jet_pt[jet_size]/F");
    mytree->Branch("jet_eta",jet_eta, "jet_eta[jet_size]/F");
    mytree->Branch("jet_phi",jet_phi, "jet_phi[jet_size]/F");
    mytree->Branch("jet_mass",jet_mass, "jet_mass[jet_size]/F");
    mytree->Branch("jet_idpass", jet_idpass, "jet_idpass[jet_size]/i");
    mytree->Branch("jet_bmva",jet_bMVA, "jet_bmva[jet_size]/F");
    mytree->Branch("jet_DeepCSV",jet_DeepCSV,"jet_DeepCSV[jet_size]/F");
    mytree->Branch("jet_DeepJET",jet_DeepJET,"jet_DeepJET[jet_size]/F");
    
    
    mytree->Branch("met_size",&met_size, "met_size/I");
    mytree->Branch("met_pt", met_pt, "met_pt[met_size]/F");
    mytree->Branch("met_phi",met_phi, "met_phi[met_size]/F");
    if(debug_)      std::cout<<"Here I am : ending constructor "<<std::endl;
}
  
  


Validator::~Validator()
{}

//
// member functions
//

// ------------ method called for each event  ------------
void 
Validator::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  if(debug_) std::cout<<"here starts the event:"<<std::endl;
  
  Handle<std::vector<reco::Vertex>> vertices;
  iEvent.getByToken(verticesToken_, vertices);

  Handle<std::vector<reco::GenParticle>> genParts;
  iEvent.getByToken(genPartsToken_, genParts);

  Handle<std::vector<reco::GenJet>> genJets;
  iEvent.getByToken(genJetsToken_, genJets);

  Handle<std::vector<pat::Photon>> photns;
  iEvent.getByToken(photnsToken_, photns);

  Handle<std::vector<pat::Electron>> elecs;
  iEvent.getByToken(elecsToken_, elecs);

  Handle<std::vector<pat::Muon>> muons;
  iEvent.getByToken(muonsToken_, muons);

  Handle<std::vector<pat::Tau>> taus;
  iEvent.getByToken(tausToken_, taus);

  Handle<std::vector<pat::Jet>> jets;
  iEvent.getByToken(jetsToken_, jets);

  Handle<std::vector<pat::MET>> met;
  iEvent.getByToken(metToken_, met);
  
  if(debug_) std::cout<<"Here I am : got handles right "<<std::endl;  
  vtx_size      = 0;
  genjet_size   = 0;
  genpart_size  = 0;
  elec_size     = 0;
  jet_size      = 0;
  muon_size     = 0;
  met_size      = 0;
  gamma_size    = 0;
  tau_size      = 0;

  if(debug_) std::cout<<"Here I am : initalised number of particles=0 in the event "<<std::endl;  

  evt_size++;


  /////////////////////////////
  //////vertices info//////
  /////////////////////////////

  int prVtx = -1;
  for (size_t i = 0; i < vertices->size(); i++) {
    if (vertices->at(i).isFake()) continue;
    if (vertices->at(i).ndof() <= 4) continue;
    if (prVtx < 0) prVtx = i;
    vtx_pt2[vtx_size] = vertices->at(i).p4().pt();
    vtx_size++;
  }
  if (prVtx < 0) return;
  if(debug_)  std::cout<<"Here I am : got vertex infor right "<<std::endl;


  
  
  
  /////////////////////////////
  //////Gen Particle info//////
  /////////////////////////////
  vector<const reco::Candidate *> vectorCandidate;
  vector<const reco::Candidate *>::iterator itCandidate;

  for( vector<reco::GenParticle>::const_iterator itPackedParticle = genParts->begin(); itPackedParticle != genParts->end(); ++itPackedParticle)
    {
      vectorCandidate.push_back(&*itPackedParticle);
    }
  
  //GenParticle information
  for (size_t i = 0; i < genParts->size(); i++) {

    genpart_pid[genpart_size]       = genParts->at(i).pdgId();
    genpart_status[genpart_size]    = genParts->at(i).status();
    genpart_pt[genpart_size]        = genParts->at(i).pt();
    genpart_phi[genpart_size]       = genParts->at(i).phi();
    genpart_eta[genpart_size]       = genParts->at(i).eta();
    genpart_mass[genpart_size]      = genParts->at(i).mass();
    if(genParts->at(i).mother())
      {
	itCandidate = find(vectorCandidate.begin(), vectorCandidate.end(), genParts->at(i).mother());
	if(itCandidate != vectorCandidate.end())
	  {
	    genpart_m1[genpart_size] = distance(vectorCandidate.begin(), itCandidate);
	    genpart_m2[genpart_size] = distance(vectorCandidate.begin(), itCandidate);
	  }
	
      }
    else
      {
	genpart_m1[genpart_size] =-99;
	genpart_m2[genpart_size] =-99;
      }
    
    
    itCandidate = find(vectorCandidate.begin(), vectorCandidate.end(), genParts->at(i).daughter(0));
    if(itCandidate != vectorCandidate.end()) genpart_d1[genpart_size] = distance(vectorCandidate.begin(), itCandidate);
    
    itCandidate = find(vectorCandidate.begin(), vectorCandidate.end(), genParts->at(i).daughter(genParts->at(i).numberOfDaughters() - 1));
    if(itCandidate != vectorCandidate.end()) genpart_d2[genpart_size] = distance(vectorCandidate.begin(), itCandidate);
    genpart_size++;
    if(genpart_size>kMaxParticle) break;       
  }
  if(debug_)   std::cout<<"Here I am : got genpart infor right "<<std::endl;
  
  
  /////////////////////////////
  //GenJet information
  /////////////////////////////
  std::vector<size_t> jGenJets;                                            
  for(size_t ij= 0 ; ij < genJets->size(); ij++)
    {
      if (genJets->at(ij).pt() < 20.) continue;
      if (fabs(genJets->at(ij).eta()) > 5) continue;
      
      bool overlaps = false;
      for (size_t j = 0; j < genParts->size(); j++) {
	if (abs(genParts->at(j).pdgId()) != 11 && abs(genParts->at(j).pdgId()) != 13) continue;
	if (fabs(genJets->at(ij).pt()-genParts->at(j).pt()) < 0.01*genParts->at(j).pt() && ROOT::Math::VectorUtil::DeltaR(genParts->at(j).p4(),genJets->at(ij).p4()) < 0.01) {
	  overlaps = true;
	  break;
	}
      }
      if (overlaps) continue;
      jGenJets.push_back(ij);
      
      genjet_pt[genjet_size]  = genJets->at(ij).pt();
      genjet_eta[genjet_size] = genJets->at(ij).eta();
      genjet_phi[genjet_size] = genJets->at(ij).phi();
      genjet_mass[genjet_size]= genJets->at(ij).mass();
      genjet_size++;

      if(genjet_size>kMaxGenJet) break;       
    }
  if(debug_)   std::cout<<"Here I am : got genjet infor right "<<std::endl;
  
  /////////////////////////////
  //Photon information         
  /////////////////////////////                                                                                                
  for(size_t ip= 0 ; ip < photns->size(); ip++)
    {
      if(photns->at(ip).pt() < 10.) continue;
      if(fabs(photns->at(ip).eta()) > 3.) continue;
      float mvaValue = photns->at(ip).userFloat("mvaValue");
      bool isEB = photns->at(ip).isEB();
      bool isLoose(0), isMedium(0), isTight(0);
      
      if( isEB )
	{
	  isLoose  = (mvaValue > 0.00);
	  isMedium = (mvaValue > 0.2); 
	  isTight  = (mvaValue > 0.56);
	}     
      else
	{
	  isLoose = (mvaValue > 0.20);
	  isMedium = (mvaValue > 0.4); 
	  isTight = (mvaValue > 0.68);
	}          
      
      gamma_pt[gamma_size]        = photns->at(ip).pt();
      gamma_eta[gamma_size]       = photns->at(ip).eta();
      gamma_phi[gamma_size]       = photns->at(ip).phi();
      gamma_mass[gamma_size]      = photns->at(ip).mass();
      gamma_idvar[gamma_size]     = mvaValue; // MVA
      gamma_reliso[gamma_size] = 1.;
      
      
      if(isLoose)
	gamma_idpass[gamma_size] |= 1 << 0;
      
      if(isMedium)
	gamma_idpass[gamma_size] |= 1 << 1;
      
      if(isTight)
	gamma_idpass[gamma_size] |= 1 << 2;
      
      if(gamma_reliso[gamma_size] < 0.1)
	gamma_isopass[gamma_size] |= 1 << 0;
      
      if(gamma_reliso[gamma_size] < 0.2)
	 gamma_isopass[gamma_size] |= 1 << 1;
      
      if(gamma_reliso[gamma_size] < 0.3)
	gamma_isopass[gamma_size] |= 1 << 2;
      
      if(gamma_reliso[gamma_size] < 0.4)
	gamma_isopass[gamma_size] |= 1 << 3;
      
      gamma_size++;
      if(gamma_size>kMaxPhoton) break;
    }
  if(debug_)   std::cout<<"Here I am : got pho infor right "<<std::endl;
  
  /////////////////////////////
  //Electron information
  /////////////////////////////                       
  for(size_t ie= 0 ; ie < elecs->size(); ie++)  {
    if(elecs->at(ie).pt() < 10.) continue;
    if (fabs(elecs->at(ie).eta()) > 3.) continue;
    float mvaValue = elecs->at(ie).userFloat("mvaValue");
    elec_pt[elec_size]               = elecs->at(ie).pt();
    elec_eta[elec_size]              = elecs->at(ie).eta();
    elec_phi[elec_size]              = elecs->at(ie).phi();
    elec_mass[elec_size]             = elecs->at(ie).mass();
    elec_charge[elec_size]           = elecs->at(ie).charge();
    elec_idvar[elec_size]            = mvaValue; //MVA
    bool isEB = elecs->at(ie).isEB();
    if(isEB) 
      elec_reliso[elec_size] = (elecs->at(ie).puppiNoLeptonsChargedHadronIso() + elecs->at(ie).puppiNoLeptonsNeutralHadronIso() + elecs->at(ie).puppiNoLeptonsPhotonIso()) / elecs->at(ie).pt();
    else 
      elec_reliso[elec_size] = (elecs->at(ie).userFloat("hgcElectronID:caloIsoRing1") + elecs->at(ie).userFloat("hgcElectronID:caloIsoRing2") + elecs->at(ie).userFloat("hgcElectronID:caloIsoRing3") + elecs->at(ie).userFloat("hgcElectronID:caloIsoRing4")) / elecs->at(ie).energy();

    bool isLoose(0), isMedium(0), isTight(0);
    
    if( isEB ) {
      if (elecs->at(ie).pt() < 20.) {
	isLoose = (mvaValue  > -0.661);
	isMedium = (mvaValue > 0.885);
	isTight = (mvaValue  > 0.986);
     	
	
      }
      else {
	isLoose = (mvaValue  > -0.797);
	isMedium = (mvaValue > 0.723);
	isTight = (mvaValue  > 0.988);
      }
    }
    else {
      if (not (elecs->at(ie).userFloat("hgcElectronID:ecEnergy") > 0)) continue;
      if (not (elecs->at(ie).userFloat("hgcElectronID:sigmaUU") > 0)) continue;
      if (not (elecs->at(ie).fbrem() > -1)) continue;
      if (not (elecs->at(ie).userFloat("hgcElectronID:measuredDepth") < 40)) continue;
      if (not (elecs->at(ie).userFloat("hgcElectronID:nLayers") > 20)) continue;
      if (elecs->at(ie).pt() < 20.) {
	isLoose = (mvaValue  > -0.320);
	isMedium = (mvaValue > 0.777);
	isTight = (mvaValue  > 0.969);
      }
      else {
	isLoose = (mvaValue  > -0.919);
	isMedium = (mvaValue > 0.591);
	isTight = (mvaValue  > 0.983);
      }
    }
    
    if(isLoose)
      elec_idpass[elec_size] |= 1 << 0;
    
    if(isMedium)
      elec_idpass[elec_size] |= 1 << 1;
    
    if(isTight)
      elec_idpass[elec_size] |= 1 << 2;
    
    
    if(elec_reliso[elec_size] < 0.1)
      elec_isopass[elec_size] |= 1 << 0;
    
    if(elec_reliso[elec_size] < 0.2)
      elec_isopass[elec_size] |= 1 << 1;
    
    if(elec_reliso[elec_size] < 0.3)
      elec_isopass[elec_size] |= 1 << 2;
    
    if(elec_reliso[elec_size] < 0.4)
      elec_isopass[elec_size] |= 1 << 3;
    
    elec_size++;
    if(elec_size>kMaxElectron) break;
  }
  
  //std::cout<<elec_size<<std::endl;
  if(debug_)   std::cout<<"Here I am : got elec infor right "<<std::endl;
  
  /////////////////////////////
  // Muon information
  /////////////////////////////
  for(size_t im= 0 ; im < muons->size(); im++)
    {
      if (muons->at(im).pt() < 2.) continue;
      if (fabs(muons->at(im).eta()) > 2.8) continue;
      
      muon_pt[muon_size]               = muons->at(im).pt();
      muon_eta[muon_size]              = muons->at(im).eta();
      muon_phi[muon_size]              = muons->at(im).phi();
      muon_mass[muon_size]             = muons->at(im).mass();
      muon_charge[muon_size]           = muons->at(im).charge();
      muon_reliso[muon_size]           = muons->at(im).trackIso()/muons->at(im).pt();
      muon_idvar[muon_size]            = 1.0;
      double dPhiCut = std::min(std::max(1.2/muons->at(im).p(),1.2/100),0.056);
      double dPhiBendCut = std::min(std::max(0.2/muons->at(im).p(),0.2/100),0.0096);
      
      int isLoose = (int) (fabs(muons->at(im).eta()) < 2.4 && muon::isLooseMuon(muons->at(im))) || (fabs(muons->at(im).eta()) > 2.4 && isME0MuonSelNew(muons->at(im), 0.077, dPhiCut, dPhiBendCut, iSetup));
      if(isLoose)
	muon_idpass[muon_size] |= 1 << 0;
      
      //// isLoose = isMedium
      int isMedium = isLoose;
      if(isMedium)
	muon_idpass[muon_size] |= 1 << 1;
      
      
      bool ipxy = false, ipz = false, validPxlHit = false, highPurity = false;
      if (muons->at(im).innerTrack().isNonnull()){
	ipxy = std::abs(muons->at(im).muonBestTrack()->dxy(vertices->at(prVtx).position())) < 0.2;
	ipz = std::abs(muons->at(im).muonBestTrack()->dz(vertices->at(prVtx).position())) < 0.5;
	validPxlHit = muons->at(im).innerTrack()->hitPattern().numberOfValidPixelHits() > 0;
	highPurity = muons->at(im).innerTrack()->quality(reco::Track::highPurity);
      }      
      dPhiCut = std::min(std::max(1.2/muons->at(im).p(),1.2/100),0.032);
      dPhiBendCut = std::min(std::max(0.2/muons->at(im).p(),0.2/100),0.0041);
      int isTight = (int) (fabs(muons->at(im).eta()) < 2.4 && vertices->size() > 0 && muon::isTightMuon(muons->at(im),vertices->at(prVtx))) || (fabs(muons->at(im).eta()) > 2.4 && isME0MuonSelNew(muons->at(im), 0.048, dPhiCut, dPhiBendCut, iSetup) && ipxy && ipz && validPxlHit && highPurity);
      if(isTight)
	muon_idpass[muon_size] |= 1 << 2;
      
      
      
      //CutBasedIdLoosemuon[muon_size] = muons->at(im).passed(reco::Muon::CutBasedIdLoose);
      //CutBasedIdMediummuon[muon_size] = muons->at(im).passed(reco::Muon::CutBasedIdMedium);
      //CutBasedIdTightmuon[muon_size] = muons->at(im).passed(reco::Muon::CutBasedIdTight);
      //PFIsoLoosemuon[muon_size] = muons->at(im).passed(reco::Muon::PFIsoLoose);
      //PFIsoMediummuon[muon_size] = muons->at(im).passed(reco::Muon::PFIsoMedium);
      //
      //PFIsoTightmuon[muon_size] = muons->at(im).passed(reco::Muon::PFIsoTight);
      
      
      
      if(muon_reliso[muon_size] < 0.1)
	muon_isopass[muon_size] |= 1 << 0;
      
      if(muon_reliso[muon_size] < 0.2)
	muon_isopass[muon_size] |= 1 << 1;
      
      if(muon_reliso[muon_size] < 0.3)
	muon_isopass[muon_size] |= 1 << 2;
      
      if(muon_reliso[muon_size] < 0.4)
	muon_isopass[muon_size] |= 1 << 3;
      
      
      
      muon_size++;
      if(muon_size>kMaxMuonLoose) break;
    }
  
   if(debug_)   std::cout<<"Here I am : got muon infor right "<<std::endl;
   //std::cout<<"Muon"<<std::endl;
   
   /////////////////////////////
   // Taus info
   /////////////////////////////
   for (size_t it = 0; it < taus->size(); it++) {
     if (taus->at(it).pt()<15.) continue; 
     if (fabs(taus->at(it).eta()) > 3.0) continue;
     if (taus->at(it).tauID("decayModeFinding")<0) continue;     
     
     
     tau_pt[tau_size]          = taus->at(it).pt();
     tau_eta[tau_size]         = taus->at(it).eta();
     tau_phi[tau_size]         = taus->at(it).phi();
     tau_mass[tau_size]        = taus->at(it).mass();
     tau_charge[tau_size]      = taus->at(it).charge();
     tau_decaymode[tau_size]   = taus->at(it).decayMode();
     tau_chargediso[tau_size]  = taus->at(it).tauID("chargedIsoPtSum");
     tau_neutraliso[tau_size]  = taus->at(it).tauID("neutralIsoPtSumdR03");
     
     
     if (std::abs(tau_eta[tau_size])<1.4)
       tau_combinediso[tau_size]      = tau_chargediso[tau_size] + 0.2*max(0.,tau_neutraliso[tau_size] - 5.);
     else 
       tau_combinediso[tau_size]      = tau_chargediso[tau_size] + 0.2*max(0.,tau_neutraliso[tau_size] - 1.);
       
     tau_isofunction[tau_size] = calculate_demetraIsolation(taus->at(it));
     
     if(tau_combinediso[tau_size] < 1.2)
       tau_combinedisopass[tau_size] |= 1 << 0;
     
     if(tau_combinediso[tau_size] < 2.)
       tau_combinedisopass[tau_size] |= 1 << 1;

     if(tau_combinediso[tau_size] < 4.)
	 tau_combinedisopass[tau_size] |= 1 << 2;
     
     if(tau_combinediso[tau_size] < 5.)
       tau_combinedisopass[tau_size] |= 1 << 3;
     

     tau_size++;
     if(tau_size>kMaxTau) break; 
   }
   
   if(debug_)   std::cout<<"Here I am : got tau infor right "<<std::endl;
   
   
   /////////////////////////////
   //Jet information
   /////////////////////////////
   for(size_t ij= 0 ; ij < jets->size(); ij++)
     {
       //if (jets->at(ij).pt() < 20.) continue;
       //if (fabs(jets->at(ij).eta()) > 5) continue;
       //
       jet_pt[jet_size]  = jets->at(ij).pt();
       jet_eta[jet_size] = jets->at(ij).eta();
       jet_phi[jet_size] = jets->at(ij).phi();
       jet_mass[jet_size]= jets->at(ij).mass();

       bool isLoose(0), isMedium(0), isTight(0);
       if( (jets->at(ij).numberOfDaughters() > 1 ) && ( jets->at(ij).neutralEmEnergyFraction()< 0.99 ) && ( jets->at(ij).neutralHadronEnergyFraction() < 0.99 ))
       	 {
       	   isLoose = 1;
       	 }
       isMedium = isLoose;
       if( (jets->at(ij).numberOfDaughters() > 1 ) && ( jets->at(ij).neutralEmEnergyFraction()< 0.9 ) && ( jets->at(ij).neutralHadronEnergyFraction() < 0.9 ))
       	 {
	   isTight = 1;
	 }

       if(isLoose)
	 jet_idpass[jet_size] |= 1 << 0;
	   
       if(isMedium)
	 jet_idpass[jet_size] |= 1 << 1;

       if(isTight)
	 jet_idpass[jet_size] |= 1 << 2;
       
       jet_bMVA[jet_size]         = (float) jets->at(ij).bDiscriminator("pfCombinedMVAV2BJetTags");
//$$       jet_deepcsvtag1[jet_size]  = (float) jets->at(ij).bDiscriminator("pfDeepCSVJetTags:probb");
//$$       jet_deepcsvtag2[jet_size]  = (float) jets->at(ij).bDiscriminator("pfDeepCSVJetTags:probbb");
//$$
       float DeepCSVb   = (float) jets->at(ij).bDiscriminator("pfDeepCSVJetTags:probb");
       float DeepCSVbb  = (float) jets->at(ij).bDiscriminator("pfDeepCSVJetTags:probbb");
       jet_DeepCSV[jet_size]  = DeepCSVb + DeepCSVbb;

       float DeepJETb    = (float) jets->at(ij).bDiscriminator("pfDeepFlavourJetTags:probb");
       float DeepJETbb   = (float) jets->at(ij).bDiscriminator("pfDeepFlavourJetTags:probbb");
       float DeepJETlepb = (float) jets->at(ij).bDiscriminator("pfDeepFlavourJetTags:problepb");
       jet_DeepJET[jet_size]  = (DeepJETb > -5) ? DeepJETb + DeepJETbb + DeepJETlepb : -10;
//$$
          
       if(debug_)
	 {
	   std::cout<<"Btaggers::::::::pfCombinedMVAV2BJetTags/pfDeepCSVJetTags:probb/pfDeepCSVJetTags:probbb:::::::::::::"<<std::endl;
	   std::cout<<jet_bMVA[jet_size]<<" / "<< jet_DeepCSV[jet_size] <<" / "<< jet_DeepJET[jet_size] <<std::endl;
	 }
       jet_size++;
       if(jet_size>kMaxJet) break;
     }
   if(debug_)   std::cout<<"Here I am : got jet infor right "<<std::endl;
   

   /////////////////////////////
   //Met information
   /////////////////////////////
   for(size_t imet= 0 ; imet < met->size(); imet++)
     {
       met_pt[met_size]    = met->at(imet).pt();
       met_phi[met_size] = met->at(imet).phi();
       met_size++;
      }
   
   
   if(debug_)   std::cout<<"Here I am : got met infor right "<<std::endl;
   if(debug_) std::cout<<"beforeend"<<std::endl;
 
   mytree->Fill();
   if(debug_) std::cout<<"end"<<std::endl;
}


// ------------ method to improve ME0 muon ID ----------------
bool 
Validator::isME0MuonSelNew(reco::Muon muon, double dEtaCut, double dPhiCut, double dPhiBendCut, edm::EventSetup const& iSetup)
{

  bool result = false;
  bool isME0 = muon.isME0Muon();

  if(isME0){

    double deltaEta = 999;
    double deltaPhi = 999;
    double deltaPhiBend = 999;


    if(!ME0Geometry_)
      //throw std::runtime_error("Validator::isME0MuonSelNew: muon geometry not loaded");
      {
	edm::ESHandle<ME0Geometry> hGeom;
	iSetup.get<MuonGeometryRecord>().get(hGeom);
	ME0Geometry_ =( &*hGeom);
	if(!ME0Geometry_) return result;
      }

    const std::vector<reco::MuonChamberMatch>& chambers = muon.matches();
    for( std::vector<reco::MuonChamberMatch>::const_iterator chamber = chambers.begin(); chamber != chambers.end(); ++chamber ){

      if (chamber->detector() == 5){

        for ( std::vector<reco::MuonSegmentMatch>::const_iterator segment = chamber->me0Matches.begin(); segment != chamber->me0Matches.end(); ++segment ){

          LocalPoint trk_loc_coord(chamber->x, chamber->y, 0);
          LocalPoint seg_loc_coord(segment->x, segment->y, 0);
          LocalVector trk_loc_vec(chamber->dXdZ, chamber->dYdZ, 1);
          LocalVector seg_loc_vec(segment->dXdZ, segment->dYdZ, 1);

          const ME0Chamber * me0chamber = ME0Geometry_->chamber(chamber->id);
	  if(!me0chamber)continue;

          GlobalPoint trk_glb_coord = me0chamber->toGlobal(trk_loc_coord);
          GlobalPoint seg_glb_coord = me0chamber->toGlobal(seg_loc_coord);

          //double segDPhi = segment->me0SegmentRef->deltaPhi();
          // need to check if this works
          double segDPhi = me0chamber->computeDeltaPhi(seg_loc_coord, seg_loc_vec);
          double trackDPhi = me0chamber->computeDeltaPhi(trk_loc_coord, trk_loc_vec);

          deltaEta = std::abs(trk_glb_coord.eta() - seg_glb_coord.eta() );
          deltaPhi = std::abs(trk_glb_coord.phi() - seg_glb_coord.phi() );
          deltaPhiBend = std::abs(segDPhi - trackDPhi);

          if (deltaEta < dEtaCut && deltaPhi < dPhiCut && deltaPhiBend < dPhiBendCut) result = true;

        }
      }
    }

  }

  return result;

}


////Jan's code//
float Validator::calculate_demetraIsolation(const pat::Tau& tau)const{
  /*  unsigned int tau_vertex_idxpf=-1;
  pat::PackedCandidate const* packedLeadTauCand = dynamic_cast<pat::PackedCandidate const*>(tau.leadChargedHadrCand().get());
  tau_vertex_idxpf = packedLeadTauCand->vertexRef().key();
  
  float isoDR03pt08dz015=0;
  float gamma_DR03sum=0;
  
  
  for(const auto& IsoCand: tau.isolationChargedHadrCands()){
    pat::PackedCandidate const* cand = dynamic_cast<pat::PackedCandidate const*>(IsoCand.get());
    if (! cand->charge() )continue;
    //WATCH OUT WHICH VERTICES THESE ARE
    /*if(!vertices())continue;
    const auto& tau_vertex = (*vertices())[tau_vertex_idxpf];
    
    if ((cand->pt()<=0.8) || (fabs(cand->dxy(tau_vertex.position()))>=0.05))continue;
    if (cand->hasTrackDetails()){
      const auto &tt = cand->pseudoTrack();
      if (tt.normalizedChi2()>=100. || cand->numberOfHits()<3)continue;
    }
    
    if (reco::deltaR2(tau,*cand)<0.3*0.3
	&& fabs(cand->dz(tau_vertex.position()))<0.15){
	   
	   isoDR03pt08dz015+=cand->pt();
    }
  }
  for(const auto&  IsoCand: tau.isolationGammaCands()){
    pat::PackedCandidate const* cand = dynamic_cast<pat::PackedCandidate const*>(IsoCand.get());
    if ( cand->pt() < 0.5 ) continue;
    if (reco::deltaR2(tau,*cand)<0.3*0.3 && cand->pt()>1.){
      gamma_DR03sum+=cand->pt();
      }
  }
  */
  // if ( std::abs(cand->eta()) < 1.4) return (isoDR03pt08dz015 + 0.2 * std::max(0., gamma_DR03sum - 5.));
  // else return (isoDR03pt08dz015 + 0.2 * std::max(0., gamma_DR03sum - 1.));
  return 1.;
    
  
}





// ------------ method called once each job just before starting event loop  ------------
void 
Validator::beginJob()
{

}


void
Validator::beginRun(edm::Run const& iRun, edm::EventSetup const& iSetup)
{
  edm::ESHandle<ME0Geometry> hGeom;
  iSetup.get<MuonGeometryRecord>().get(hGeom);
  ME0Geometry_ =( &*hGeom);
  /*  edm::Handle<LHERunInfoProduct> run; 
  typedef std::vector<LHERunInfoProduct::Header>::const_iterator headers_const_iterator;
 
  iRun.getByToken( "externalLHEProducer", abc );
  LHERunInfoProduct myLHERunInfoProduct = *(abc.product());
 
  for (headers_const_iterator iter=myLHERunInfoProduct.headers_begin(); iter!=myLHERunInfoProduct.headers_end(); iter++){
    std::cout << iter->tag() << std::endl;
    std::vector<std::string> lines = iter->lines();
    for (unsigned int iLine = 0; iLine<lines.size(); iLine++) {
      std::cout << lines.at(iLine);
    }
    }*/
}

// ------------ method called when ending the processing of a run  ------------
  void
  Validator::endRun(edm::Run const&, edm::EventSetup const&)
  {
  }


// ------------ method called once each job just after ending the event loop  ------------
void 
Validator::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
Validator::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Validator);

