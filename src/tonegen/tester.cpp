#include "genome.h"

using namespace h2p_tonegen;

int main() {
    Genome genome;      // test default constructor (if we have one)
    Genome::Initializer(genome);
    Genome::Evaluator(genome);
    return 0;
}


