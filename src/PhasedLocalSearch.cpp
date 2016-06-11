#include "PhasedLocalSearch.h"

// system includes
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

using namespace std;

PhasedLocalSearch::PhasedLocalSearch(vector<vector<int>> const &vAdjacencyArray, vector<double> const &vVertexWeights)
: Algorithm("PLS")
, m_vAdjacencyArray(vAdjacencyArray)
, m_vVertexWeights(vVertexWeights)
, m_uTargetSize(vAdjacencyArray.size())
, m_vVertexPenalties()
, m_IndependentSet(vAdjacencyArray.size())
, m_U(vAdjacencyArray.size())
, m_NotAdjacentToOne(vAdjacencyArray.size())
, m_NotAdjacentToZero(vAdjacencyArray.size())
, m_IndependentSetWeight(0.0)
, m_SelectionPhase(SelectionPhase::RANDOM_SELECTION)
, m_ScratchSpace(vAdjacencyArray.size())
{
    for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {
        m_NotAdjacentToZero.Insert(vertex);
    }
}

void PhasedLocalSearch::Perturb()
{
cout << "Perturb..." << endl << flush;
}

void PhasedLocalSearch::UpdatePenalties()
{
}

int PhasedLocalSearch::SelectRandom() const
{
    return 0;
}

int PhasedLocalSearch::SelectPenalty() const
{
    return 0;
}

int PhasedLocalSearch::SelectDegree() const
{
    return 0;
}

// TODO/DS: select vertex from C_0(K)
int PhasedLocalSearch::SelectFromZero() const
{
    cout << "Selecting from C_0..." << endl << flush;
    assert(!m_NotAdjacentToZero.Empty());
    switch (m_SelectionPhase) {
        // random vertex in set.
        case SelectionPhase::RANDOM_SELECTION:
            {
            return *(m_NotAdjacentToZero.begin() + rand()%m_NotAdjacentToZero.Size());
            }
        case SelectionPhase::DEGREE_SELECTION:
            {
            size_t maxDegree(0);
            size_t maxDegreeCount(0);
            m_ScratchSpace.Clear();
            for (int const vertex : m_NotAdjacentToZero) {
                if (m_vAdjacencyArray[vertex].size() > maxDegree) {
                    m_ScratchSpace.Clear();
                    maxDegree = m_vAdjacencyArray[vertex].size();
                    m_ScratchSpace.Insert(vertex);
                } else if (m_vAdjacencyArray[vertex].size() == maxDegree) {
                    maxDegreeCount++;
                    m_ScratchSpace.Insert(vertex);
                }
            }

            int const vertexToReturn = *(m_ScratchSpace.begin() + rand()%m_ScratchSpace.Size());
            m_ScratchSpace.Clear();
            return vertexToReturn;
            }

        case SelectionPhase::PENALTY_SELECTION:
            {
            assert(0);
            return -1;
            }
    };
    assert(0);
    return -1;
}

// TODO/DS: select vertex from C_1(K)\U
int PhasedLocalSearch::SelectFromOne() const
{
    return 0;
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
    cout << "Adding " << vertex << " to $K$" << endl << flush;
    if (m_IndependentSet.Contains(vertex)) return;

    m_IndependentSet.Insert(vertex);

    // were already neighbors of $K$, now must be neighbors of vertex too
    vector<int> zeroDiffVertices;
    m_NotAdjacentToZero.IntersectInPlace(m_vAdjacencyArray[vertex], zeroDiffVertices);

    // if previously adjacent to all but one, and neighbor of newly added vertex
    // then still adjacent to all but one.
    vector<int> oneDiffVertices;
    m_NotAdjacentToOne.IntersectInPlace(m_vAdjacencyArray[vertex], oneDiffVertices);
    for (int const newVertex : zeroDiffVertices) {
////        if (newVertex == 18) {
////            cout << "Moving " << newVertex << " from C_0 to C_1" << endl << flush;
////        }
        m_NotAdjacentToOne.Insert(newVertex);
    }

////    cout << "Eject from C_1:";
////    for (int const vertex : oneDiffVertices) {
////        cout << " " << vertex;
////    }
////    cout << endl;
////
    m_IndependentSetWeight += m_vVertexWeights[vertex];

    if (!IsConsistent()) {
        cout << "Line " << __LINE__ << ": Consistency check failed" << endl << flush;
    }
}

bool PhasedLocalSearch::IsConsistent() const
{
    cout << "Checking Consistency..." << endl << flush;
    bool bConsistent(true);
    // check weight
    double weight(0.0);
    for (int const vertex : m_IndependentSet) {
        weight += m_vVertexWeights[vertex];
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

bool PhasedLocalSearch::Run()
{
    cout << "Executing algorithm " << GetName() << "..." << endl << flush;
    cout << "Graph has : " << m_vAdjacencyArray.size() << " vertices " << endl;

    size_t uSampleVertex(1);
    cout << "Vertex " << uSampleVertex << " has weight " << m_vVertexWeights[uSampleVertex] << endl;

    size_t uSelections(0);
    size_t pu = 0;
    size_t pd = 2;
    size_t sa = 0; //= random;
    size_t uIterations = 50;

    // initial weight, TODO/DS: change.
    size_t uTargetSize(ULONG_MAX);

    set<int> U;

    // initialize random number generator with seed;
    int const seed(0);
    srand(seed);

    // initialize independent set
    int const randomVertex(rand()%m_vAdjacencyArray.size());
    AddToIndependentSet(randomVertex);

    m_vVertexPenalties.clear();
    m_vVertexPenalties.resize(m_vAdjacencyArray.size(), 0.0);

    while (uSelections < m_uMaxSelections) {
        while (!m_NotAdjacentToZero.Empty() || !DiffIsEmpty(m_NotAdjacentToOne, m_U)) {
            // select
            while (!DiffIsEmpty(m_NotAdjacentToZero, m_U)) {
                int const vertex = SelectFromZero();
                cout << "Selected vertex " << vertex << " from C_0" << endl << flush;
                AddToIndependentSet(vertex);
                uSelections++;

                // done! independent set weight reached target size
                if (m_IndependentSetWeight == uTargetSize) return true;
                m_U.Clear();
            }

/*
            TODO/DS: put back
            if (!DiffIsEmpty(m_NotAdjacentToOne, m_U)) {

                // TODO: different select from above, select from C_1(K)\U
                int const vertex = SelectFromOne();

                // first remove non-neighbors
                // TODO/DS: Remove K \ N(v) rather than N(v)? Bad pseudocode.
                for (int const neighbor : m_vAdjacencyArray[vertex]) {
                    m_IndependentSet.Remove(neighbor);
                    m_IndependentSetWeight -= m_vVertexWeights[neighbor];
                    m_U.Insert(neighbor);
                }

                // then add vertex.
                AddToIndependentSet(vertex);
                uSelections++;
            }
*/
        }
        uIterations--; // unused in algorithm?
        UpdatePenalties();
        Perturb();
    }

    return true;
}

void PhasedLocalSearch::SetTargetSize(size_t const uTargetSize)
{
    m_uTargetSize = uTargetSize;
}

void PhasedLocalSearch::SetMaxSelections(size_t const uMaxSelections)
{
    m_uMaxSelections = uMaxSelections;
}
