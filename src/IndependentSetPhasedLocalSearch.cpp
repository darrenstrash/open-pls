
#include "IndependentSetPhasedLocalSearch.h"

#include <climits>

////#define ALLOW_OVERLAP
////#define CHECK_CONSISTENCY 
////#define DEBUG

using namespace std;

IndependentSetPhasedLocalSearch::IndependentSetPhasedLocalSearch(vector<vector<int>> const &vAdjacencyArray, vector<double> const &vVertexWeights)
: PhasedLocalSearch(vAdjacencyArray,vVertexWeights)
{
    SetName("pls-independent-set");
}

int IndependentSetPhasedLocalSearch::DegreeSelect(ResetableArraySet const &vertexSet) const
{
    size_t minDegree(ULONG_MAX);
    m_ScratchSpace.Clear();
    for (int const vertex : vertexSet) {
        if (m_vAdjacencyArray[vertex].size() < minDegree) {
            m_ScratchSpace.Clear();
            minDegree = m_vAdjacencyArray[vertex].size();
            m_ScratchSpace.Insert(vertex);
        } else if (m_vAdjacencyArray[vertex].size() == minDegree) {
            m_ScratchSpace.Insert(vertex);
        }
    }

    int const vertexToReturn = *(m_ScratchSpace.begin() + rand()%m_ScratchSpace.Size());
    m_ScratchSpace.Clear();
    return vertexToReturn;
}


void IndependentSetPhasedLocalSearch::AddToK(int const vertex)
{
#ifdef DEBUG
    cout << "Adding " << vertex << " to $K$" << endl << flush;
#endif // DEBUG

#ifdef CHECK_CONSISTENCY
    if (!IsConsistent()) {
        cout << "Line " << __LINE__ << ": Consistency check failed" << endl << flush;
    }
#endif //CHECK_CONSISTENCY

    if (m_K.Contains(vertex)) return;

    m_K.Insert(vertex);
#ifndef ALLOW_OVERLAP
    // Should they be mutually exclusive from $K$?
    m_NotAdjacentToOne.Remove(vertex);

    // definitely mutually exclusive, by definition
    m_NotAdjacentToZero.Remove(vertex);
#endif // ALLOW_OVERLAP

    // neighbors of new vertex that were in C_0 are now in C_1.
    // non-neighbors remain in C_0
    vector<int> zeroIntersectVertices;
    m_NotAdjacentToZero.DiffInPlace(m_vAdjacencyArray[vertex], zeroIntersectVertices);

    // if previously to one, and neighbor of newly added vertex
    // then now adjacent to two...remove
    // i.e., keep non-neighbors of vertex
    m_NotAdjacentToOne.DiffInPlace(m_vAdjacencyArray[vertex]);
    for (int const newVertex : zeroIntersectVertices) {
        m_NotAdjacentToOne.Insert(newVertex);
        m_bCheckOne = m_bCheckOne || !m_U.Contains(newVertex); // if u\in C_1 \ U
    }

////    cout << "Eject from C_1:";
////    for (int const vertex : oneDiffVertices) {
////        cout << " " << vertex;
////    }
////    cout << endl;
////
    m_dKWeight += m_vVertexWeights[vertex];

#ifdef CHECK_CONSISTENCY
    if (!IsConsistent()) {
        cout << "Line " << __LINE__ << ": Consistency check failed" << endl << flush;
    }
#endif // CHECK_CONSISTENCY
}


