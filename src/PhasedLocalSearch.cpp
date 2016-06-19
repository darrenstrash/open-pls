#include "PhasedLocalSearch.h"
#include "Tools.h"

// system includes
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <climits>

////#define ALLOW_OVERLAP
////#define CHECK_CONSISTENCY
////#define DEBUG

using namespace std;

PhasedLocalSearch::PhasedLocalSearch(vector<vector<int>> const &vAdjacencyArray, vector<double> const &vVertexWeights)
: Algorithm("PLS")
, m_vAdjacencyArray(vAdjacencyArray)
, m_vVertexWeights(vVertexWeights)

// Penalty variables
, m_vVertexPenalties(vAdjacencyArray.size(), 0)
, m_uPenaltyDelay(2)
, m_uNumPenalizedVertices(vAdjacencyArray.size())
, m_uTargetPenalizedVertices(0.75*vAdjacencyArray.size())
, m_uIterationsSinceLastPenaltyUpdate(0)

, m_uTargetSize(vAdjacencyArray.size())
// initial weight, TODO/DS: change.
, m_uTargetWeight(ULONG_MAX)
, m_uMaxSelections(100000000)
, m_uSelections(0)

// Sets
, m_IndependentSet(vAdjacencyArray.size())
, m_RandomIndependentSet(vAdjacencyArray.size()) // for saving state of random phase
, m_DegreeIndependentSet(vAdjacencyArray.size()) // for saving state of degree phase
, m_U(vAdjacencyArray.size())
, m_NotAdjacentToOne(vAdjacencyArray.size())
, m_NotAdjacentToZero(vAdjacencyArray.size())
, m_ScratchSpace(vAdjacencyArray.size())
, m_vScratchCounters(vAdjacencyArray.size(),0)
, m_bCheckZero(true)
, m_bCheckOne(true)

// Progress Tracking
, m_SelectionPhase(SelectionPhase::RANDOM_SELECTION)
, m_IndependentSetWeight(0.0)
, m_uBestWeight(0)
, m_uBestSize(0)
, m_StartTime(0)
, m_TimeToReachBestWeight(0)
, m_uSelectionsToBestWeight(0)
, m_TimeOut(ULONG_MAX)
, m_bQuiet(true)
{
    for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {
        m_NotAdjacentToZero.Insert(vertex);
    }
}

void PhasedLocalSearch::Perturb()
{
#ifdef DEBUG
    cout << "Perturbing..." << endl << flush;
#endif // DEBUG

    m_IndependentSet.Clear();
    int const randomVertex(rand()%m_vAdjacencyArray.size());

    // Set $K$ to contain only random vertex.
    if (SelectionPhase::PENALTY_SELECTION) {
        m_IndependentSet.Insert(randomVertex);
        InitializeFromIndependentSet2();
        return;
    }

    // Add random vertex to $K$, remove non-neighbors from $K$.
    m_IndependentSet.IntersectInPlace(m_vAdjacencyArray[randomVertex]);
    m_IndependentSet.Insert(randomVertex);
    InitializeFromIndependentSet2();
}

