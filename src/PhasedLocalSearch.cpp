#include "PhasedLocalSearch.h"

// system includes
#include <string>
#include <vector>
#include <iostream>

using namespace std;

PhasedLocalSearch::PhasedLocalSearch(vector<vector<int>> const &vAdjacencyArray, vector<double> const &vVertexWeights)
: Algorithm("PLS")
, m_vAdjacencyArray(vAdjacencyArray)
, m_vVertexWeights(vVertexWeights)
{
}

bool PhasedLocalSearch::Run()
{
    cout << "Executing algorithm " << GetName() << "..." << endl << flush;
    cout << "Graph has : " << m_vAdjacencyArray.size() << " vertices " << endl;

    size_t uSampleVertex(1);
    cout << "Vertex " << uSampleVertex << " has weight " << m_vVertexWeights[uSampleVertex] << endl;
    return false;
}
