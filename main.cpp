#include <iostream>
#include <fstream>
#include "image.h"
#include "filter.h"

using std::string;
using std::vector;

namespace Bmp_Checker {
    bool check_bmp_file(const string &File) {
        size_t file_size = File.size();
        if (file_size <= 4) {
            return false;
        }
        if (File[file_size - 1] != 'p') {
            return false;
        }
        if (File[file_size - 2] != 'm') {
            return false;
        }
        if (File[file_size - 3] != 'b') {
            return false;
        }
        if (File[file_size - 4] != '.') {
            return false;
        }
        return true;
    }
}

namespace Query_Manager {
    bool is_integer(std::string_view number) {
        for (auto i : number) {
            if (i < '0' || i > '9') {
                return false;
            }
        }
        return true;
    }

    bool is_double(std::string_view number) {
        int cnt = 0;
        for (auto i : number) {
            if ((i < '0' || i > '9') && (i != '.')) {
                return false;
            }
            if (i == '.') {
                ++cnt;
                if (cnt > 1) {
                    return false;
                }
            }
        }
        return true;
    }

    void do_query(Image &current_image, const std::vector<std::vector<std::string_view> > &args) {
        for (const auto &arg : args) {
            if (arg[0] == "-crop") {
                if (arg.size() < 3) {
                    std::cout << "Not enough arguments in -crop\n";
                    exit(0);
                }
                if (arg.size() > 3) {
                    std::cout << "Too much arguments in -crop\n";
                    exit(0);
                }
                if ((!is_integer(arg[1])) || (!is_integer(arg[2]))) {
                    std::cout << "-crop arguments must be integer\n";
                    exit(0);
                }
                int width = std::stoi(std::string(arg[1]));
                int height = std::stoi(std::string(arg[2]));
                Corp filter(height, width);
                filter.Apply(current_image);
            } else if (arg[0] == "-gs") {
                if (arg.size() > 1) {
                    std::cout << "-gs need no arguments\n";
                    exit(0);
                }
                Grayscale filter;
                filter.Apply(current_image);
            } else if (arg[0] == "-neg") {
                if (arg.size() > 1) {
                    std::cout << "-neg need no arguments\n";
                    exit(0);
                }
                Negative filter;
                filter.Apply(current_image);
            } else if (arg[0] == "-sharp") {
                if (arg.size() > 1) {
                    std::cout << "-sharp need no arguments\n";
                    exit(0);
                }
                Sharpening filter;
                filter.Apply(current_image);
            } else if (arg[0] == "-edge") {
                if (arg.size() < 2) {
                    std::cout << "Not enough arguments in -edge\n";
                    exit(0);
                }
                if (arg.size() > 2) {
                    std::cout << "Too much arguments in -edge\n";
                    exit(0);
                }
                if (!is_double(arg[1])) {
                    std::cout << "-edge arguments must be double\n";
                    exit(0);
                }
                long double threshold = std::stold(std::string(arg[1]));
                EdgeDetection filter(threshold);
                filter.Apply(current_image);
            } else if (arg[0] == "-blur") {
                if (arg.size() < 2) {
                    std::cout << "Not enough arguments in -blur\n";
                    exit(0);
                }
                if (arg.size() > 2) {
                    std::cout << "Too much arguments in -blur\n";
                    exit(0);
                }
                if (!is_double(arg[1])) {
                    std::cout << "-blur arguments must be double\n";
                    exit(0);
                }
                int sigma = std::stoi(std::string(arg[1]));
                GaussianBlur filter(sigma);
                filter.Apply(current_image);
            } else if (arg[0] == "-brightness") {
                if (arg.size() < 2) {
                    std::cout << "Not enough arguments in -brightness\n";
                    exit(0);
                }
                if (arg.size() > 2) {
                    std::cout << "Too much arguments in -brightness\n";
                    exit(0);
                }
                if (!is_integer(arg[1])) {
                    std::cout << "-brightness arguments must be integer\n";
                    exit(0);
                }
                int percentage = std::stoi(std::string(arg[1]));
                Brightness filter(percentage);
                filter.Apply(current_image);
            } else if (arg[0] == "-help") {
                if (arg.size() > 1) {
                    std::cout << "-help need no arguments\n";
                    exit(0);
                }
                std::cout << "Input and output files should be .bmp format.\n"
                             "{Name of program} {Path to input file} {Path to output file} [-{Filter name} {Filter's parameters} ...] [...]\n";
            } else {
                std::cout << "Invalid query\n";
                exit(0);
            }
        }
    }
}


namespace Arguments {
    std::vector<std::vector<std::string_view>> SplitArgs(int argc, char **argv) {
        std::vector<std::vector<std::string_view>> args;
        for (int i = 3; i < argc; ++i) {
            if (argv[i][0] == '-') {
                args.push_back({argv[i]});
            } else {
                args.back().push_back(argv[i]);
            }
        }
        return args;
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cout << "Not enough arguments\n";
        return 0;
    }
    string input_name = argv[1];
    string output_name = argv[2];
    if (!Bmp_Checker::check_bmp_file(input_name) || !Bmp_Checker::check_bmp_file(output_name)) {
        std::cout << "Input file is not .bmp\n";
        return 0;
    }
    std::ifstream input{input_name, std::ios::binary};
    std::ofstream output{output_name, std::ios::binary};
    if (!input) {
        std::cout << "Error opening input file\n";
        return 0;
    }
    if (!output) {
        std::cout << "Error opening output file\n";
        return 0;
    }
    Image current_image;
    try {
        current_image.Read(input);
    } catch (const std::exception &e) {
        std::cout << "Invalid input file\n";
    }
    input.close();
    auto args = Arguments::SplitArgs(argc, argv);
    Query_Manager::do_query(current_image, args);
    current_image.Write(output);
    output.close();
    return 0;
}
