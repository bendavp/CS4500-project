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
    // int size = 0;
    // bool found_size = false;
    // int start = 0;
    // bool found_start = false;

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

    // if (found_start)
    //     sorParser.setDataStart(start);
    // if (found_size)
    //     sorParser.setDataSize(size);

    // getting total row number
    std::ifstream inFile(filename);
    unsigned int numRows = std::count(std::istreambuf_iterator<char>(inFile),
                                      std::istreambuf_iterator<char>(), '\n');
    // getting total col number
    unsigned int numCols = sorParser.getNumCols();

    // filling in the char* that will define the schema
    char *schema_str = new char[numCols];
    for (unsigned int i = 0; i < numCols; i++)
    {
        switch (sorParser.getColType(i))
        {
        case SoRType::BOOL:
            schema_str[i] = 'B';
            break;
        case SoRType::INT:
            schema_str[i] = 'I';
            break;
        case SoRType::FLOAT:
            schema_str[i] = 'F';
            break;
        case SoRType::STRING:
            schema_str[i] = 'S';
            break;
        }
    }

    Schema *s = new Schema(schema_str);

    DataFrame *df = new DataFrame(*s);

    // filling in dataframe from the sorparser
    Row *r = new Row(df->getSchema());
    for (unsigned int i = 0; i < numRows; i++)
    {
        for (unsigned int j = 0; j < numCols; j++)
        {
            std::string toAdd = sorParser.getColIdxStr(j, i);

            switch (df->get_schema().col_type(j))
            {
            case 'B':
                bool b_ = std::stoi(toAdd);
                r->set(j, b_);
                break;
            case 'I':
                int i_ = std::stoi(toAdd);
                r->set(j, i_);
                break;
            case 'F':
                float f_ = std::stof(toAdd);
                r->set(j, f_);
                break;
            case 'S':
                String s_ = new String(toAdd.c_str());
                r->set(j, s_);
                break;
            }
        }
        df->add_row(*r);
    }

    delete r;

    return 0;
}
