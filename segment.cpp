#include "segment.h"
#include <glm/glm.hpp>

Segment::Segment(std::array<float, 2> s, std::array<float, 2> e)
    : start{s[0], s[1]}, end{e[0], e[1]}
{}

// A function that computes the x-intersection of the line segment with a
// horizontal line based on the horizontal line's y-coordinate.
// This function should return a boolean indicating whether or not the lines intersect at all.
// The function will output the x-intercept by writing to a float pointed to by an argument
// to the function.

bool Segment::getIntersection(float y, float *x) const {

    // case 3: skip section because both coords are strictly less/greater and never intersect

    if (!((y <= start[1] && y >= end[1]) || (y >= start[1] && y <= end[1]))) {
        return false;
    }

    // case 1: slope of segment is undefined

    else if (end[0] - start[0] == 0.f) {
        *x = start[0];
        return true;
    }

    // case 2: slope of segment is 0

    else if (end[1] - start[1] == 0.f) {
        return false;
    }

    // else find intersection

    else {

        float slope = (end[1] - start[1]) / (end[0] - start[0]);
        *x = ((y - start[1]) / slope) + start[0];
        return true;
    }
}

std::array<float, 2> Segment::getStart() const {
    return start;
}

std::array<float, 2> Segment::getEnd() const {
    return end;
}



