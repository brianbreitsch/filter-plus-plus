#include <iostream>
#include <type_traits>
#include <iterator>
#include <string>
#include <algorithm>
#include <fstream>
#include <streambuf>

#include "json.hpp"

#include "filter.hpp"

using std::vector;
using std::cout;
using std::endl;
using std::for_each;
using std::fill;
using std::string;
using std::ofstream;
using std::ifstream;
using std::istreambuf_iterator;
using std::inner_product;

using json = nlohmann::json;

using fpp::IIRFilter;

double pi = 3.14159265358979323846;

void generate_chirp(vector<double>& signal, double fs, double T, double start, double stop) {
    int N = static_cast<int>(T * fs);
    for (double i = 0; i < N; i += 1) {
        double t = i / fs;
        double f = start + i / N * (stop - start);
        double y = sin(2 * pi * f * t);
        signal.push_back(y);
    }
}

void generate_test_input(string& filepath) {
    // bwlp 2nd order .1 wc normalized
    vector<double> numerator = {0.02008337,  0.04016673,  0.02008337};
    vector<double> denominator = {1.        , -1.56101808,  0.64135154};
    vector<double> chirp;
    generate_chirp(chirp, 100, 5.0, 1.0, 10.0);

    json js;
    js["numerator"] = numerator;
    js["denominator"] = denominator;
    js["signal"] = chirp;

    ofstream ofs(filepath);
    ofs << js.dump(4);
}

void filter(vector<double>& input, vector<double>& numerator, vector<double>& denominator, vector<double>& output) {
    IIRFilter<double> filter(numerator, denominator);
    cout << filter.state() << endl;
    for_each(input.begin(), input.end(), [&output, &filter](double sample) {
        output.push_back(filter.f(sample));
    });
    cout << filter.state() << endl;
}

int main(int argc, char** argv) {
    string input_filepath("input.json"), output_filepath("output.json");
    if (argc > 1 && string(argv[1]) == "test") {
        // TODO
        return 0;
    }
    if (argc > 2 && string(argv[1]) == "generate") {
        input_filepath = argv[2];
        generate_test_input(input_filepath);
        return 0;
    }
    if (argc > 2) {
        input_filepath = argv[1];
        output_filepath = argv[2];
        // read in json file
        ifstream ifs(input_filepath);
        string filetext((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
        json js = json::parse(filetext);
        // read json attributes
        vector<double> signal, numerator, denominator, output;
        signal = js["signal"].get<vector<double>>();
        numerator = js["numerator"].get<vector<double>>();
        denominator = js["denominator"].get<vector<double>>();
        // filter data
        filter(signal, numerator, denominator, output);
        // write out json
        ofstream ofs(output_filepath);
        js.clear();
        js["output"] = output;
        js["signal"] = signal;
        ofs << js.dump(4) << endl;
        ofs.close();
        // success!
        return 0;
    }
    cout << "Usage: filter <input_filepath.json> <output_filepath.json>" << endl;
    return 0;
}
