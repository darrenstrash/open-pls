
#include "PhasedLocalSearch.h"

#include <vector>

class CliquePhasedLocalSearch : public PhasedLocalSearch
{
    public:
    CliquePhasedLocalSearch(std::vector<std::vector<int>> const &vAdjacencyArray, std::vector<double> const &vVertexWeights);
    virtual ~CliquePhasedLocalSearch() {}

    virtual int DegreeSelect(ResetableArraySet const &vertexSet) const;

    virtual void AddToK(int const vertex);

    virtual void InitializeFromK();
    virtual void InitializeFromK2();

    virtual bool IsConsistent() const;

    virtual void ForceIntoK(int const vertex, bool updateU);
};
