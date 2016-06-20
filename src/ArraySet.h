#ifndef ARRAY_SET_H
#define ARRAY_SET_H

#include <set>
#include <vector>
#include <iostream>
#include <cassert>
#include <utility>

class ArraySet
{
public:
    ArraySet(size_t const size) : m_Lookup(size, -1), m_Elements(size, -1), m_iBegin(0), m_iEnd(-1), m_States(0), m_bInserted(false), m_bRemoved(false)
    {
    }

    ArraySet() : m_Lookup(), m_Elements(), m_iBegin(0), m_iEnd(-1), m_States(0), m_bInserted(false), m_bRemoved(false)
    {
    }

    ~ArraySet() {}

    void Resize(size_t const size)
    {
        m_Lookup.resize(size, -1);
        m_Elements.resize(size, -1);
    }

    void InitializeFromAdjacencyArray(std::vector<std::vector<int>> const &adjacencyArray, int const vertex)
    {
        Resize(adjacencyArray.size());
        for (int const neighbor : adjacencyArray[vertex]) {
            Insert(neighbor);
        }
    }

    void PrintSummary() const
    {
        std::cout << "Array[" << m_iBegin << ":" << m_iEnd << "] : ";
        for (int const p : *this) {
            std::cout << p << (Contains(p)? "":"<-inconsistency") << " ";
        }
        std::cout << std::endl;
    }

    bool Contains(int const x) const {
        if (x < 0 || x >= m_Lookup.size()) return false;
        int const locationX(m_Lookup[x]);
        return locationX >= m_iBegin && locationX <= m_iEnd;
    }

    // Inserts are not allowed after saving state, as it is currently not supported.
    void Insert(int const x) {
////        if (x==107) std::cout << "Inserting vertex " << x << " into graph." << std::endl << std::flush;
        if (Contains(x)) return;
        assert(!m_bRemoved); // not allowed to insert and remove when saving states
        if (!m_States.empty()) m_bInserted = true;
        if (m_Elements[m_iEnd+1] != -1 && !Contains(m_Elements[m_iEnd+1])) // if not a reinserted element, overwrite its lookup.
            m_Lookup[m_Elements[m_iEnd+1]] = -1; 
        m_iEnd++;
        m_Lookup[x] = m_iEnd;
        m_Elements[m_iEnd] = x;
    }

    void Remove(int const x) {
////        if (x==107) std::cout << "Removing vertex " << x << " from graph." << std::endl << std::flush;
        if (!Contains(x)) return;
        assert(!m_bInserted); // not allowed to insert and remove when saving states
        if (!m_States.empty()) m_bRemoved = true;
        int const locationX(m_Lookup[x]);
        m_Elements[locationX] = m_Elements[m_iEnd];
        m_Lookup[m_Elements[locationX]] = locationX;
        m_Lookup[x] = m_iEnd;
        m_Elements[m_iEnd] = x;
        m_iEnd--;
    }

    void MoveTo(int const x, ArraySet &other) {
        if (!Contains(x)) return;
        Remove(x);
        other.Insert(x);
    }

    void CopyTo(int const x, ArraySet &other) {
        if (!Contains(x)) return;
        other.Insert(x);
    }

    void IntersectInPlace(std::vector<int> const &vOther, std::vector<int> &vRemaining) {
        int iPutValueHere(m_iBegin);
        for (int const valueOther : vOther) {
            if (Contains(valueOther)) {
                int const locationOther(m_Lookup[valueOther]);
                int const valueExisting(m_Elements[iPutValueHere]);

                m_Lookup[valueExisting] = locationOther;
                m_Elements[locationOther] = valueExisting;

                m_Lookup[valueOther] = iPutValueHere;
                m_Elements[iPutValueHere] = valueOther;
                iPutValueHere++;
            }
        }

        vRemaining.insert(vRemaining.end(), m_Elements.begin() + iPutValueHere, end());

        m_iEnd = iPutValueHere-1;
    }

    void IntersectInPlace(std::vector<int> const &vOther) {
        int iPutValueHere(m_iBegin);
        for (int const valueOther : vOther) {
            if (Contains(valueOther)) {
                int const locationOther(m_Lookup[valueOther]);
                int const valueExisting(m_Elements[iPutValueHere]);

                m_Lookup[valueExisting] = locationOther;
                m_Elements[locationOther] = valueExisting;

                m_Lookup[valueOther] = iPutValueHere;
                m_Elements[iPutValueHere] = valueOther;
                iPutValueHere++;
            }
        }

        m_iEnd = iPutValueHere-1;
    }

    void DiffInPlace(ArraySet const &other) {
        // if we have the same value as the other set, then we remove it.
        for (int const valueOther : other) {
            if (Contains(valueOther)) Remove(valueOther);
        }
    }

