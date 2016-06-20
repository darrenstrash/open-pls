
#include "IndependentSetPhasedLocalSearch.h"

using namespace std;

IndependentSetPhasedLocalSearch::IndependentSetPhasedLocalSearch(vector<vector<int>> const &vAdjacencyArray, vector<double> const &vVertexWeights)
: PhasedLocalSearch(vAdjacencyArray,vVertexWeights)
{
    SetName("pls-independent-set");
}

void IndependentSetPhasedLocalSearch::AddToK(int const vertex)
{
#ifdef DEBUG
    cout << "Adding " << vertex << " to $K$" << endl << flush;
#endif // DEBUG
    if (m_K.Contains(vertex)) return;

    m_K.Insert(vertex);
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
    m_KWeight += m_vVertexWeights[vertex];

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
    m_KWeight = 0;
    m_NotAdjacentToZero.Clear();
    m_NotAdjacentToOne.Clear();

    for (int const vertex : m_K) {
        m_KWeight += m_vVertexWeights[vertex];
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
        }

        if (neighborCount == m_K.Size()) {
            m_NotAdjacentToZero.Insert(vertex);
            m_bCheckZero = m_bCheckZero || !m_U.Contains(vertex);
        }

        if (neighborCount == m_K.Size()-1) { 
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
    assert(!m_K.Empty());
    //Empty items that dependent on independent set, so they can be initialized.
    m_KWeight = 0;
    m_ScratchSpace.Clear();
    m_NotAdjacentToZero.Clear();
    m_NotAdjacentToOne.Clear();

    m_bCheckZero = false;
    m_bCheckOne  = false;

    if (m_K.Size() == 1) {
        int const vertexInK(*m_K.begin());
        m_KWeight = m_vVertexWeights[vertexInK];
        for (int const neighbor : m_vAdjacencyArray[vertexInK]) {
            m_NotAdjacentToZero.Insert(neighbor);
            m_bCheckZero = m_bCheckZero || !m_U.Contains(neighbor);
        }
        for (int vertex = 0; vertex < m_vAdjacencyArray.size(); ++vertex) {
            if (m_NotAdjacentToZero.Contains(vertex)) continue;
            m_NotAdjacentToOne.Insert(vertex);
            m_bCheckOne = m_bCheckOne || !m_U.Contains(vertex);
        }
        m_NotAdjacentToOne.Remove(vertexInK);

        return;
    }

    // update weights, follow neighbors, count them
    // insert into levels sets C_0 and C_1
    for (int const vertex : m_K) {
        m_KWeight += m_vVertexWeights[vertex];

        for (int const neighbor : m_vAdjacencyArray[vertex]) {
#ifndef ALLOW_OVERLAP
            if (m_K.Contains(neighbor)) continue;
#endif // ALLOW_OVERLAP
            m_ScratchSpace.Insert(neighbor);
            m_vScratchCounters[neighbor]++; 
        }
    }

    for (int const vertex : m_ScratchSpace) {
        int const neighborCount(m_vScratchCounters[vertex]);
        if (neighborCount == m_K.Size()) {
            m_NotAdjacentToZero.Insert(vertex);
            m_bCheckZero = m_bCheckZero || !m_U.Contains(vertex);
        } else if (neighborCount == m_K.Size()-1) { 
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

