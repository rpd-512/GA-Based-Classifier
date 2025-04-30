#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <indicators/block_progress_bar.hpp>
#include <indicators/cursor_control.hpp>
#include <thread>
#include <csignal>
#include <cstdlib>

#include <omp.h>

#include "matplotlibcpp.h"
#include "src.h"

using namespace std;
namespace plt = matplotlibcpp;

plotPoint geneticAlgorithm(int popl, int itrn, vector<vector <float>> popArr, vector<vector<float>> train_data){
    plotPoint plotData;
    int crossValue = 3;
    float crossProb = 0.5;
    float mutateProb = 0.25;
    int eliteVal = popl/5;

    int dim = popArr.at(0).size();

    indicators::show_console_cursor(false);
    using namespace indicators;
    BlockProgressBar bar{
        option::BarWidth{40},
        option::ForegroundColor{Color::white},
        option::PrefixText{"Genetic Algorithm "},
        option::ShowElapsedTime{true},
        option::Start{"|"},
        option::End{"|"},
        option::ShowRemainingTime{true},
        option::FontStyles{
            std::vector<FontStyle>{FontStyle::bold}},
        option::MaxProgress{itrn}
    };

    
    vector<chromoInfo> popData;
    vector<chromoInfo> eliteData;
    //Generation 0
    for(int u=0;u<popl;u++){
        vector<float> valArr = popArr[u];
        chromoInfo valChromo;
        valChromo.gene = valArr;
        valChromo.fitness = fitness(valArr,train_data);
        popData.push_back(valChromo);
    }
    chromoInfo bestPop;
    for(int gen=0;gen<itrn+1;gen++){
        //-----------//
        bar.set_option(option::PostfixText{
        std::to_string(gen) + "/" + std::to_string(itrn)
        });
        bar.tick();
        //-----------//

        //sort the vector
        partial_sort(popData.begin(), popData.begin() + popl, popData.end(),
                     [](const chromoInfo& a, const chromoInfo& b) { return b.fitness < a.fitness; });

        //popData.reserve(popl);
        popData.erase(popData.begin() + popl, popData.end());
        popData.shrink_to_fit();
        eliteData.assign(popData.begin(), popData.begin() + eliteVal);

   
        bestPop = popData[0];
        plotData.iteration.push_back(gen);
        plotData.fitness.push_back(bestPop.fitness);
        
        //cout << gen << "\t" << bestPop.fitness << "\t\t";print_vector(bestPop.gene);

        for(int p=0; p<popl; p++){
            vector<float>& chromoMain = popData[p].gene;
            int rand_p =randint(0,popl-1);
            vector<float>& chromoRand = popData[rand_p].gene;
            int crossOverNum = randint(0,popArr[p].size() - crossValue);
            //crossover
            if(uniform(0,1) < crossProb){
                swap_ranges(chromoMain.begin() + crossOverNum,
                        chromoMain.begin() + crossOverNum + crossValue,
                        chromoRand.begin() + crossOverNum);
            }
            for(int g=crossOverNum;g<crossOverNum+crossValue;g++){
                if(uniform(0,1)<mutateProb){
                    swap(chromoMain[g],chromoMain[randint(0,dim-1)]);
                    swap(chromoRand[g],chromoRand[randint(0,dim-1)]);
                }
                    chromoMain[g] += uniform(-2,2);
                    chromoRand[g] += uniform(-2,2);
            }
        }
        #pragma omp parallel for
        for(int p=0; p<popl; p++){
            popData[p].gene = repair_weights(popData[p].gene);
            vector<float>& chromoMain = popData[p].gene;
            popData[p].fitness = fitness(chromoMain,train_data);
        }
        popData.insert(popData.end(),eliteData.begin(),eliteData.end());
    }
    //bar.mark_as_completed();
    indicators::show_console_cursor(true);
    plotData.chromoData = bestPop.gene;
    return plotData;
}

#ifdef MAIN_MODULE

int main(){
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    srand(time(0));
    int pop = 100;
    int itr = 200;

    int dim = 8;

    vector<double> v(dim); iota(v.begin(), v.end(), 1);

    vector<vector <float>> randGen = generateChromosome(pop,dim);
    vector<vector<float>> train_data = auth_read("../generate_dataset_in_accordance/dataset_var_6_threshold_2.5/training_data.json");
    vector<vector<float>> valid_data = auth_read("../generate_dataset_in_accordance/dataset_var_6_threshold_2.5/validating_data.json");

    clock_t start = std::clock();
    
	plotPoint geneticAlgorithmResults = geneticAlgorithm(pop,itr,randGen,train_data);
    
	clock_t end = std::clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;
    cout << "Time taken: " << elapsed << " seconds" << std::endl;

	cout << "Optimized Weights: ";
	print_vector(geneticAlgorithmResults.chromoData);
	
	cout << "Final fitness value:\t" << geneticAlgorithmResults.fitness[geneticAlgorithmResults.fitness.size()-1] << endl;
	
	cout << "Fitness over validating dataset:" << fitness(geneticAlgorithmResults.chromoData ,valid_data) << endl;

    gen_heatmap(geneticAlgorithmResults.chromoData);

    plt::figure_size(1000,800);
    plt::plot(geneticAlgorithmResults.iteration, geneticAlgorithmResults.fitness, {{"label", "Genetic Algorithms"},{"color","red"}});

    plt::xlabel("Iterations");
    plt::ylabel("Fitness");

    plt::show();
	return 0;
}

#endif