    void DiffInPlace(std::vector<int> const &other) {
        // if we have the same value as the other set, then we remove it.
        for (int const valueOther : other) {
            if (Contains(valueOther)) Remove(valueOther);
        }
    }


    void DiffInPlace(std::vector<int> const &other, std::vector<int> &intersect) {
        // if we have the same value as the other set, then we remove it.
        for (int const valueOther : other) {
            if (Contains(valueOther)) {
                Remove(valueOther);
                intersect.push_back(valueOther);
            }
        }
    }


////    void ForAllInDiff()
////    {
////    }

    size_t Size()  const { return m_iEnd - m_iBegin + 1; }
    bool   Empty() const { return m_iEnd < m_iBegin; }

    // slow because end is called a lot?
    std::vector<int>::iterator begin() { return m_Elements.begin() + m_iBegin;   }
    std::vector<int>::iterator end()   { return m_Elements.begin() + m_iEnd + 1; }

    std::vector<int>::const_iterator begin() const { return m_Elements.begin() + m_iBegin;   }
    std::vector<int>::const_iterator end()   const { return m_Elements.begin() + m_iEnd + 1; }

    int At(size_t const index) const
    {
        return m_Elements[index];
    }

    int operator[](size_t const index) const { return At(index); }

    void SaveState()
    {
////        std::cout << "Saving State" << std::endl << std::flush;
        m_States.push_back(std::make_pair(m_iBegin, m_iEnd));
    }

    void RestoreState()
    {
////        std::cout << "Restoring State" << std::endl << std::flush;
        std::pair<int,int> const &range(m_States.back());
        m_iBegin = range.first;
        m_iEnd   = range.second;
        m_States.pop_back();
////        std::cout << "#States = " << m_States.size() << std::endl << std::flush;
        if (m_States.empty()) {
            m_bRemoved  = false;
            m_bInserted = false;
        }
    }

    void Clear()
    {
        m_iBegin = 0;
        m_iEnd = -1;
    }

    bool operator==(ArraySet const &that) const
    {
        if (Size() != that.Size()) return false;
        for (int const value : *this) {
            if (!that.Contains(value)) return false;
        }
        return true;
    }

    bool operator!=(ArraySet const &that) const
    {
        return !(*this == that);
    }

    static bool Test() {
        std::cout << "ArraySet: ";
        ArraySet testSet(3);
        testSet.Insert(0);
        testSet.Insert(1);
        testSet.Insert(2);

        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ArraySet failed ContainsTest" << std::endl;
            return false;
        }

        testSet.Remove(0);
        if (testSet.Contains(0) || testSet.Size() != 2) {
            std::cout << "FAILED: ArraySet failed RemoveFirst Test" << std::endl;
            return false;
        }

        testSet.Insert(0);
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ArraySet failed RemoveAdd Test" << std::endl;
            return false;
        }

        testSet.Remove(0);
        if (testSet.Contains(0) || testSet.Size() != 2) {
            std::cout << "FAILED: ArraySet failed RemoveLast Test" << std::endl;
            return false;
        }

        testSet.Insert(0);
        testSet.Remove(1);
        if (testSet.Contains(1) || testSet.Size() != 2) {
            std::cout << "FAILED: ArraySet failed RemoveMiddle Test" << std::endl;
            return false;
        }

        testSet.Insert(1);
        testSet.Remove(1);
        testSet.Remove(2);
        testSet.Remove(0);
        if (testSet.Contains(0) || testSet.Contains(1) || testSet.Contains(2) || testSet.Size() != 0 || !testSet.Empty()) {
            std::cout << "FAILED: ArraySet failed RemoveAll Test" << std::endl;
            return false;
        }

        testSet.Insert(0);
        testSet.Insert(1);
        testSet.Insert(2);
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ArraySet failed InsertAll Test" << std::endl;
            return false;
        }

        testSet.SaveState();
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ArraySet failed SaveState Test" << std::endl;
            return false;
        }

        testSet.Remove(0);
        if (testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 2) {
            std::cout << "FAILED: ArraySet failed RemoveAfterSave Test" << std::endl;
            return false;
        }

        testSet.RestoreState();
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ArraySet failed RestoreState Test" << std::endl;
            return false;
        }

        std::cout << "PASSED!" << std::endl << std::flush;
        return true;
    }

private:
    std::vector<int> m_Lookup;
    std::vector<int> m_Elements;
    int m_iBegin;
    int m_iEnd;
    std::vector<std::pair<int,int>> m_States;
    bool m_bInserted;
    bool m_bRemoved;
};

#endif // ARRAY_SET_H
