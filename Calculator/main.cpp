#include <iostream>
#include <fstream>
#include <optional>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>
#include <stack>
#include <map>
#include <exception>

#include "calculator.hpp"
#include "tests.hpp"

int main(int argc, char** argv){
    runAllTests();
    if (argc == 2){
        std::cout << "Evaluating File:\n";
        try{
            evaluateFile(argv[1]);
        }
        catch (const std::exception& err){
            std::cout << err.what() << "\n";
        }
    }
    else if (argc == 1){
        std::map<std::string, double> variables;
        std::map<std::string, Function> functions;
        std::cout << "Evaluating Line-by-Line: Please input your expressions\n";
        std::string line = " ";
        while (!line.empty()){
            try{
                std::cout << "> " << std::flush;
                getline(std::cin, line);
                if (!line.empty()){
                    double result = evaluateExpression(line, variables, functions);
                    if (std::find(line.begin(), line.end(), '=') == line.end() && std::find(line.begin(), line.end(), ':') == line.end())
                        std::cout << result << "\n";
                }
            }
            catch (const std::exception& err){
                std::cout << err.what() << "\n";
            }
        }
    }
    else
        throw Error{"[Error]: Too many command line arguments"};
    return 0;
}
