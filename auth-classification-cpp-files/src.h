#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <sstream>
#include <string>

#define THRESHOLD_L 30
#define THRESHOLD_H 70

using namespace std;


typedef struct chromoInfo{
    float fitness;
    vector <float> gene;
}chromoInfo;


typedef struct plotPoint{
    vector<double> iteration;
    vector<double> fitness;
    vector<float> chromoData;
}plotPoint;


vector<double> float_to_double(vector<float> fltVec){
    vector<double> dlbVec;
    for(float f:fltVec){
        dlbVec.push_back((double)f);
    }
    return dlbVec;
}

// Function to handle signals
void signalHandler(int signum) {
    cout << endl;
    indicators::show_console_cursor(true);
    exit(signum);
}

random_device rd;  // Seed the random number engine
mt19937 gen(rd()); // Standard mersenne_twister_engine

int randint(int low, int high) {
    uniform_int_distribution<> dist(low, high);
    return dist(gen);
}

float uniform(double low, double high) {
    uniform_real_distribution<> dist(low, high);
    return dist(gen);
}

void print_vector(vector<float> vec){
    for(auto v: vec){
        cout << v << "\t";
    }
    cout << endl;
}

void printChromoInfo(vector<chromoInfo> popData){
    cout << endl;
    for (const auto& chromo : popData) {
        std::cout << "Fitness: " << chromo.fitness << ", Genes: ";
        for (const auto& gene : chromo.gene) {
            std::cout << gene << "\t";
        }
        std::cout << std::endl;
    }
    cout << endl;
}


vector<float> repair_weights(vector<float> weights) {
    float total = accumulate(weights.begin(), weights.end(), 0.0f);

    if (total == 0.0f) {
        throw invalid_argument("Sum of weights cannot be zero.");
    }

    // Scale weights so they sum to 100 and round to 2 decimal places
    vector<float> repaired_weights(weights.size());
    transform(weights.begin(), weights.end(), repaired_weights.begin(),
              [total](float w) { return roundf((w / total) * 10000.0f) / 100.0f; });

    // Fix rounding errors by adjusting the largest value
    float sum_repaired = accumulate(repaired_weights.begin(), repaired_weights.end(), 0.0f);
    float diff = 100.0f - sum_repaired;

    if (diff < 0.0f) {
        auto max_it = max_element(repaired_weights.begin(), repaired_weights.end());
        *max_it += diff;  // Adjust the largest weight to ensure the sum is exactly 100
    }
    if (diff > 0.0f) {
        auto min_it = min_element(repaired_weights.begin(), repaired_weights.end());
        *min_it += diff;  // Adjust the largest weight to ensure the sum is exactly 100
    }

    return repaired_weights;
}

vector<vector <float>> generateChromosome(int pop, int size){
    vector<vector <float>> rpop;
    vector<float> chrm;
    for(int i=0;i<pop;i++){
        chrm = {};
        for(int j=0;j<size;j++){
            chrm.push_back(uniform(0,30));
        }
        rpop.push_back(repair_weights(chrm));
    }
    return rpop;
}

vector<vector<float>> auth_read(string filename) {
    vector<vector<float>> auth_data;
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return auth_data;
    }

    // Define expected categories in the correct order
    vector<string> categories = {
        "Geolocation", "Device Posture", "Device Binding", "User Behavior Patterns",
        "Login History", "Network Security", "Real-Time Threat Data", "User Behavior Analytics", "Label"
    };

    string line;
    vector<float> entry(categories.size(), 0.0);

    while (getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.find("{") != string::npos) {
            entry.assign(categories.size(), 0.0); // Reset entry
        } 
        else if (line.find("}") != string::npos) {
            auth_data.push_back(entry); // Store row-wise entry
        } 
        else {
            size_t colon_pos = line.find(":");
            if (colon_pos != string::npos) {
                string key = line.substr(0, colon_pos);
                string value_str = line.substr(colon_pos + 1);

                key.erase(0, key.find_first_not_of(" \t\""));
                key.erase(key.find_last_not_of(" \t\"") + 1);

                value_str.erase(0, value_str.find_first_not_of(" \t"));
                value_str.erase(value_str.find_last_not_of(", \t") + 1);
                
                auto it = find(categories.begin(), categories.end(), key);
                if (it != categories.end()) {
                    size_t index = distance(categories.begin(), it);
                    try {
                        entry[index] = stof(value_str);
                    } catch (const exception &e) {
                        cerr << "Warning: Failed to convert '" << value_str << "' to float." << endl;
                    }
                }
            }
        }
    }

    file.close();
    return auth_data;
}


int malauth(const vector<float>& user_score, const vector<float>& weights) {
    constexpr float T_L = 3.0f;  // Low-risk threshold
    constexpr float T_H = 7.0f;  // High-risk threshold
    
    float risk_score = 0.0f;
    
    // Compute weighted risk score
    for (int i=0 ; i < weights.size(); i++) {
        risk_score += user_score.at(i) * (weights.at(i) / 100.0f);
    }
    
    // Determine risk category
    if (risk_score < T_L) return 0;  // Legitimate
    if (risk_score > T_H) return 2;  // Fraudulent
    return 1;  // Suspicious
}

int fitness(const vector<float>& weights, const vector<vector<float>>& auth_data) {
    int cnt = 0;
    
    // Assign values from wgt to corresponding weight categories
    
    #pragma omp parallel for reduction(+:cnt)
    for (size_t i = 0; i < auth_data.size(); i++) {
        auto actual_label = auth_data[i].at(8);

        int predicted_label = malauth(auth_data[i], weights);

        if (actual_label == predicted_label) {
            cnt++;
        }
    }
    
    return cnt; // Return total correct predictions
}

void gen_heatmap(const vector<float>& chromoData, string filename) {
    // Convert vector to space-separated string
    ostringstream oss;
    for (const auto& val : chromoData) {
        oss << val << " ";
    }
    
    string command = "python3 heatmap.py " + oss.str() + " "+filename+" &";
    
    // Run the command
    system(command.c_str());
}