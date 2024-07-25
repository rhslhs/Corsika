import re
import os

head = "#!/bin/bash\n\nmake clean\n\nmake muonplotter\n\n"
with open("GetAllMuon.sh", 'w') as file:
    file.write(head)

current_script_dir = os.path.dirname(os.path.abspath(__file__))
target_directory = os.path.join(current_script_dir, 'DATfiles')
DATfile_names = os.listdir(target_directory)
# DAT_names = [os.path.splitext(file_name)[0] for file_name in DATfile_names]
DAT_names = [dataname for dataname in DATfile_names if dataname.startswith('DAT0') and not dataname.endswith('.long')]

pDataFiles = []
HeDataFiles = []

for DAT_name in DAT_names:
    with open("GetAllMuon.sh", 'a') as file:
        file.write("./CorsikaMuonPlotter ./DATfiles/" + DAT_name + "\n\n")


with open("GetAllMuon.sh", 'a') as file:
    file.write("hadd DAT_Muon_p.root ./rootfiles/pDAT*\n\nhadd DAT_Muon_He.root ./rootfiles/HeDAT*")