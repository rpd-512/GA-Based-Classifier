import os
import json

ser = "C"

dir_list = os.listdir("graphics/line_graph/SERIES_"+ser)
j_data = {}


with open("json_data/VAR_4_EXP_"+ser+".json",'r') as file:
    j_data = j_data | json.loads(file.read())

with open("json_data/VAR_5_EXP_"+ser+".json",'r') as file:
    j_data = j_data | json.loads(file.read())

with open("json_data/VAR_6_EXP_"+ser+".json",'r') as file:
    j_data = j_data | json.loads(file.read())

for d in dir_list:
    id_exp = d[:13]
    exp_info = j_data[id_exp]
    exp_wgts = exp_info["Weights"]
    wgts = " ".join(map(str, exp_wgts))
    filename = id_exp+"_VAR_"+str(int(exp_info["Variance"]))+"_EXP_"+ser+"_confusion_matrix"
    datafile = "datasets/RABA_Var_"+str(int(exp_info["Variance"]))+"/validating_data.json"
    os.system("python3 heatmap.py "+wgts+" "+filename+" "+datafile)