void PhasedLocalSearch::UpdatePenalties()
{
    if (m_uPenaltyDelay > m_uIterationsSinceLastPenaltyUpdate) {
        m_uIterationsSinceLastPenaltyUpdate++;
        return;
    }

#ifdef DEBUG
    cout << "Updating penalties..." << flush;
#endif // DEBUG

    m_uIterationsSinceLastPenaltyUpdate = 0;
    m_uNumPenalizedVertices = 0;

    // TODO/DS: Don't actually store penalties.
    // store in array, ordered by penalty, with separator indices
    // in another array. Updates change separator indices (easy)
    // Just need to be careful about updating clique vertices
    // Idea: Somehow update clique vertices after?

    // All items in clique have penalty increased by one
    // All others have penalty decreased by one.
    for (size_t vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {
        if (m_IndependentSet.Contains(vertex)) {
            m_vVertexPenalties[vertex]++;
        } else {
            if (m_vVertexPenalties[vertex] > 0) {
                m_vVertexPenalties[vertex]--;
            }
        }
        if (m_vVertexPenalties[vertex] != 0) {
            m_uNumPenalizedVertices++;
        }
    }

    for (int const independentVertex : m_IndependentSet) {
        m_vVertexPenalties[independentVertex]++;
    }

    //// Update the Penalty Delay.
    //// TODO/DS: Should update happen before or after penalties update?

    // if < 75% of vertices have penalty > 0, increase penalty delay by one
    // otherwise, penalty delay goes down by one.
    if (m_uNumPenalizedVertices < m_uTargetPenalizedVertices) {
        m_uPenaltyDelay++;
    } else if (m_uPenaltyDelay > 0) {
        m_uPenaltyDelay--;
    }

#ifdef DEBUG
    cout << "Done!" << endl << flush;
#endif // DEBBUG
}

int PhasedLocalSearch::RandomSelect(ArraySet const &vertexSet) const
{
    return *(vertexSet.begin() + rand()%vertexSet.Size());
}

int PhasedLocalSearch::PenaltySelect(ArraySet const &vertexSet) const
{
    assert(!vertexSet.Empty());
#ifdef DEBUG
    cout << "PenaltySelect..." << flush;
#endif // DEBUG
    // don't select vertex with penalty 10?
    size_t minPenalty(ULONG_MAX);
    m_ScratchSpace.Clear();
    for (int const vertex : vertexSet) {
        if (m_vVertexPenalties[vertex] < minPenalty) {
            m_ScratchSpace.Clear();
            minPenalty = m_vVertexPenalties[vertex];
            m_ScratchSpace.Insert(vertex);
        } else if (m_vVertexPenalties[vertex] == minPenalty) {
            m_ScratchSpace.Insert(vertex);
        }
    }
    assert(!m_ScratchSpace.Empty());

////    cout << "vertex-set-size=" << vertexSet.Size() << ", scratch-space-size=" << m_ScratchSpace.Size() << endl << flush;
////    if (vertexSet.Size() == 2 && m_ScratchSpace.Size() == 0) {
////        cout << "vs={" << *(vertexSet.begin()) << "," << *(vertexSet.begin() + 1) << "}" << endl << flush;
////    }
    int const vertexToReturn = *(m_ScratchSpace.begin() + rand()%m_ScratchSpace.Size());
    m_ScratchSpace.Clear();
#ifdef DEBUG
    cout << "Done!" << endl << flush;
#endif // DEBUG
    return vertexToReturn;
}

int PhasedLocalSearch::DegreeSelect(ArraySet const &vertexSet) const
{
    size_t maxDegree(0);
    m_ScratchSpace.Clear();
    for (int const vertex : vertexSet) {
        if (m_vAdjacencyArray[vertex].size() > maxDegree) {
            m_ScratchSpace.Clear();
            maxDegree = m_vAdjacencyArray[vertex].size();
            m_ScratchSpace.Insert(vertex);
        } else if (m_vAdjacencyArray[vertex].size() == maxDegree) {
            m_ScratchSpace.Insert(vertex);
        }
    }

    int const vertexToReturn = *(m_ScratchSpace.begin() + rand()%m_ScratchSpace.Size());
    m_ScratchSpace.Clear();
    return vertexToReturn;
}

// TODO/DS: select vertex from C_0(K)
int PhasedLocalSearch::SelectFromZero()
{
#ifdef DEBUG
    cout << "Selecting from C_0..." << endl << flush;
#endif // DEBUG

////    m_NotAdjacentToZero.SaveState();
////    m_NotAdjacentToZero.DiffInPlace(m_U);
////    assert(!m_NotAdjacentToZero.Empty());

    int const vertexToSelect(SelectFrom(m_NotAdjacentToZero));

////    m_NotAdjacentToZero.RestoreState();
    assert(vertexToSelect != -1);
    return vertexToSelect;
}

int PhasedLocalSearch::SelectFrom(ArraySet const &vertexSet) const
{
    switch (m_SelectionPhase) {
        // random vertex in set.
        case SelectionPhase::RANDOM_SELECTION:
            return RandomSelect(vertexSet);
        case SelectionPhase::DEGREE_SELECTION:
            return RandomSelect(vertexSet);
        case SelectionPhase::PENALTY_SELECTION:
            return PenaltySelect(vertexSet);
        default:
            assert(0);
            return -1;
    }
    assert(0);
    return -1;
}


// Not const because we perform difference.
// TODO/DS: optimize so that we don't perform the difference?
int PhasedLocalSearch::SelectFromOne()
{
#ifdef DEBUG
    cout << "Selecting from C_1 \\ U..." << endl << flush;
#endif // DEBUG

    m_NotAdjacentToOne.SaveState();
    m_NotAdjacentToOne.DiffInPlace(m_U);
    assert(!m_NotAdjacentToOne.Empty());

    int const vertexToReturn(SelectFrom(m_NotAdjacentToOne));

    m_NotAdjacentToOne.RestoreState();

    assert(vertexToReturn != -1);
    return vertexToReturn;
}


// Perform set minus and return true iff it is empty
bool PhasedLocalSearch::DiffIsEmpty(ArraySet const A, ArraySet const B) const
{
    if (B.Size() < A.Size()) return false;

    size_t uIntersectionCount(0);

    // TODO/DS: Can probably optimize by breaking out early.
    for (int const b : B) {
        if (A.Contains(b)) {
            uIntersectionCount++;
        }
    }

    return (uIntersectionCount == A.Size());
}

void PhasedLocalSearch::AddToIndependentSet(int const vertex)
{
#ifdef DEBUG
    cout << "Adding " << vertex << " to $K$" << endl << flush;
#endif // DEBUG
    if (m_IndependentSet.Contains(vertex)) return;

    m_IndependentSet.Insert(vertex);
#ifndef ALLOW_OVERLAP
    // Should they be mutually exclusive from $K$?
    m_NotAdjacentToOne.Remove(vertex);

    // definitely mutually exclusive, by definition
    m_NotAdjacentToZero.Remove(vertex);
#endif // ALLOW_OVERLAP

    // were already neighbors of $K$, now must be neighbors of vertex too
    vector<int> zeroDiffVertices;
    m_NotAdjacentToZero.IntersectInPlace(m_vAdjacencyArray[vertex], zeroDiffVertices);

    // if previously adjacent to all but one, and neighbor of newly added vertex
    // then still adjacent to all but one.
    // TODO/DS: Remove?
////    vector<int> oneDiffVertices;
////    m_NotAdjacentToOne.IntersectInPlace(m_vAdjacencyArray[vertex], oneDiffVertices);
    m_NotAdjacentToOne.IntersectInPlace(m_vAdjacencyArray[vertex]);
    // TODO/DS: check that C_0\U is empty.
    for (int const newVertex : zeroDiffVertices) {
////        if (newVertex == 18) {
////            cout << "Moving " << newVertex << " from C_0 to C_1" << endl << flush;
////        }
        m_NotAdjacentToOne.Insert(newVertex);
        m_bCheckOne = m_bCheckOne || !m_U.Contains(newVertex); // if u\in C_1 \ U
    }

////    cout << "Eject from C_1:";
////    for (int const vertex : oneDiffVertices) {
////        cout << " " << vertex;
////    }
////    cout << endl;
////
    m_IndependentSetWeight += m_vVertexWeights[vertex];

#ifdef CHECK_CONSISTENCY
    if (!IsConsistent()) {
        cout << "Line " << __LINE__ << ": Consistency check failed" << endl << flush;
    }
#endif // CHECK_CONSISTENCY
}

void PhasedLocalSearch::InitializeFromIndependentSet()
{
    //Empty items that dependent on independent set, so they can be initialized.
    m_IndependentSetWeight = 0;
    m_NotAdjacentToZero.Clear();
    m_NotAdjacentToOne.Clear();

    for (int const vertex : m_IndependentSet) {
        m_IndependentSetWeight += m_vVertexWeights[vertex];
    }

    m_bCheckZero = false;
    m_bCheckOne  = false;

    // check all-neighbors and all-but-one-neighbors
    for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {
        // C_0 and C_1 don't contain vertices from K
#ifndef ALLOW_OVERLAP
        if (m_IndependentSet.Contains(vertex)) continue;
#endif // ALLOW_OVERLAP
        size_t neighborCount(0);
        for (int const neighbor : m_vAdjacencyArray[vertex]) {
            if (m_IndependentSet.Contains(neighbor)) neighborCount++;
        }

        if (neighborCount == m_IndependentSet.Size()) {
            m_NotAdjacentToZero.Insert(vertex);
            m_bCheckZero = m_bCheckZero || !m_U.Contains(vertex);
        }

        if (neighborCount == m_IndependentSet.Size()-1) { 
            m_NotAdjacentToOne.Insert(vertex);
            m_bCheckOne = m_bCheckOne || !m_U.Contains(vertex);
        }
    }

#ifdef CHECK_CONSISTENCY
    if (!IsConsistent()) {
        cout << "Line " << __LINE__ << ": Consistency check failed" << endl << flush;
    }
#endif // CHECK_CONSISTENCY
}

void PhasedLocalSearch::InitializeFromIndependentSet2()
{
    assert(!m_IndependentSet.Empty());
    //Empty items that dependent on independent set, so they can be initialized.
    m_IndependentSetWeight = 0;
    m_ScratchSpace.Clear();
    m_NotAdjacentToZero.Clear();
    m_NotAdjacentToOne.Clear();

    m_bCheckZero = false;
    m_bCheckOne  = false;

    if (m_IndependentSet.Size() == 1) {
        int const vertexInIndependentSet(*m_IndependentSet.begin());
        for (int const neighbor : m_vAdjacencyArray[vertexInIndependentSet]) {
            m_NotAdjacentToZero.Insert(neighbor);
            m_bCheckZero = m_bCheckZero || !m_U.Contains(neighbor);
        }
        for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {
            if (m_NotAdjacentToZero.Contains(vertex)) continue;
            m_NotAdjacentToOne.Insert(vertex);
            m_bCheckOne = m_bCheckOne || !m_U.Contains(vertex);
        }
        m_NotAdjacentToOne.Remove(vertexInIndependentSet);

        return;
    }

    // update weights, follow neighbors, count them
    // insert into levels sets C_0 and C_1
    for (int const vertex : m_IndependentSet) {
        m_IndependentSetWeight += m_vVertexWeights[vertex];

        for (int const neighbor : m_vAdjacencyArray[vertex]) {
#ifndef ALLOW_OVERLAP
            if (m_IndependentSet.Contains(neighbor)) continue;
#endif // ALLOW_OVERLAP
            m_ScratchSpace.Insert(neighbor);
            m_vScratchCounters[neighbor]++; 
        }
    }

    for (int const vertex : m_ScratchSpace) {
        int const neighborCount(m_vScratchCounters[vertex]);
        if (neighborCount == m_IndependentSet.Size()) {
            m_NotAdjacentToZero.Insert(vertex);
            m_bCheckZero = m_bCheckZero || !m_U.Contains(vertex);
        } else if (neighborCount == m_IndependentSet.Size()-1) { 
            m_NotAdjacentToOne.Insert(vertex);
            m_bCheckOne = m_bCheckOne || !m_U.Contains(vertex);
        }

        m_vScratchCounters[vertex] = 0;
    }

    m_ScratchSpace.Clear();

#ifdef CHECK_CONSISTENCY
    if (!IsConsistent()) {
        cout << "Line " << __LINE__ << ": Consistency check failed" << endl << flush;
    }
#endif // CHECK_CONSISTENCY
}



bool PhasedLocalSearch::IsConsistent() const
{
////    cout << "Checking Consistency..." << endl << flush;
    bool bConsistent(true);
    // check weight
    double weight(0.0);
    for (int const vertex : m_IndependentSet) {
        weight += m_vVertexWeights[vertex];
        size_t neighborsInSet(0);
        for (int const neighbor : m_vAdjacencyArray[vertex]) {
            if (m_IndependentSet.Contains(neighbor)) {
                neighborsInSet++;
            }
        }

        if (neighborsInSet != m_IndependentSet.Size()-1) {
            cout << "Consistency Error!: vertex " << vertex << " has " << neighborsInSet << " neighbors in $K$, but should have " << m_IndependentSet.Size()-1 << endl << flush;
        }
    }
    if (weight != m_IndependentSetWeight) {
        cout << "Consistency Error!: weight incorrect -> should be " << weight << ", is " << m_IndependentSetWeight << endl << flush;
        bConsistent = false;
    }

    // check all-neighbors and all-but-one-neighbors
    for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {

        bool const bDebug(false);
////        bool const bDebug(vertex == 18);
        size_t neighborCount(0);
        for (int const neighbor : m_vAdjacencyArray[vertex]) {
            if (m_IndependentSet.Contains(neighbor)) neighborCount++;
        }

        if (bDebug) {
            cout << vertex << ":";
            for (int const neighbor : m_vAdjacencyArray[vertex]) {
                cout << neighbor << " ";
            }
            cout << endl;

            cout << 176 << ":";
            for (int const neighbor : m_vAdjacencyArray[176]) {
                cout << neighbor << " ";
            }
            cout << endl;

            cout << " vertex " << vertex << " has " << neighborCount << " neighbors in independent set, and independent set has " << m_IndependentSet.Size() << endl << flush;
        }


#ifndef ALLOW_OVERLAP
        if (m_IndependentSet.Contains(vertex) && m_NotAdjacentToZero.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is in K and C_0, but they are mutually exclusive (should only be in K?)" << endl << flush;
        }

        if (m_IndependentSet.Contains(vertex) && m_NotAdjacentToOne.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is in K and C_1, but they are mutually exclusive (should only be in K?)" << endl << flush;
        }

        bool dontRunCountCheck(m_IndependentSet.Contains(vertex));
        if (dontRunCountCheck) continue;
#endif // ALLOW_OVERLAP

        if (neighborCount != m_IndependentSet.Size() && m_NotAdjacentToZero.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is in C_0, but does not belong in C_0" << endl << flush;
            bConsistent = false;
        }

        if (neighborCount == m_IndependentSet.Size() && !m_NotAdjacentToZero.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is not in C_0, but belongs in C_0" << endl << flush;
            bConsistent = false;
        }

        if (neighborCount != m_IndependentSet.Size()-1 && m_NotAdjacentToOne.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is in C_1, but does not belong in C_1" << endl << flush;
            bConsistent = false;
        }

        if (neighborCount == m_IndependentSet.Size()-1 && !m_NotAdjacentToOne.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is not in C_1, but belongs in C_1" << endl << flush;
            bConsistent = false;
        }
    }

    return bConsistent;
}

void PhasedLocalSearch::UpdateStatistics()
{
    if (m_IndependentSetWeight > m_uBestWeight) {
        m_TimeToReachBestWeight = clock() - m_StartTime;
        m_uSelectionsToBestWeight = m_uSelections;
        m_uBestWeight = m_IndependentSetWeight;
        if (!m_bQuiet)
            cout << "(" << Tools::GetTimeInSeconds(m_TimeToReachBestWeight)<< ":" << m_uSelections << "): Best MWIS weight=" << m_uBestWeight << " has size   " << m_IndependentSet.Size() << endl << flush;
    }

    if (m_IndependentSet.Size() > m_uBestSize) {
        m_uBestSize = m_IndependentSet.Size();
        ////                    cout << "Best MWIS Size=" << m_uBestSize << endl << flush;
        if (!m_bQuiet)
            cout << "(" << Tools::GetTimeInSeconds(m_TimeToReachBestWeight)<< ":" << m_uSelections << "): Best WIS size   =" << m_uBestWeight << " has weight " << m_IndependentSetWeight << endl << flush;
    }
}


bool PhasedLocalSearch::Phase(size_t uIterations, SelectionPhase const selectionPhase)
{
    m_SelectionPhase = selectionPhase;
    //// TODO/DS: optimization opportunity: Can we maintain a flag that detects if C_1 \ U is empty?
    while (uIterations > 0 && m_uSelections < m_uMaxSelections) {
#ifdef DEBUG
        cout << "Outer Loop... Selections=" << m_uSelections << endl << flush;
#endif // DEBUG

        // TODO/DS: understand why $U\superseteq C_0$, which can cause no selections.
        bool bNoSelectionWasMade(false);
////        while ((!m_NotAdjacentToZero.Empty() || !DiffIsEmpty(m_NotAdjacentToOne, m_U)) && !bNoSelectionWasMade) {
////        while (((!m_NotAdjacentToZero.Empty() && m_bCheckZero) || (!m_NotAdjacentToOne.Empty() && m_bCheckOne)) && !bNoSelectionWasMade) {
        while ((!m_NotAdjacentToZero.Empty() || (!m_NotAdjacentToOne.Empty() && m_bCheckOne)) && !bNoSelectionWasMade) {
#ifdef DEBUG
            bool const bDiffNotEmpty2(!DiffIsEmpty(m_NotAdjacentToOne, m_U));
            bool const bNewDiffNotEmpty2(!m_NotAdjacentToOne.Empty() && m_bCheckOne);
            if (bNewDiffNotEmpty2 != bDiffNotEmpty2) {
                cout << "New check failed..." << endl << flush;
                cout << "    DiffNotEmpty2    =" << (bDiffNotEmpty2    ? "true" : "false") << endl << flush;
                cout << "    NewDiffNotEmpty2 =" << (bNewDiffNotEmpty2 ? "true" : "false") << endl << flush;
            }
#endif // DEBUG
            bNoSelectionWasMade = true;
#ifdef DEBUG
            cout << "Inner Loop... Selections=" << m_uSelections << endl << flush;
            cout << "C_0 \    is " << (m_NotAdjacentToZero.Empty()           ? "empty" : "not empty") << endl << flush;
            cout << "C_0 \\ U is " << (DiffIsEmpty(m_NotAdjacentToZero, m_U) ? "empty" : "not empty") << endl << flush;
            cout << "C_1 \\ U is " << (DiffIsEmpty(m_NotAdjacentToOne, m_U)  ? "empty" : "not empty") << endl << flush;
#endif // DEBUG

#ifdef DEBUG
            bool const bDiffNotEmpty(!DiffIsEmpty(m_NotAdjacentToZero, m_U));
            bool const bNewDiffNotEmpty(!m_NotAdjacentToZero.Empty() && m_bCheckZero);
            if (bNewDiffNotEmpty != bDiffNotEmpty) {
                cout << "New check failed..." << endl << flush;
                cout << "    DiffNotEmpty    =" << (bDiffNotEmpty ? "true" : "false") << endl << flush;
                cout << "    NewDiffNotEmpty =" << (bNewDiffNotEmpty ? "true" : "false") << endl << flush;
            }
#endif // DEBUG

            // select from C_0
////            while (!DiffIsEmpty(m_NotAdjacentToZero, m_U)) {
            while (!m_NotAdjacentToZero.Empty() && m_bCheckZero) {
                bNoSelectionWasMade = false;

                int const vertex(SelectFromZero());
#ifdef DEBUG
                cout << "Selected vertex " << vertex << " from C_0" << endl << flush;
#endif // DEBUG
                AddToIndependentSet(vertex);
                m_uSelections++;

                // done! independent set weight reached target
                m_U.Clear();
                m_bCheckZero = true;
                m_bCheckOne  = true;
            }

            UpdateStatistics();
            if (m_IndependentSetWeight == m_uTargetWeight) {
                return true;
            }

#ifdef DEBUG
            bool const bDiffNotEmpty3(!DiffIsEmpty(m_NotAdjacentToOne, m_U));
            bool const bNewDiffNotEmpty3(!m_NotAdjacentToOne.Empty() && m_bCheckOne);
            if (bNewDiffNotEmpty3 != bDiffNotEmpty3) {
                cout << "New check failed..." << endl << flush;
                cout << "    DiffNotEmpty3    =" << (bDiffNotEmpty3    ? "true" : "false") << endl << flush;
                cout << "    NewDiffNotEmpty3 =" << (bNewDiffNotEmpty3 ? "true" : "false") << endl << flush;
            }
#endif // DEBUG

            // select from C_1 \ U
////            if (!DiffIsEmpty(m_NotAdjacentToOne, m_U)) {
            if (!m_NotAdjacentToOne.Empty() && m_bCheckOne) {
                bNoSelectionWasMade = false;

                int const vertex = SelectFromOne();
#ifdef DEBUG
                cout << "Selected vertex " << vertex << " from C_1 \\ U" << endl << flush;
#endif // DEBUG

                // first restrict to neighborhood of $v$
                vector<int> diffSet;
                m_IndependentSet.IntersectInPlace(m_vAdjacencyArray[vertex], diffSet);

                for (int const diffVertex : diffSet) {
                    m_U.Insert(diffVertex);
                }

                // then add v and update helper sets.
                m_IndependentSet.Insert(vertex);
                InitializeFromIndependentSet2();

                m_uSelections++;
            }
        }
        uIterations--; // unused in algorithm?
        UpdatePenalties();
        Perturb();
////        m_U.Clear();
////        m_bCheckZero = true;
////        m_bCheckOne = true;
    }

    return false;
}

bool PhasedLocalSearch::Run()
{
    m_StartTime = clock();
    if (!m_bQuiet) {
        cout << "Executing algorithm " << GetName() << "..." << endl << flush;
        cout << "Graph has : " << m_vAdjacencyArray.size() << " vertices " << endl;
    }

    if (!m_bQuiet) {
        size_t const uSampleVertex(1);
        cout << "Vertex " << uSampleVertex << " has weight " << m_vVertexWeights[uSampleVertex] << endl;
    }

    // initialize independent set
    int const randomVertex(rand()%m_vAdjacencyArray.size());
    AddToIndependentSet(randomVertex);
    m_RandomIndependentSet.Insert(randomVertex);
    m_DegreeIndependentSet.Insert(randomVertex);
    m_bCheckZero = true;

    bool foundSolution(false);
    while (m_uSelections < m_uMaxSelections) {
        foundSolution = Phase(50,  SelectionPhase::RANDOM_SELECTION);
        m_RandomIndependentSet = m_IndependentSet;
        if (foundSolution) return true;
        if (clock() - m_StartTime > m_TimeOut) {
            return false;
        }

        // penalty phase begins where random phase left off
        foundSolution = Phase(50, SelectionPhase::PENALTY_SELECTION);
        if (foundSolution) return true;
        if (clock() - m_StartTime > m_TimeOut) {
            return false;
        }

        // degree phase starts where previous degree phase left off.
        m_IndependentSet = m_DegreeIndependentSet;
        InitializeFromIndependentSet2();

        foundSolution = Phase(100, SelectionPhase::DEGREE_SELECTION);
        if (foundSolution) return true;
        if (clock() - m_StartTime > m_TimeOut) {
            return false;
        }

        m_DegreeIndependentSet = m_IndependentSet;

        // random phase begins where random phase left off.
        m_IndependentSet = m_RandomIndependentSet;
        InitializeFromIndependentSet2();
    }

    return false;
}

void PhasedLocalSearch::SetTargetSize(size_t const uTargetSize)
{
    m_uTargetSize = uTargetSize;
}

void PhasedLocalSearch::SetMaxSelections(size_t const uMaxSelections)
{
    m_uMaxSelections = uMaxSelections;
}

void PhasedLocalSearch::SetTimeOutInMilliseconds(size_t const timeout)
{
    m_TimeOut = ((double)(timeout)/1000.0 * CLOCKS_PER_SEC);
}

void PhasedLocalSearch::SetTargetWeight(size_t const targetWeight)
{
    m_uTargetWeight = targetWeight;
}

double PhasedLocalSearch::GetTimeoutInSeconds() const
{
    return ((double)m_TimeOut)/CLOCKS_PER_SEC;
}

size_t PhasedLocalSearch::GetPenaltyDelay() const
{
    return m_uPenaltyDelay;
}
