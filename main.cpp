#include "auth-classification-cpp-files/main.cpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

typedef struct Experiment {
    int population;
    int iteration;
    float variance;
    int final_fitness;
    int valid_fitness;
    int static_score;
    int static_valid;
    vector<float> weights;
    vector<double> iteration_list;
    vector<double> fitness_growth;
    string exp_alpha;
} Experiment;


void save_json(const string& filename, const string& exp_id, const Experiment& exp) {
    json existingData;
    
    // Read existing JSON data from file
    ifstream file(filename);
    if (file) {
        try {
            file >> existingData;
        } catch (json::parse_error& e) {
            cerr << "Error parsing JSON file: " << e.what() << endl;
        }
    }
    file.close();
    
    // Create JSON object for the new experiment
    json newExperiment;
    newExperiment["Population"] = exp.population;
    newExperiment["Iteration"] = exp.iteration;
    newExperiment["Variance"] = exp.variance;
    newExperiment["Final_fit"] = exp.final_fitness;
    newExperiment["Valid_fit"] = exp.valid_fitness;
    newExperiment["Weights"] = exp.weights;
    newExperiment["Static_fit"] = exp.static_score;
    newExperiment["Static_valid"] = exp.static_valid;
    // Append new experiment under the unique ID
    string id_key = "ID-" + exp_id;
    existingData[id_key] = newExperiment;
    
    // Write back to file
    ofstream outFile(filename);
    outFile << existingData.dump(4); // Pretty print with 4-space indentation
    outFile.close();
    
    cout << "Experiment " << id_key << " added successfully!" << endl;
}

string generateID(int size) {
    std::string randomText;
    const std::string characters = "abcdefghijklmnopqrstuvwxyz0123456789";
    
    // Seed the random number generator with the current time
    srand(static_cast<unsigned int>(std::time(0)));

    // Generate random text
    for (int i = 0; i < size; ++i) {
        // Choose a random character from the 'characters' string
        randomText += characters[std::rand() % characters.size()];
    }
    return randomText;
}


void plot_and_save_experiment(Experiment exp){
    string exp_id = generateID(10);
    vector<double> x_line = {0, exp.iteration};  // x-values for the line
    vector<double> static_line = {exp.static_score,exp.static_score};

    save_json("json_data/VAR_"+to_string((int)exp.variance)+"_EXP_"+exp.exp_alpha+".json",exp_id, exp);
    gen_heatmap(exp.weights, "graphics/confusion_matrix/SERIES_"+exp.exp_alpha+"/ID-"+exp_id+"_VAR_"+to_string((int)exp.variance)+"_EXP_"+exp.exp_alpha+"_confusion_matrix");
    plt::figure_size(1000,800);
    plt::plot(exp.iteration_list, exp.fitness_growth, {{"label", "Genetic Algorithms"},{"color","red"}});
    plt::plot(x_line, static_line, {{"label", "Score of static weights"},{"color","blue"}});
    plt::xlabel("Iterations");
    plt::ylabel("Fitness");

    plt::legend();
    plt::save("graphics/line_graph/SERIES_"+exp.exp_alpha+"/ID-"+exp_id+"_VAR_"+to_string((int)exp.variance)+"_EXP_"+exp.exp_alpha+"_linegraph");
}

