#!/usr/bin/env python3
import sys

def log_analyzer(run):
    filename = str(run) + "stats.log"
    #print(filename)
    f = open(filename,"r") #modify path to the file because now it is assumed the .log files are in the current working directory 
    runs_info = {}
    last_seen_run = -1
    last_seen_spill = -1
    all_lines = f.readlines()
    for line in all_lines:
        line = line.strip()
        if line.startswith("Event Header from DAQEventBuilder"):
            tokens = line.split()
            run = int(tokens[5].replace(",",""))
            spill = int(tokens[7].replace(",",""))
            runs_info[run] = {}
            runs_info[run][spill]={}
            last_seen_run = run
            last_seen_spill = spill

        elif line.startswith("ID"):
            tokens = line.split()
            iD = int(tokens[1])
            counts = int(tokens[4])
            if 0<=iD<=5:
       
                if "caen" not in runs_info[last_seen_run][last_seen_spill]:
                    runs_info[last_seen_run][last_seen_spill]["caen"]={}
                    runs_info[last_seen_run][last_seen_spill]["caen"][iD]=counts
                else:
                    runs_info[last_seen_run][last_seen_spill]["caen"][iD] = counts
            elif 101<=iD<=104:
                if "trb3" not in runs_info[last_seen_run][last_seen_spill]:
                    runs_info[last_seen_run][last_seen_spill]["trb3"]={}
                    runs_info[last_seen_run][last_seen_spill]["trb3"][iD] = counts
                else:
                    runs_info[last_seen_run][last_seen_spill]["trb3"][iD] = counts
  
    for run in runs_info:
        for spill in runs_info[run]:
            nCAEN = len(runs_info[run][spill]["caen"]) 
            nTRB3 = len(runs_info[run][spill]["trb3"])
 
            if nCAEN == 6 and nTRB3 == 4:
                caen_keys = list(runs_info[run][spill]["caen"].keys())
                prev_counts_caen = runs_info[run][spill]["caen"][caen_keys[0]]
                for iD in caen_keys:
                    if runs_info[run][spill]["caen"][iD]!= prev_counts_caen:
                        runs_info[run][spill]["status"] = "BAD"
                        break 
                trb3_keys =  list(runs_info[run][spill]["trb3"].keys())
                prev_counts_trb3 = runs_info[run][spill]["trb3"][trb3_keys[0]]
                
                for iD in trb3_keys:
                    
                    if runs_info[run][spill]["trb3"][iD] != prev_counts_trb3:
                        runs_info[run][spill]["status"] = "BAD"
                        break 

                if "status" not in runs_info[run][spill]:
                    runs_info[run][spill]["status"] = "GOOD"
            else:
                runs_info[run][spill]["status"] = "BAD"

    #print(runs_info)
   
    return runs_info 


if __name__ == '__main__':
    args = sys.argv[1:]
    runs_info = log_analyzer(args[0])       
    ids = [0,1,2,3,4,5,101,102,103,104]
    total_counts = {}
    for run in runs_info:
        total_counts[run] = {}

        for iD in ids:
            total_counts[run][iD] = 0

        for spill in runs_info[run]:
            print("SUMMARY: Run ", run, "spill ", spill,": status = ",runs_info[run][spill]["status"])
            for iD in ids:
                if 0<=iD<=5:
                    total_counts[run][iD] += runs_info[run][spill]["caen"][iD]
                elif 101<=iD<=104:
                    total_counts[run][iD] +=runs_info[run][spill]["trb3"][iD]

        for iD in ids:
            print("ID: ",iD, "   Total counts: ", total_counts[run][iD], "Average couts per spill: ", total_counts[run][iD]/len(runs_info[run]), "over ", len(runs_info[run]), "spills")



