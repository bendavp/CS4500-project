#include <iostream>
#include <string>
#include <vector>

#include "OptParser.hh"
#include "SoRParser.hh"
#include "StrConverter.hh"

void badArgError(const std::string& arg) {
    std::cout << "Invalid argument " << arg << " provided" << std::endl;
    exit(1);
}

void missingArgError(const char* arg) {
    std::cout << "Missing argument " << arg << std::endl;
    exit(1);
}

void tooManyCmdsError() {
    std::cout << "Too many command arguments provided" << std::endl;
    exit(1);
}

namespace {
//! The supported commands that can be run from the command line arguments
enum class SoRCLI { UNKNOWN, PRINTCOLTYPE, PRINTCOLIDX, ISMISSINGIDX };
}  // namespace

int main(int argc, char* argv[]) {
    // Read command line args
    OptParser optParser;
    Opt opt;

    optParser.addFlag("f", 1);
    optParser.addFlag("from", 1);
    optParser.addFlag("len", 1);
    optParser.addFlag("print_col_type", 1);
    optParser.addFlag("print_col_idx", 2);
    optParser.addFlag("is_missing_idx", 2);

    SoRParser sorParser;
    SoRCLI cmd = SoRCLI::UNKNOWN;
    std::vector<std::string> cmdArgs;
    bool hasCmd = false;

    std::string filename;
    int size = 0;
    bool found_size = false;
    int start = 0;
    bool found_start = false;

    while (!(opt = optParser.getOpt(argc, argv)).first.empty()) {
        const std::string& flag = opt.first;
        const std::vector<std::string>& optVals = opt.second;
        if (flag == "f") {
            filename = optVals.front();
        } else if (flag == "from") {
            if (found_start) badArgError(flag);
            const std::string& dataStartStr = optVals.front();
            unsigned long value;
            if (!StrConverter::isUInt(dataStartStr, value))
                badArgError(dataStartStr);
            start = value;
            found_start = true;
        } else if (flag == "len") {
            if (found_size) badArgError(flag);
            const std::string& dataSizeStr = optVals.front();
            unsigned long value;
            if (!StrConverter::isUInt(dataSizeStr, value))
                badArgError(dataSizeStr);
            size = value;
            found_size = true;
        } else if (flag == "print_col_type") {
            if (hasCmd) tooManyCmdsError();
            hasCmd = true;
            cmd = SoRCLI::PRINTCOLTYPE;
            cmdArgs = optVals;
        } else if (flag == "print_col_idx") {
            if (hasCmd) tooManyCmdsError();
            hasCmd = true;
            cmd = SoRCLI::PRINTCOLIDX;
            cmdArgs = optVals;
        } else if (flag == "is_missing_idx") {
            if (hasCmd) tooManyCmdsError();
            hasCmd = true;
            cmd = SoRCLI::ISMISSINGIDX;
            cmdArgs = optVals;
        } else {
            badArgError(flag);
        }
    }

    if (!sorParser.initialize(filename)) {
        std::cout << "Unable to open file " << filename << std::endl;
        exit(1);
    }

    if (found_start) sorParser.setDataStart(start);
    if (found_size) sorParser.setDataSize(size);

    if (cmd == SoRCLI::PRINTCOLTYPE) {
        std::cout << sorParser.getColType(std::stoul(cmdArgs[0])) << std::endl;
    } else if (cmd == SoRCLI::PRINTCOLIDX) {
        std::cout << sorParser.getColIdxStr(std::stoul(cmdArgs[0]),
                                         std::stoul(cmdArgs[1]))
                  << std::endl;
    } else if (cmd == SoRCLI::ISMISSINGIDX) {
        std::cout << sorParser.isMissingIdx(std::stoul(cmdArgs[0]),
                                            std::stoul(cmdArgs[1]))
                  << std::endl;
    }

    return 0;
}
