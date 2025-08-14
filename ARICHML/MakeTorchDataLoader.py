import numpy as np
import uproot
from argparse import ArgumentParser
from torch.utils.data import Dataset, DataLoader, ConcatDataset
import glob
import torch
import time

parser = ArgumentParser()
parser.add_argument('-path', '-p', help = "Path to input files *.dataML.root")
parser.add_argument('-Momenta', '-M', help = "Momenta of input file")

class TorchDataset(Dataset):
    def __init__(self, images, aux_data, labels):
        self.images = torch.tensor(images, dtype=torch.float32).permute(0, 3, 1, 2)      # Shape: [N, 1, 24, 24]
        self.aux_data = torch.tensor(aux_data, dtype=torch.float32) # Shape: [N, 1]
        self.labels = torch.tensor(labels, dtype=torch.float32)        # Shape: [N]

    def __len__(self):
        return len(self.labels)

    def __getitem__(self, idx):
        return self.images[idx], self.aux_data[idx], self.labels[idx]
                
def GetImage(histo):
    pos = histo.to_numpy()[1]
    pic = np.zeros(shape= (26,26))
    for i in range(0,len(pos)-1):
        for j in np.where(histo.to_numpy()[0][i] != 0):
            pic[25-j,i] = histo.to_numpy()[0][i,j]

    return np.expand_dims(pic, axis=-1)
    
   
if __name__ == "__main__":

   args = parser.parse_args()
   c = 0
   for file_path in glob.glob(f"{args.path}/*M{args.Momenta}_*dataML.root"): 
                
      start = time.time()
      file = uproot.open(file_path)
      print(f"Loading from {file_path}", end ="") 
       #note: the last element is empty, idk why, so I'm just skipping it :)
      temp_momenta = np.array(file['getdata/ARICHRECO']["Momenta"].array()[:-1])	
      temp_pdg = np.array(file['getdata/ARICHRECO']["pdg"].array()[:-1])
      temp_histos = file['getdata/ARICHRECO']["histo"].array(library="np")[:-1]
        
      print(f", found {len(temp_pdg)} events", end= "")
        
      histo = np.array(list(map(GetImage,temp_histos)))
       # print(temp_momenta.shape)
        	
      print(f", time taken {start -time.time():.2f} sec")
       
      momenta_array = np.round(temp_momenta[:,2]/1000,3)

      set_ = TorchDataset(np.array(histo),np.array(momenta_array), np.array(temp_pdg))
      del temp_momenta;del temp_pdg;del temp_histos;del histo;del momenta_array
      file.close()       
      if(c==0):
         concat_set = ConcatDataset([set_])
      else:
         concat_set = ConcatDataset([concat_set,set_])
      del set_;
      c+=1

   data_loader = DataLoader(concat_set,128,shuffle=True)
   print(f"Found {len(data_loader.dataset)} events in total")
   torch.save(data_loader,f"data_{args.Momenta}GeV.pt") 
   del concat_set; del data_loader;
