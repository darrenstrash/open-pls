#ifndef CONSTRAINTKRESTRICTED_H
#define CONSTRAINTKRESTRICTED_H

#include <vector>
#include <cassert>
#include <functional>

struct Interval{
    int begin;
    int end;
    int firstAtomicSegment=-1;
    int lastAtomicSegment=-1;


    std::vector<int> activeIntervals;


    Interval() {}
    Interval(int begin, int end) : begin(begin), end(end) {}
};

struct AtomicSegment{
    int begin;
    int end;
    int    selectedIntervals=0;
    std::vector<int> intervals;

    AtomicSegment(double begin, double end): begin(begin), end(end){

    }
    AtomicSegment(){}
};

class ConstraintKRestricted
{
public:
    ConstraintKRestricted(int k, const std::vector<std::pair<int,int>> & intervals);
    ConstraintKRestricted();

    void addInterval(int i, std::function<void (std::vector<int> & )> candidates){
        assert(canBeAdded(i));
        const Interval & interval = intervals[i];
        for(int j = interval.firstAtomicSegment; j < interval.lastAtomicSegment; ++j){
            segments[j].selectedIntervals++;
            if(segments[j].selectedIntervals == k){
                 candidates(segments[j].intervals);
            }
        }
        contained[i] = true;
    }

    void removeInterval(int i, std::function<void (std::vector<int> & )> candidates){
        assert(contained[i]);
        contained[i] = false;
        const Interval & interval = intervals[i];
        for(int j = interval.firstAtomicSegment; j < interval.lastAtomicSegment; ++j){
            if(segments[j].selectedIntervals == k){
                candidates(segments[j].intervals);
            }
            segments[j].selectedIntervals--;
        }

    }

    bool canBeAdded(int i){

        if(contained[i]){return false;}
        const Interval & interval = intervals[i];
        for(int j = interval.firstAtomicSegment; j < interval.lastAtomicSegment; ++j){
            if(segments[j].selectedIntervals == k){
                return false;
            }
        }
        return true;
    }

    // check if can swap one interval in by swapping one interval out.
////    bool canSwapOneForOne(int i) {
////        if (contained[i]) {
////            return false;
////        }
////        Interval const &interval = intervals[i];
////        for(int j = interval.firstAtomicSegment; j < interval.lastAtomicSegment; ++j){
////            if(segments[j].selectedIntervals == k){
////        }
////    }


    int k;

private:

    std::vector<Interval> intervals;
    std::vector<bool>     contained;
    std::vector<AtomicSegment> segments;


};

#endif // CONSTRAINTKRESTRICTED_H
