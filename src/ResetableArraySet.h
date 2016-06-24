#ifndef RESETABLE_ARRAY_SET_H
#define RESETABLE_ARRAY_SET_H

#include <set>
#include <vector>
#include <iostream>
#include <cassert>
#include <utility>

#define USE_RESETABLE

#ifdef USE_RESETABLE
class ResetableArraySet
{
public:
    ResetableArraySet(size_t const size) : m_Lookup(size, -1), m_Elements(size, -1), m_iBegin(0), m_iEnd(-1), m_States(0), m_bInserted(false), m_bRemoved(false)
    {
        for (size_t i = 0; i < size; ++i) {
            m_Lookup[i] = i;
            m_Elements[i] = i;
        }
    }

    ResetableArraySet() : m_Lookup(), m_Elements(), m_iBegin(0), m_iEnd(-1), m_States(0), m_bInserted(false), m_bRemoved(false)
    {
    }

    ~ResetableArraySet() {}

    void Resize(size_t const size)
    {
        m_Lookup.resize(size, -1);
        m_Elements.resize(size, -1);
    }

    void Reset() {
        m_iBegin = 0;
        m_iEnd = static_cast<int>(m_Elements.size())-1;
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

    bool SwapElements(int const x, int const y) {
        size_t posX(m_Lookup[x]);
        size_t posY(m_Lookup[y]);
        m_Lookup[x] = posY;
        m_Lookup[y] = posX;
        m_Elements[posY] = x;
        m_Elements[posX] = y;
    }

    // Inserts are not allowed after saving state, as it is currently not supported.
    void Insert(int const x) {
////        if (x==107) std::cout << "Inserting vertex " << x << " into graph." << std::endl << std::flush;
        if (Contains(x)) return;
        assert(!m_bRemoved); // not allowed to insert and remove when saving states
        if (!m_States.empty()) m_bInserted = true;
        SwapElements(x, m_Elements[++m_iEnd]);
////        m_iEnd++;
////        m_Lookup[x] = m_iEnd;
////        m_Elements[m_iEnd] = x;
    }

    void Remove(int const x) {
////        if (x==107) std::cout << "Removing vertex " << x << " from graph." << std::endl << std::flush;
        if (!Contains(x)) return;
        assert(!m_bInserted); // not allowed to insert and remove when saving states
        if (!m_States.empty()) m_bRemoved = true;
        SwapElements(x, m_Elements[m_iEnd--]);
////        int const locationX(m_Lookup[x]);
////        m_Elements[locationX] = m_Elements[m_iEnd];
////        m_Lookup[m_Elements[locationX]] = locationX;
////        m_Lookup[x] = m_iEnd;
////        m_Elements[m_iEnd] = x;
////        m_iEnd--;
    }

    void MoveTo(int const x, ResetableArraySet &other) {
        if (!Contains(x)) return;
        Remove(x);
        other.Insert(x);
    }

    void CopyTo(int const x, ResetableArraySet &other) {
        if (!Contains(x)) return;
        other.Insert(x);
    }

    void IntersectInPlace(std::vector<int> const &vOther, std::vector<int> &vRemaining) {
        int iPutValueHere(m_iBegin);
        for (int const valueOther : vOther) {
            if (Contains(valueOther)) {
                SwapElements(valueOther, m_Elements[iPutValueHere++]);
////                int const locationOther(m_Lookup[valueOther]);
////                int const valueExisting(m_Elements[iPutValueHere]);
////
////                m_Lookup[valueExisting] = locationOther;
////                m_Elements[locationOther] = valueExisting;
////
////                m_Lookup[valueOther] = iPutValueHere;
////                m_Elements[iPutValueHere] = valueOther;
////                iPutValueHere++;
            }
        }

        vRemaining.insert(vRemaining.end(), m_Elements.begin() + iPutValueHere, end());

        m_iEnd = iPutValueHere-1;
    }

    void IntersectInPlace(std::vector<int> const &vOther) {
        int iPutValueHere(m_iBegin);
        for (int const valueOther : vOther) {
            if (Contains(valueOther)) {
                SwapElements(valueOther, m_Elements[iPutValueHere++]);
////                int const locationOther(m_Lookup[valueOther]);
////                int const valueExisting(m_Elements[iPutValueHere]);
////
////                m_Lookup[valueExisting] = locationOther;
////                m_Elements[locationOther] = valueExisting;
////
////                m_Lookup[valueOther] = iPutValueHere;
////                m_Elements[iPutValueHere] = valueOther;
////                iPutValueHere++;
            }
        }

        m_iEnd = iPutValueHere-1;
    }

    void DiffInPlace(ResetableArraySet const &other) {
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

    bool operator==(ResetableArraySet const &that) const
    {
        if (Size() != that.Size()) return false;
        for (int const value : *this) {
            if (!that.Contains(value)) return false;
        }
        return true;
    }

    bool operator!=(ResetableArraySet const &that) const
    {
        return !(*this == that);
    }

    static bool Test() {
        std::cout << "ResetableArraySet: ";
        ResetableArraySet testSet(3);
        testSet.Insert(0);
        testSet.Insert(1);
        testSet.Insert(2);

        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ResetableArraySet failed ContainsTest" << std::endl;
            return false;
        }

        testSet.Remove(0);
        if (testSet.Contains(0) || testSet.Size() != 2) {
            std::cout << "FAILED: ResetableArraySet failed RemoveFirst Test" << std::endl;
            return false;
        }

        testSet.Insert(0);
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ResetableArraySet failed RemoveAdd Test" << std::endl;
            return false;
        }

        testSet.Remove(0);
        if (testSet.Contains(0) || testSet.Size() != 2) {
            std::cout << "FAILED: ResetableArraySet failed RemoveLast Test" << std::endl;
            return false;
        }

        testSet.Insert(0);
        testSet.Remove(1);
        if (testSet.Contains(1) || testSet.Size() != 2) {
            std::cout << "FAILED: ResetableArraySet failed RemoveMiddle Test" << std::endl;
            return false;
        }

        testSet.Insert(1);
        testSet.Remove(1);
        testSet.Remove(2);
        testSet.Remove(0);
        if (testSet.Contains(0) || testSet.Contains(1) || testSet.Contains(2) || testSet.Size() != 0 || !testSet.Empty()) {
            std::cout << "FAILED: ResetableArraySet failed RemoveAll Test" << std::endl;
            return false;
        }

        testSet.Insert(0);
        testSet.Insert(1);
        testSet.Insert(2);
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ResetableArraySet failed InsertAll Test" << std::endl;
            return false;
        }

        testSet.SaveState();
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ResetableArraySet failed SaveState Test" << std::endl;
            return false;
        }

        testSet.Remove(0);
        if (testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 2) {
            std::cout << "FAILED: ResetableArraySet failed RemoveAfterSave Test" << std::endl;
            return false;
        }

        testSet.RestoreState();
        if (!testSet.Contains(0) || !testSet.Contains(1) || !testSet.Contains(2) || testSet.Size() != 3) {
            std::cout << "FAILED: ResetableArraySet failed RestoreState Test" << std::endl;
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
#else
    #include "ArraySet.h"
    typedef ArraySet ResetableArraySet;
#endif // USE_RESETABLE

#endif // RESETABLE_ARRAY_SET_H
