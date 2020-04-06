#include "adapter/arg.h"
#include "app/wordcounter.h"

Arg arg;

int main(int argc, char *argv[])
{
    arg.parse(argc, argv);

    PseudoNetwork *network_ = new PseudoNetwork(arg.num_nodes);
    Node **nodes = new Node *[arg.num_nodes];
    Application **apps = new Application *[arg.num_nodes];

    for (int i = 0; i < arg.num_nodes; i++)
    {
        nodes[i] = new Node(i, network_);
        std::cout << "Node " << i << " registered..." << std::endl;
        nodes[i]->start();
        std::cout << "... and started" << std::endl;
    }
    for (int i = 0; i < arg.num_nodes; i++)
    {
        apps[i] = new WordCount(i, nodes[i]);
        apps[i]->sleep(1000);
        std::cout << "App " << i << " started..." << std::endl;
    }
    for (int i = 0; i < arg.num_nodes; i++)
    {
        apps[i]->run();
        std::cout << "... and running" << std::endl;
    }
}
