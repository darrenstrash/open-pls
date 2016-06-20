
#include "PhasedLocalSearch.h"

#include <vector>

class IndependentSetPhasedLocalSearch : public PhasedLocalSearch
{
    public:
    IndependentSetPhasedLocalSearch(std::vector<std::vector<int>> const &vAdjacencyArray, std::vector<double> const &vVertexWeights);
    virtual ~IndependentSetPhasedLocalSearch() {}

    virtual void AddToK(int const vertex);

    virtual void InitializeFromK();
    virtual void InitializeFromK2();
};
