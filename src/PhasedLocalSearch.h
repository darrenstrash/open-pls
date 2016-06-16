#ifndef PLS_H 
#define PLS_H

#include "Algorithm.h"
#include "ArraySet.h"

#include <string>
#include <vector>
#include <cstddef>

enum SelectionPhase {RANDOM_SELECTION, PENALTY_SELECTION, DEGREE_SELECTION };

class PhasedLocalSearch : public Algorithm
{
public:
    PhasedLocalSearch(std::vector<std::vector<int>> const &vAdjacencyArray, std::vector<double> const &vVertexWeights);

    void Perturb();

    void UpdatePenalties();

    int SelectRandom() const;
    int SelectPenalty() const;
    int SelectDegree() const;

////    void RunPhase(int const iterations, SelectionPhase selectionPhase);

    int SelectFromZero() const;
    int SelectFromOne();

    virtual bool Run();
    std::string GetName() const { return m_sName; }

    void SetTargetSize(size_t const uTargetSize);

    void SetMaxSelections(size_t const uMaxSelections);

    bool DiffIsEmpty(ArraySet const A, ArraySet const B) const;

    void AddToIndependentSet(int const vertex);

    void InitializeFromIndependentSet();

    bool IsConsistent() const;

protected:
    std::string m_sName;
    std::vector<std::vector<int>> const &m_vAdjacencyArray;
    std::vector<double>           const &m_vVertexWeights;
    std::vector<int>                     m_vVertexPenalties;
    size_t                               m_uTargetSize;
    size_t                               m_uMaxSelections;
    ArraySet                             m_IndependentSet;
    ArraySet                             m_U;
    ArraySet                             m_NotAdjacentToOne;
    ArraySet                             m_NotAdjacentToZero;
    double                               m_IndependentSetWeight;

    enum SelectionPhase                  m_SelectionPhase;
    mutable ArraySet                     m_ScratchSpace;

};

#endif //PLS_H
