#include <iostream>
#include <type_traits>
#include <iterator>
#include <string>
#include <algorithm>
#include <fstream>

#include "json.hpp"

#include "iir_filter.hpp"

using std::vector;
using std::cout;
using std::endl;
using std::for_each;
using std::fill;
using std::string;
using std::ofstream;

using json = nlohmann::json;

int main() {
    vector<double> input_coeffs = {0.7, 0.05, 0.12, 0.13};
    vector<double> output_coeffs = {-0.12, -0.7, -0.05, -0.13};
    IIRFilter<double> filter(input_coeffs.begin(), input_coeffs.end(), output_coeffs.begin(), output_coeffs.end());
    vector<double> signal;
    vector<double> output;
    double fs = 1000;
    for (int i = 0; i < 5000; ++i) {
        double t = static_cast<double>(i) / fs;
        double y = fmod(pow(t, 5), 1.0);
        signal.push_back(y);
    }
    for_each(signal.begin(), signal.end(), [&output, &filter](double sample) {
            //filter.filter(sample);
            output.push_back(filter.filter(sample));
        });

    string filepath = "output.json";
    ofstream ofs(filepath);
    json js = output;
    ofs << "{\n\"output\":" << endl;
    ofs << js.dump(4) << endl;
    ofs << ",\n\"signal\":" << endl;
    js = signal;
    ofs << js.dump(4) << endl;
    ofs << "}" << endl;
    ofs.close();

}


