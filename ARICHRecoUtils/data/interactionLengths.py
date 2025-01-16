import numpy as np
import csv

files = ["leps1-1b.dx","btr4-1a.dx","hds2-3b.dx","leps2-1a.dx","leps6-1a.dx"]
ns = [1.0505,1.0452,1.0401,1.0352,1.0297]
nerrs = [0.0006, 0.0008,0.0008,0.0008,0.0006]
thicknesses = [2.04, 1.99, 2.00, 1.93,2.00]

for i, file in enumerate(files):
    thickness = thicknesses[i]
    n = ns[i]
    wavs = [] # Wavelengths
    trans = [] # Transmission percentages
    with open(file) as f:
        for line in f:
            wavs.append(float(line.split()[0]))
            trans.append(float(line.split()[1]))
    trans = np.array(trans)
    # Convert to proportion
    trans = trans / 100. 
    # Prevent any funny log business
    trans[trans <= 0] = 0.0001
    # Calculate interactions length, given trans = exp(-thickness/intLength)
    intlengths = -thickness/np.log(trans)
    output = np.column_stack((wavs, intlengths))  
    newFile = file.split('.')[0] + "IntLength.csv"  
    np.savetxt(newFile, output)

