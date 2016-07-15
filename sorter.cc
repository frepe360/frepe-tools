#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <set>

using namespace std;

class Plate {
public:
    string number;
    float confidence;
};

int main(int argc, char* argv[]) {

    set<string> plates;

    if(argc != 2) {
        cout << "Usage: sorter filename.log" << endl;
        exit(0);
    }

    ifstream myfile(argv[1]);

    if(!myfile.is_open()) {
        cout << argv[1] << " cannot be found" << endl;
        exit(0);
    }

    string line;
    while(getline(myfile, line)) {
        if(line.substr(0,5) == "plate") {
            int noOfResults = stoi(line.substr(line.find(":") + 1, line.find("r")));
            float maxConf = 85.0; // We don't use results under 85
            for(int i = 0; i < noOfResults; ++i) {
                getline(myfile, line);
                if (line.substr(line.length() - 1, line.length()) == "1") {
                    string newPlate;
                    newPlate = line.substr(line.find("-") + 2, line.find("-") + 2);
                    // I, Q and V are not used on swedish plates
                    if(newPlate.find("I") != string::npos) continue;
                    if(newPlate.find("Q") != string::npos) continue;
                    if(newPlate.find("V") != string::npos) continue;
                    int confidence = stof(line.substr(line.find(":") + 1, line.find("p")));
                    if(confidence > maxConf) {
                        plates.insert(newPlate);
                        maxConf = confidence;
                    }

                }
            }
        }
    }

    for(auto const& p: plates) {
        cout << p << endl;
    }

    myfile.close();
}
