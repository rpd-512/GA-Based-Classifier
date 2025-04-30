import json
import matplotlib.pyplot as plt

# Load JSON data from file

def boxplotter(var,ser):
    with open("json_data/VAR_"+str(var)+"_EXP_"+ser+".json", "r") as file:
        data = json.load(file)
    # Extract accuracies
    valid_accuracies = [entry["Valid_fit"] / 2000 for entry in data.values()]
    final_accuracies = [entry["Final_fit"] / 8000 for entry in data.values()]

    # Static weight accuracy
    static_valid_acc = list(data.values())[0]["Static_valid"] / 2000
    static_train_acc = list(data.values())[0]["Static_fit"] / 8000

    # Create box plot
    plt.figure(figsize=(8, 6))
    plt.boxplot([valid_accuracies, final_accuracies], labels=["Validation Accuracy", "Training Accuracy"])

    # Add horizontal lines for static weight accuracy
    plt.axhline(y=static_valid_acc, color="r", linestyle="--", label="Static Weight Validation Accuracy")
    plt.axhline(y=static_train_acc, color="b", linestyle="--", label="Static Weight Training Accuracy")

    # Formatting
    plt.ylabel("Accuracy")
    plt.title("Box Plot of Accuracies")
    plt.legend()
    plt.grid(True, linestyle="--", alpha=0.6)

    # Show plot
    plt.savefig("VAR_"+str(var)+"_EXP_"+ser+"_boxplot")

boxplotter(4,"A")
boxplotter(4,"B")
boxplotter(4,"C")
boxplotter(5,"A")
boxplotter(5,"B")
boxplotter(5,"C")
boxplotter(6,"A")
boxplotter(6,"B")
boxplotter(6,"C")
