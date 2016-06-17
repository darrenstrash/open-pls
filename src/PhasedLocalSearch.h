#ifndef PLS_H 
#define PLS_H

#include "Algorithm.h"
#include "ArraySet.h"

#include <string>
#include <vector>
#include <cstddef>
#include <ctime>

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

    bool Phase(size_t iterations, SelectionPhase const selectionPhase);

    virtual bool Run();
    std::string GetName() const { return m_sName; }

    void SetTargetSize(size_t const uTargetSize);

    void SetMaxSelections(size_t const uMaxSelections);

    bool DiffIsEmpty(ArraySet const A, ArraySet const B) const;

    void AddToIndependentSet(int const vertex);

    void InitializeFromIndependentSet();

    bool IsConsistent() const;

    void SetTimeOutInMilliseconds(size_t const timeout);
    void   SetTargetWeight(size_t const targetWeight);
    size_t GetTargetWeight() { return m_uTargetWeight; }

    size_t GetSelections() const { return m_uSelections; }
    size_t GetBestWeight() const { return m_uBestWeight; }

    clock_t GetTimeToBestWeight() const { return m_TimeToReachBestWeight; }
    size_t  GetSelectionsToBestWeight() const { return m_uSelectionsToBestWeight; }

    void SetQuiet(bool const quiet) { m_bQuiet = quiet; }

    double GetTimeoutInSeconds() const;
    size_t GetMaxSelections() const { return m_uMaxSelections; }

protected:
    std::string m_sName;

// Inputs.
    std::vector<std::vector<int>> const &m_vAdjacencyArray;
    std::vector<double>           const &m_vVertexWeights;

// Penalty variables.
    std::vector<int>                     m_vVertexPenalties;
    size_t                               m_uPenaltyDelay;
    size_t                               m_uNumPenalizedVertices;
    size_t                               m_uTargetPenalizedVertices;
    size_t                               m_uIterationsSinceLastPenaltyUpdate;

    size_t                               m_uTargetSize;
    size_t                               m_uTargetWeight;
    size_t                               m_uMaxSelections;
    size_t                               m_uSelections;

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
    size_t                               m_uBestSize;
    clock_t                              m_TimeToReachBestWeight;
    size_t                               m_uSelectionsToBestWeight;
    clock_t                              m_StartTime;
    clock_t                              m_TimeOut;

    bool                                 m_bQuiet;
};

#endif //PLS_H
