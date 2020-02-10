#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

class Options
{
public:
    Options();
    void parse(int argc, char** argv);
    bool multi_processing;
    int max_runs;
    std::string output_directory;
    std::string experiments_file;
};

#endif /* ifndef OPTIONS_H */
