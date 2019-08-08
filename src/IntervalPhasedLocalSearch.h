#ifndef INTERVAL_PLS_H
#define INTERVAL_PLS_H

#include "PhasedLocalSearch.h"
#include "constraintkrestricted.h"

#include <vector>

class IntervalPhasedLocalSearch : public PhasedLocalSearch
{
    public:
    IntervalPhasedLocalSearch(std::vector<std::vector<int>> const &vAdjacencyArray, std::vector<double> const &vVertexWeights, size_t const uRestrictedSize);
    virtual ~IntervalPhasedLocalSearch() 
    {
        delete m_pRestricted;
        m_pRestricted = nullptr;
    }

    virtual int DegreeSelect(ResetableArraySet const &vertexSet) const;

    virtual void AddToK(int const vertex);

    virtual void InitializeFromK();
    virtual void InitializeFromK2(bool const updateU);

    virtual bool IsConsistent() const;

    virtual void ForceIntoK(int const vertex, bool const updateU);

    virtual void AddToKFromOne(int const vertex);

    void SetIntervals(std::vector<std::pair<int,int>> const &vIntervals) {
        assert(m_pRestricted == nullptr);
        assert(m_uRestrictedSize != std::numeric_limits<size_t>::max());
        m_pRestricted = new ConstraintKRestricted(m_uRestrictedSize, vIntervals);
        intervals = vIntervals;
    }

    protected:
    size_t m_uRestrictedSize;
    ConstraintKRestricted *m_pRestricted;
    std::vector<std::pair<int,int>> intervals;

};

#endif //INTERVAL_PLS_H
