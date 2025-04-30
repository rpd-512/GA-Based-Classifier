import statistics
import json
import sys

var = 6
exp_ser = sys.argv[1]

path = "/home/rapidfire69/rapid/coding/researchWorks/raba-ga/final_data_production/json_data/"
filename = path + f"VAR_{var}_EXP_{exp_ser}.json"

with open(filename, 'r') as file:
    stat_data = json.loads(file.read())

train_total = 8000
valid_total = 2000

sim_fit = []
trn_fit = 0

exp_fit = []
vld_fit = 0

wgts = []

for s in stat_data:
    stat = stat_data[s]
    sim_fit.append(stat['Final_fit'])
    trn_fit = stat['Static_fit']
    exp_fit.append(stat['Valid_fit'])
    vld_fit = stat['Static_valid']
    wgts.append(sorted(stat["Weights"]))

sim_fit_mean = statistics.mean(sim_fit)
vld_fit_mean = statistics.mean(exp_fit)

sim_fit_stdev = statistics.stdev(sim_fit)
vld_fit_stdev = statistics.stdev(exp_fit)

sim_acc = sim_fit_mean / train_total
vld_acc = vld_fit_mean / valid_total

static_sim_acc = trn_fit / train_total
static_vld_acc = vld_fit / valid_total

# Compute the average of each index across all weight lists
avg_wgts = [round(statistics.mean(w[i] for w in wgts),3) for i in range(len(wgts[0]))]
sorted_avg_wgts = sorted(avg_wgts)

print("\n===== Experiment Results =====")
print(f"Variance          : {var}")
print(f"Experiment Series : {exp_ser}")

print("\n===== Fitness Scores =====")
print(f"Simulation Fitness Mean   : {sim_fit_mean:.4f} ± {sim_fit_stdev:.4f}")
print(f"Validation Fitness Mean   : {vld_fit_mean:.4f} ± {vld_fit_stdev:.4f}")

print("\n===== Accuracy =====")
print(f"Simulated Accuracy       : {sim_acc:.4%}")
print(f"Validation Accuracy      : {vld_acc:.4%}")
print(f"Static Simulated Accuracy: {static_sim_acc:.4%}")
print(f"Static Validation Accuracy: {static_vld_acc:.4%}")

print("\n===== Average Weights (Sorted) =====")
print(sorted_avg_wgts)
