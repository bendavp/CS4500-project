#include <iostream>
#include <string>
#include <vector>

#include "OptParser.hh"
#include "SoRParser.hh"
#include "StrConverter.hh"

void badArgError(const std::string &arg)
{
    std::cout << "Invalid argument " << arg << " provided" << std::endl;
    exit(1);
}

void missingArgError(const char *arg)
{
    std::cout << "Missing argument " << arg << std::endl;
    exit(1);
}

void tooManyCmdsError()
{
    std::cout << "Too many command arguments provided" << std::endl;
    exit(1);
}

namespace
{
//! The supported commands that can be run from the command line arguments
enum class SoRCLI
{
    UNKNOWN,
    PRINTCOLTYPE,
    PRINTCOLIDX,
    ISMISSINGIDX
};
} // namespace

int main(int argc, char *argv[])
{
    // Read command line args
    OptParser optParser;
    Opt opt;

    optParser.addFlag("f", 1);

    SoRParser sorParser;
    SoRCLI cmd = SoRCLI::UNKNOWN;
    std::vector<std::string> cmdArgs;
    bool hasCmd = false;

    std::string filename;
    int size = 0;
    bool found_size = false;
    int start = 0;
    bool found_start = false;

    while (!(opt = optParser.getOpt(argc, argv)).first.empty())
    {
        const std::string &flag = opt.first;
        const std::vector<std::string> &optVals = opt.second;
        if (flag == "f")
        {
            filename = optVals.front();
        }
        else
        {
            badArgError(flag);
        }
    }

    if (!sorParser.initialize(filename))
    {
        std::cout << "Unable to open file " << filename << std::endl;
        exit(1);
    }

    if (found_start)
        sorParser.setDataStart(start);
    if (found_size)
        sorParser.setDataSize(size);

    return 0;
}
