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

    int RandomSelect (ArraySet const &vertexSet) const;
    int PenaltySelect(ArraySet const &vertexSet) const;
    int DegreeSelect (ArraySet const &vertexSet) const;

////    void RunPhase(int const iterations, SelectionPhase selectionPhase);

    int SelectFrom(ArraySet const &vertexSet) const;
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

// Inputs.
    std::vector<std::vector<int>> const &m_vAdjacencyArray;
    std::vector<double>           const &m_vVertexWeights;

// Penalty variables.
    std::vector<int>                     m_vVertexPenalties;
    size_t                               m_uPenaltyDelay;
    size_t                               m_uNumZeroPenaltyVertices;
    size_t                               m_uTargetZeroPenaltyVertices;
    size_t                               m_uIterationsSinceLastPenaltyUpdate;

    size_t                               m_uTargetSize;
    size_t                               m_uMaxSelections;

// Sets
    ArraySet                             m_IndependentSet;
    ArraySet                             m_U;
    ArraySet                             m_NotAdjacentToOne;
    ArraySet                             m_NotAdjacentToZero;
    mutable ArraySet                     m_ScratchSpace;

// Progress Tracking
    enum SelectionPhase                  m_SelectionPhase;
    size_t                               m_IndependentSetWeight;
    size_t                               m_uBestWeight;

};

#endif //PLS_H