// starting from IndependentSet, initialize level sets and flags
void IndependentSetPhasedLocalSearch::InitializeFromK()
{
    //Empty items that dependent on independent set, so they can be initialized.
    m_dKWeight = 0;
    m_NotAdjacentToZero.Clear();
    m_NotAdjacentToOne.Clear();

    for (int const vertex : m_K) {
        m_dKWeight += m_vVertexWeights[vertex];
    }

    m_bCheckZero = false;
    m_bCheckOne  = false;

    // check all-neighbors and all-but-one-neighbors
    for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {
        // C_0 and C_1 don't contain vertices from K
#ifndef ALLOW_OVERLAP
        if (m_K.Contains(vertex)) continue;
#endif // ALLOW_OVERLAP
        size_t neighborCount(0);
        for (int const neighbor : m_vAdjacencyArray[vertex]) {
            if (m_K.Contains(neighbor)) neighborCount++;
            if (neighborCount > 1) break;
        }

        if (neighborCount == 0) {
            m_NotAdjacentToZero.Insert(vertex);
            m_bCheckZero = m_bCheckZero || !m_U.Contains(vertex);
        }

        if (neighborCount == 1) { 
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

// same as InitializeFromK, but more efficient, iterates over $K$ instead
// of over all vertices.
void IndependentSetPhasedLocalSearch::InitializeFromK2()
{
    if (m_K.Size() != 1) {
        InitializeFromK();
        return;
    }

    m_dKWeight = 0;
    m_ScratchSpace.Clear();
    m_NotAdjacentToZero.Clear();
    m_NotAdjacentToOne.Clear();

    m_bCheckZero = false;
    m_bCheckOne  = false;

    int const vertexInK(*m_K.begin());
    m_dKWeight = m_vVertexWeights[vertexInK];
    // all neighbors of vertex are in C_1
    for (int const neighbor : m_vAdjacencyArray[vertexInK]) {
        m_NotAdjacentToOne.Insert(neighbor);
        m_bCheckOne = m_bCheckOne || !m_U.Contains(neighbor);
    }

    // all others are in C_0
    for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {
        if (m_NotAdjacentToOne.Contains(vertex)) continue;
        if (vertexInK==vertex) continue;
        m_NotAdjacentToZero.Insert(vertex);
        m_bCheckZero = m_bCheckZero || !m_U.Contains(vertex);
    }
}
////    assert(!m_K.Empty());
////    //Empty items that dependent on independent set, so they can be initialized.
////    m_dKWeight = 0;
////    m_ScratchSpace.Clear();
////    m_NotAdjacentToZero.Clear();
////    m_NotAdjacentToOne.Clear();
////
////    m_bCheckZero = false;
////    m_bCheckOne  = false;
////
////    if (m_K.Size() == 1) {
////        int const vertexInK(*m_K.begin());
////        m_dKWeight = m_vVertexWeights[vertexInK];
////        // all neighbors of vertex are in C_1
////        for (int const neighbor : m_vAdjacencyArray[vertexInK]) {
////            m_NotAdjacentToOne.Insert(neighbor);
////            m_bCheckOne = m_bCheckOne || !m_U.Contains(neighbor);
////        }
////
////        // all others are in C_0
////        for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {
////            if (m_NotAdjacentToOne.Contains(vertex)) continue;
////            if (vertexinK==vertex) continue;
////            m_NotAdjacentToZero.Insert(vertex);
////            m_bCheckZero = m_bCheckZero || !m_U.Contains(vertex);
////        }
////
////        return;
////    }
////
////    // update weights, follow neighbors, count them
////    // insert into levels sets C_0 and C_1
////    for (int const vertex : m_K) {
////        m_dKWeight += m_vVertexWeights[vertex];
////
////        for (int const neighbor : m_vAdjacencyArray[vertex]) {
////#ifndef ALLOW_OVERLAP
////            if (m_K.Contains(neighbor)) continue;
////#endif // ALLOW_OVERLAP
////            m_ScratchSpace.Insert(neighbor);
////            m_vScratchCounters[neighbor]++; 
////        }
////    }
////
////    for (int const vertex : m_ScratchSpace) {
////        int const neighborCount(m_vScratchCounters[vertex]);
////        if (neighborCount == 1) {
////            m_NotAdjacentToOne.Insert(vertex);
////            m_bCheckOne = m_bCheckOne || !m_U.Contains(vertex);
////        } 
////////        else if (neighborCount == m_K.Size()-1) { 
////////            m_NotAdjacentToOne.Insert(vertex);
////////            m_bCheckOne = m_bCheckOne || !m_U.Contains(vertex);
////////        }
////
////        m_vScratchCounters[vertex] = 0;
////    }
////
////    // TODO/DS: Still need to fill in C_0. ...basically all other vertices than m_ScratchSpace
////
////    m_ScratchSpace.Clear();
////
////#ifdef CHECK_CONSISTENCY
////    if (!IsConsistent()) {
////        cout << "Line " << __LINE__ << ": Consistency check failed" << endl << flush;
////    }
////#endif // CHECK_CONSISTENCY
////}

bool IndependentSetPhasedLocalSearch::IsConsistent() const
{
////    cout << "Checking Consistency..." << endl << flush;
    bool bConsistent(true);
    // check weight
    double weight(0.0);
    for (int const vertex : m_K) {
        weight += m_vVertexWeights[vertex];
        size_t neighborsInSet(0);
        for (int const neighbor : m_vAdjacencyArray[vertex]) {
            if (m_K.Contains(neighbor)) {
                neighborsInSet++;
            }
        }

        if (neighborsInSet != 0) {
            cout << "Consistency Error!: vertex " << vertex << " has " << neighborsInSet << " neighbors in $K$, but should have " << 0 << endl << flush;
        }
    }

    if (weight != m_dKWeight) {
        cout << "Consistency Error!: weight incorrect -> should be " << weight << ", is " << m_dKWeight << endl << flush;
        bConsistent = false;
    }

    // check all-neighbors and all-but-one-neighbors
    for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {

        bool const bDebug(false);
////        bool const bDebug(vertex == 18);
        size_t neighborCount(0);
        for (int const neighbor : m_vAdjacencyArray[vertex]) {
            if (m_K.Contains(neighbor)) neighborCount++;
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

            cout << " vertex " << vertex << " has " << neighborCount << " neighbors in independent set, and independent set has " << m_K.Size() << endl << flush;
        }


#ifndef ALLOW_OVERLAP
        if (m_K.Contains(vertex) && m_NotAdjacentToZero.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is in K and C_0, but they are mutually exclusive (should only be in K?)" << endl << flush;
        }

        if (m_K.Contains(vertex) && m_NotAdjacentToOne.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is in K and C_1, but they are mutually exclusive (should only be in K?)" << endl << flush;
        }

        bool dontRunCountCheck(m_K.Contains(vertex));
        if (dontRunCountCheck) continue;
#endif // ALLOW_OVERLAP

        if (neighborCount != 0 && m_NotAdjacentToZero.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is in C_0, but does not belong in C_0" << endl << flush;
            bConsistent = false;
        }

        if (neighborCount == 0 && !m_NotAdjacentToZero.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is not in C_0, but belongs in C_0" << endl << flush;
            bConsistent = false;
        }

        if (neighborCount != 1 && m_NotAdjacentToOne.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is in C_1, but does not belong in C_1" << endl << flush;
            bConsistent = false;
        }

        if (neighborCount == 1 && !m_NotAdjacentToOne.Contains(vertex)) {
            cout << "Consistency Error!: vertex " << vertex << " is not in C_1, but belongs in C_1" << endl << flush;
            bConsistent = false;
        }
    }

    return bConsistent;
}

void IndependentSetPhasedLocalSearch::ForceIntoK(int const vertex, bool const updateU)
{
////    AddToKFromOne(vertex);

////    size_t neighborsInK(0);
////    for (int const neighbor : m_vAdjacencyArray[vertex]) {
////        if (m_K.Contains(neighbor)) {
////            neighborsInK++;
////        }
////    }
    // first restrict to neighborhood of $v$
    vector<int> intersectSet;
    m_K.DiffInPlace(m_vAdjacencyArray[vertex], intersectSet);

////    if (neighborsInK != m_K.Size()) {
////        cout << "Mismatch in independent set." << endl << flush;
////    }
////    if (diffSet.size() != 1) {
////        cout << "ERROR!: diff set should be one..." << endl << flush;
////    }

    if (updateU) {
        for (int const intersectVertex : intersectSet) {
            m_U.Insert(intersectVertex);
        }
    }

    // then add v and update helper sets.
    m_K.Insert(vertex);
    InitializeFromK2();
}

