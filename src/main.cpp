#include "app/wordcounter.h"
#include "adapter/arg.h"
#include "adapter/OptParser.h"
#include "adapter/OptParser.cpp"

Arg arg;

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

int main(int argc, char *argv[])
{
    // Read command line args
    OptParser optParser;
    Opt opt;

    optParser.addFlag("f", 1);
    optParser.addFlag("n", 1);

    std::vector<std::string> cmdArgs;

    std::string filename;
    size_t num_nodes;

    while (!(opt = optParser.getOpt(argc, argv)).first.empty())
    {
        const std::string &flag = opt.first;
        const std::vector<std::string> &optVals = opt.second;
        if (flag == "f")
        {
            filename = optVals.front();
        }
        else if (flag == "n")
        {
            num_nodes = stoi(optVals.front());
        }
        else
        {
            badArgError(flag);
        }
    }

    Arg arg = Arg(num_nodes, filename.c_str());

    PseudoNetwork *network_ = new PseudoNetwork(arg.num_nodes);
    Node **nodes = new Node *[num_nodes];
    Application **apps = new Application *[num_nodes];
    for (int i = 0; i < arg.num_nodes; i++)
    {
        nodes[i] = new Node(i, network_);
        nodes[i]->start();
    }
    for (int i = 0; i < arg.num_nodes; i++)
    {
        apps[i] = new WordCount(i, nodes[i]);
        apps[i]->start();
    }
}