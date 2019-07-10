#include <algorithm>
#include <set>
#include "constraintkrestricted.h"


ConstraintKRestricted::ConstraintKRestricted(int k, const std::vector<std::pair<int, int> > &arg_intervals) : k(k)
{
    if(k <=0){return;}
    for(const std::pair<int,int> & interval : arg_intervals){
        this->intervals.push_back(Interval(interval.first,interval.second));
    }
    contained.resize(intervals.size(),false);
    std::set<int> events;
    std::vector<std::pair<int,int>> begins;
    for(std::size_t i=0; i < intervals.size(); ++i){

        assert(intervals[i].begin < intervals[i].end);

        events.insert(intervals[i].begin);
        events.insert(intervals[i].end);
        begins.push_back(std::make_pair(intervals[i].begin,i));
    }

    std::sort(begins.begin(), begins.end(),[](const std::pair<int,int> & b1, const std::pair<int,int> & b2){
        return b1.first < b2.first;
    });

    int lastEvent =0;
    bool   first = true;
    for(int event : events){
        if(first){first = false;}
        else{
            segments.push_back(AtomicSegment(lastEvent,event));
        }
        lastEvent = event;
    }

    int currentSegment =0;

    for(std::pair<int,int> & begin : begins){
        Interval & interval = this->intervals[begin.second];

        while(segments[currentSegment].begin < interval.begin){
            currentSegment++;
        }

        assert(segments[currentSegment].begin == interval.begin);

        interval.firstAtomicSegment = currentSegment;

        int lastSegment = currentSegment;
        while(lastSegment < segments.size() &&segments[lastSegment].end <= interval.end){
            segments[lastSegment].intervals.push_back(begin.second);
            lastSegment++;
        }
        interval.lastAtomicSegment = lastSegment;
    }
}

ConstraintKRestricted::ConstraintKRestricted()
{

}
