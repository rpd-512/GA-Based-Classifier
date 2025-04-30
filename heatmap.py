import matplotlib.pyplot as plt
from tqdm import tqdm
import json
import seaborn as sns
from sklearn.metrics import confusion_matrix
import sys


weights = [float(x) for x in sys.argv[1:-2]]

filename = sys.argv[-2]
data_file = sys.argv[-1]

print(weights)

def evaluate_authentication(wgt, data_path=data_file):
    weights = {
        "Geolocation": 0,
        "Device Posture": 0,
        "Device Binding": 0,
        "User Behavior Patterns": 0,
        "Login History": 0,
        "Network Security": 0,
        "Real-Time Threat Data": 0,
        "User Behavior Analytics": 0
    }
    for i,w in enumerate(weights):
        weights[w] = wgt[i]

    # Load authentication data
    with open(data_path, 'r') as auth_file:
        auth_data = json.load(auth_file)

    # Define thresholds
    T_L = 3  # Low-risk threshold
    T_H = 7  # High-risk threshold

    def malauth(input_data, weight):
        """Determines if a user attempt is fraudulent based on weighted risk scoring."""
        risk_score = sum(input_data[key] * weight[key] / 100 for key in weight)
        if risk_score < T_L:
            return 0  # Legitimate
        elif risk_score > T_H:
            return 2  # Fraudulent
        else:
            return 1  # Suspicious

    # Store true labels and predicted labels
    y_true = [data['Label'] for data in auth_data]
    y_pred = [malauth(data, weights) for data in auth_data]

    # Compute confusion matrix
    conf_matrix = confusion_matrix(y_true, y_pred, labels=[0, 1, 2])

    # Plot the confusion matrix as a heatmap
    plt.figure(figsize=(6, 5))
    sns.heatmap(conf_matrix, annot=True, fmt='d', cmap='cividis',
                xticklabels=["Legit", "Suspicious", "Fraud"],
                yticklabels=["Legit", "Suspicious", "Fraud"])
    plt.xlabel("Predicted Label")
    plt.ylabel("Actual Label")
    plt.title("Confusion Matrix Heatmap")
    plt.savefig(filename+".png")
    #plt.show()

evaluate_authentication(weights)