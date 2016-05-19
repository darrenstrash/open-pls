#ifndef PLS_H 
#define PLS_H

#include "Algorithm.h"
#include <string>
#include <vector>

class PhasedLocalSearch : public Algorithm
{
public:
    PhasedLocalSearch(std::vector<std::vector<int>> const &vAdjacencyArray, std::vector<double> const &vVertexWeights);

    virtual bool Run();
    std::string GetName() const { return m_sName; }

protected:
    std::string m_sName;
    std::vector<std::vector<int>> const &m_vAdjacencyArray;
    std::vector<double>           const &m_vVertexWeights;
};

#endif //PLS_H
