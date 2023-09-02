#pragma once
#ifndef SEGMENT_H
#define SEGMENT_H
#include <array>

class Segment
{

protected:

    std::array<float, 2> start;
    std::array<float, 2> end;

public:

    Segment(std::array<float, 2> s, std::array<float, 2> e);
    bool getIntersection(float y, float* x) const;

    std::array<float, 2> getStart() const;
    std::array<float, 2> getEnd() const;

};

#endif // SEGMENT_H
