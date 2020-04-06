#pragma once

#include "OptParser.h"
#include "OptParser.cpp"
#include "../utils/object.h"
#include <stdio.h>

class Arg : public Object
{
public:
    size_t num_nodes = 0;
    const char *file = nullptr;

    void badArgError(const std::string &arg_)
    {
        std::cout << "Invalid argument " << arg_ << " provided" << std::endl;
        exit(1);
    }

    void missingArgError(const char *arg_)
    {
        std::cout << "Missing argument " << arg_ << std::endl;
        exit(1);
    }

    void tooManyCmdsError()
    {
        std::cout << "Too many command arguments provided" << std::endl;
        exit(1);
    }

    void parse(int argc, char *argv[])
    {
        // Read command line args
        OptParser optParser;
        Opt opt;

        optParser.addFlag("f", 1);
        optParser.addFlag("n", 1);

        std::vector<std::string> cmdArgs;

        while (!(opt = optParser.getOpt(argc, argv)).first.empty())
        {
            const std::string &flag = opt.first;
            const std::vector<std::string> &optVals = opt.second;
            if (flag == "f")
            {
                file = optVals.front().c_str();
            }
            else if (flag == "n")
            {
                num_nodes = atoi(optVals.front().c_str());
            }
            else
            {
                badArgError(flag);
            }
        }
    }
};

extern Arg arg;