int main(){

    //pre data
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    srand(time(0));
    int dim = 8;
    vector<double> v(dim); iota(v.begin(), v.end(), 1);
    vector<float> static_weights = {15,15,20,10,10,10,10,10};
    vector<vector<float>> train_data_variance_4 = auth_read("datasets/Auth_Var_4/training_data.json");
    vector<vector<float>> valid_data_variance_4 = auth_read("datasets/Auth_Var_4/validating_data.json");

    vector<vector<float>> train_data_variance_5 = auth_read("datasets/Auth_Var_5/training_data.json");
    vector<vector<float>> valid_data_variance_5 = auth_read("datasets/Auth_Var_5/validating_data.json");

    vector<vector<float>> train_data_variance_6 = auth_read("datasets/Auth_Var_6/training_data.json");
    vector<vector<float>> valid_data_variance_6 = auth_read("datasets/Auth_Var_6/validating_data.json");
    int count = 25;

    int pop = 20;
    int itr = 50;
    string series = "A";
    
    float var;
    float static_score;

    //EXPERIMENT Series V-4
    var = 4;
    static_score = fitness(static_weights ,train_data_variance_4);

    for(int i=0;i<count;i++){
        vector<vector <float>> randGen = generateChromosome(pop,dim);
        plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data_variance_4);

        Experiment exp;
        exp.population = pop;
        exp.iteration = itr;
        exp.variance = var;
        exp.final_fitness = fitness(geneticAlgorithmResults.chromoData ,train_data_variance_4);
        exp.valid_fitness = fitness(geneticAlgorithmResults.chromoData ,valid_data_variance_4);
        exp.static_score = static_score;
        exp.static_valid = fitness(static_weights, valid_data_variance_4);
        exp.weights = geneticAlgorithmResults.chromoData;
        exp.iteration_list = geneticAlgorithmResults.iteration;
        exp.fitness_growth = geneticAlgorithmResults.fitness;
        exp.exp_alpha = series;
        plot_and_save_experiment(exp);
    }
    //-------------------//

    //EXPERIMENT Series V-5
    var = 5;
    static_score = fitness(static_weights ,train_data_variance_5);

    for(int i=0;i<count;i++){
        vector<vector <float>> randGen = generateChromosome(pop,dim);
        plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data_variance_5);

        Experiment exp;
        exp.population = pop;
        exp.iteration = itr;
        exp.variance = var;
        exp.final_fitness = fitness(geneticAlgorithmResults.chromoData ,train_data_variance_5);
        exp.valid_fitness = fitness(geneticAlgorithmResults.chromoData ,valid_data_variance_5);
        exp.static_score = static_score;
        exp.static_valid = fitness(static_weights, valid_data_variance_5);
        exp.weights = geneticAlgorithmResults.chromoData;
        exp.iteration_list = geneticAlgorithmResults.iteration;
        exp.fitness_growth = geneticAlgorithmResults.fitness;
        exp.exp_alpha = series;
        plot_and_save_experiment(exp);
    }
    //-------------------//

    //EXPERIMENT Series V-6
    var = 6;
    static_score = fitness(static_weights ,train_data_variance_6);

    for(int i=0;i<count;i++){
        vector<vector <float>> randGen = generateChromosome(pop,dim);
        plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data_variance_6);

        Experiment exp;
        exp.population = pop;
        exp.iteration = itr;
        exp.variance = var;
        exp.final_fitness = fitness(geneticAlgorithmResults.chromoData ,train_data_variance_6);
        exp.valid_fitness = fitness(geneticAlgorithmResults.chromoData ,valid_data_variance_6);
        exp.static_score = static_score;
        exp.static_valid = fitness(static_weights, valid_data_variance_6);
        exp.weights = geneticAlgorithmResults.chromoData;
        exp.iteration_list = geneticAlgorithmResults.iteration;
        exp.fitness_growth = geneticAlgorithmResults.fitness;
        exp.exp_alpha = series;
        plot_and_save_experiment(exp);
    }
    //-------------------//



    pop = 50;
    itr = 100;
    series = "B";
    
    //EXPERIMENT Series V-4
    var = 4;
    static_score = fitness(static_weights ,train_data_variance_4);

    for(int i=0;i<count;i++){
        vector<vector <float>> randGen = generateChromosome(pop,dim);
        plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data_variance_4);

        Experiment exp;
        exp.population = pop;
        exp.iteration = itr;
        exp.variance = var;
        exp.final_fitness = fitness(geneticAlgorithmResults.chromoData ,train_data_variance_4);
        exp.valid_fitness = fitness(geneticAlgorithmResults.chromoData ,valid_data_variance_4);
        exp.static_score = static_score;
        exp.static_valid = fitness(static_weights, valid_data_variance_4);
        exp.weights = geneticAlgorithmResults.chromoData;
        exp.iteration_list = geneticAlgorithmResults.iteration;
        exp.fitness_growth = geneticAlgorithmResults.fitness;
        exp.exp_alpha = series;
        plot_and_save_experiment(exp);
    }
    //-------------------//

    //EXPERIMENT Series V-5
    var = 5;
    static_score = fitness(static_weights ,train_data_variance_5);

    for(int i=0;i<count;i++){
        vector<vector <float>> randGen = generateChromosome(pop,dim);
        plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data_variance_5);

        Experiment exp;
        exp.population = pop;
        exp.iteration = itr;
        exp.variance = var;
        exp.final_fitness = fitness(geneticAlgorithmResults.chromoData ,train_data_variance_5);
        exp.valid_fitness = fitness(geneticAlgorithmResults.chromoData ,valid_data_variance_5);
        exp.static_score = static_score;
        exp.static_valid = fitness(static_weights, valid_data_variance_5);
        exp.weights = geneticAlgorithmResults.chromoData;
        exp.iteration_list = geneticAlgorithmResults.iteration;
        exp.fitness_growth = geneticAlgorithmResults.fitness;
        exp.exp_alpha = series;
        plot_and_save_experiment(exp);
    }
    //-------------------//

    //EXPERIMENT Series V-6
    var = 6;
    static_score = fitness(static_weights ,train_data_variance_6);

    for(int i=0;i<count;i++){
        vector<vector <float>> randGen = generateChromosome(pop,dim);
        plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data_variance_6);

        Experiment exp;
        exp.population = pop;
        exp.iteration = itr;
        exp.variance = var;
        exp.final_fitness = fitness(geneticAlgorithmResults.chromoData ,train_data_variance_6);
        exp.valid_fitness = fitness(geneticAlgorithmResults.chromoData ,valid_data_variance_6);
        exp.static_score = static_score;
        exp.static_valid = fitness(static_weights, valid_data_variance_6);
        exp.weights = geneticAlgorithmResults.chromoData;
        exp.iteration_list = geneticAlgorithmResults.iteration;
        exp.fitness_growth = geneticAlgorithmResults.fitness;
        exp.exp_alpha = series;
        plot_and_save_experiment(exp);
    }
    //-------------------//




    pop = 100;
    itr = 200;
    series = "C";
    
    //EXPERIMENT Series V-4
    var = 4;
    static_score = fitness(static_weights ,train_data_variance_4);

    for(int i=0;i<count;i++){
        vector<vector <float>> randGen = generateChromosome(pop,dim);
        plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data_variance_4);

        Experiment exp;
        exp.population = pop;
        exp.iteration = itr;
        exp.variance = var;
        exp.final_fitness = fitness(geneticAlgorithmResults.chromoData ,train_data_variance_4);
        exp.valid_fitness = fitness(geneticAlgorithmResults.chromoData ,valid_data_variance_4);
        exp.static_score = static_score;
        exp.static_valid = fitness(static_weights, valid_data_variance_4);
        exp.weights = geneticAlgorithmResults.chromoData;
        exp.iteration_list = geneticAlgorithmResults.iteration;
        exp.fitness_growth = geneticAlgorithmResults.fitness;
        exp.exp_alpha = series;
        plot_and_save_experiment(exp);
    }
    //-------------------//

    //EXPERIMENT Series V-5
    var = 5;
    static_score = fitness(static_weights ,train_data_variance_5);

    for(int i=0;i<count;i++){
        vector<vector <float>> randGen = generateChromosome(pop,dim);
        plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data_variance_5);

        Experiment exp;
        exp.population = pop;
        exp.iteration = itr;
        exp.variance = var;
        exp.final_fitness = fitness(geneticAlgorithmResults.chromoData ,train_data_variance_5);
        exp.valid_fitness = fitness(geneticAlgorithmResults.chromoData ,valid_data_variance_5);
        exp.static_score = static_score;
        exp.static_valid = fitness(static_weights, valid_data_variance_5);
        exp.weights = geneticAlgorithmResults.chromoData;
        exp.iteration_list = geneticAlgorithmResults.iteration;
        exp.fitness_growth = geneticAlgorithmResults.fitness;
        exp.exp_alpha = series;
        plot_and_save_experiment(exp);
    }
    //-------------------//

    //EXPERIMENT Series V-6
    var = 6;
    static_score = fitness(static_weights ,train_data_variance_6);

    for(int i=0;i<count;i++){
        vector<vector <float>> randGen = generateChromosome(pop,dim);
        plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data_variance_6);

        Experiment exp;
        exp.population = pop;
        exp.iteration = itr;
        exp.variance = var;
        exp.final_fitness = fitness(geneticAlgorithmResults.chromoData ,train_data_variance_6);
        exp.valid_fitness = fitness(geneticAlgorithmResults.chromoData ,valid_data_variance_6);
        exp.static_score = static_score;
        exp.static_valid = fitness(static_weights, valid_data_variance_6);
        exp.weights = geneticAlgorithmResults.chromoData;
        exp.iteration_list = geneticAlgorithmResults.iteration;
        exp.fitness_growth = geneticAlgorithmResults.fitness;
        exp.exp_alpha = series;
        plot_and_save_experiment(exp);
    }
    //-------------------//


	return 0;
}
