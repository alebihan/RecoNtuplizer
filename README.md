# PhaseIIAnalysis
# Repository for collecting recipes for standard physics objects and for analysis of simulated events with the CMS phase 2 detector

The recipes for accessing PhaseII objects are summarized here:

[UPG PhaseII recipes](https://twiki.cern.ch/twiki/bin/view/CMS/PhaseIIFSObjectRecipes "UPG PhaseII recipes")

To run the Ntuplizer run the following on lxplus machines:

####################################################################################
```
cmsrel CMSSW_10_6_0_patch2
cd CMSSW_10_6_0_patch2/src/
cmsenv
git cms-init
git cms-addpkg RecoBTag/TensorFlow
git cms-addpkg RecoBTag/Combined
wget https://raw.githubusercontent.com/cms-data/RecoBTag-Combined/master/DeepCSV_PhaseII.json -P RecoBTag/Combined/data/
git cms-merge-topic rauser:DeepJetPhaseII_10_4_X
mkdir new; cd new
git clone https://github.com/recotoolsbenchmarks/RecoNtuplizer.git .
cd ../; mv new/* .
rm -rf new
scram b -j 8
cd TreeMaker/Ntuplzr/
```
####################################################################################
Basic Setup : Here in plugins/Ntuplzr.cc is the EDAnalyzer that makes the ntuples, takes in information 
from python/Ntuplzr_cfi.py. These default parameters can be modified in :
test/myproduceNtuples_cfg.py

to run this file, do :  
```
cmsRun test/myproduceNtuples_cfg.py maxEvents=10 outputFile=file.root
```

###################################################################################
STEP 1 : to get the main ntuples from this Ntuplzer using test/myproduceNtuples_cfg.py, a crab setup is there. one can update 
        various parameters in test/Step1_crab/submitCrabJobs_cfgparams.py according to their choice. 
	To crab-submit, do : 

```
cd test/Step1_crab
python submitCrabJobs_cfgparams.py 
```
###################################################################################
To check on the status or resubmit the jobs, change the file Resubmit.csh accordingly and do :  
```
source Resubmit.csh
```

###################################################################################
STEP 2 : Once the main ntuples are there, C++ classes along with a proof wrapper is written to be able to run the jobs 
parallely on the system. 

Here we have multiple classes based on what needs to be done. 
SelectorClass_SIG.C/h : Mainly to plot the efficiency on a given signal sample 
SelectorClass_BKG.C/h : Mainly to plot the fake-rate  on a given bkground sample 


To get this going on the root files stored in: 
/eos/cms/store/group/upgrade/RTB/

Step1_runcreateList.sh creates the input list of files for a given sample through a script createList.sh.
And then Step2_runScript_runAll_eff.sh runs over the given class through the wrapper code of runAll.C.
To Run : 

```
cd test/Step2_PostAN
./Step1_runcreateList.sh
./Step2_runScript_runAll_eff.sh
```

This gives the root files with all the information as askd from the class that was run. 

###################################################################################

STEP 3: This is where one makes the final histograms and puts them on web page or wherever u want them to put using the proper plotter. 
for the efficiency or fake rate plots, do : 

```
cd test/Step3_MakePlots
./Step1_getEffPlots.sh
``` 	   
###################################################################################

