#include "rasterizer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "segment.h"

Rasterizer::Rasterizer(const std::vector<Polygon>& polygons)
    : m_polygons(polygons), cam(Camera())
{}

QImage Rasterizer::RenderScene()
{
    QImage result(512, 512, QImage::Format_RGB32);
    // Fill the image with black pixels.
    // Note that qRgb creates a QColor,
    // and takes in values [0, 255] rather than [0, 1].
    result.fill(qRgb(0.f, 0.f, 0.f));
    // TODO: Complete the various components of code that make up this function.
    // It should return the rasterized image of the current scene.

    std::array<float, 512 * 512> depth;
    depth.fill(INT_MAX);

    for ( Polygon& poly : m_polygons ) {

        for (Triangle& tri : poly.m_tris) {

            Vertex& v1World = poly.m_verts[tri.m_indices[0]];
            Vertex& v2World = poly.m_verts[tri.m_indices[1]];
            Vertex& v3World = poly.m_verts[tri.m_indices[2]];

            glm::vec4 v1 = convertToCameraSpace(v1World.m_pos);
            glm::vec4 v2 = convertToCameraSpace(v2World.m_pos);
            glm::vec4 v3 = convertToCameraSpace(v3World.m_pos);
            std::array<float, 3> zCoords = {v1[2], v2[2], v3[2]};

            v1 = convertToPixelSpace(v1);
            v2 = convertToPixelSpace(v2);
            v3 = convertToPixelSpace(v3);
            std::array<glm::vec4, 3> triangleCoords = {v1, v2, v3};

            std::array<Segment, 3> segments = {
                Segment({v1[0], v1[1]}, {v2[0], v2[1]}),
                Segment({v2[0], v2[1]}, {v3[0], v3[1]}),
                Segment({v3[0], v3[1]}, {v1[0], v1[1]})
            };

            std::array<float, 4> boundingBox = makeBoundingBox(v1, v2, v3);

            for (float y = std::ceil(boundingBox[2]); y <= std::floor(boundingBox[3]); y++) {

                float xLeft = 512.0f;
                float xRight = 0.0f;

                for (Segment s : segments) {
                    float xIntersects;
                    if (s.getIntersection(y, &xIntersects)) {
                        xLeft = std::min(xLeft, xIntersects);
                        xRight = std::max(xRight, xIntersects);
                        xLeft = glm::clamp(xLeft, 0.f, 511.f);
                        xRight = glm::clamp(xRight, 0.f, 511.f);
                    }
                }

                for (float x = std::ceil(xLeft); x <= std::floor(xRight); x++) {

                    float z = poly.persInterpolate(triangleCoords, zCoords, glm::vec2(x,y));

                    float index = x + 512 * std::floor(y);

                    if (z < depth[static_cast <int> (index)]) {
                        depth[static_cast <int> (index)] = z;

                        std::array<glm::vec4, 3> normals = {v1World.m_normal, v2World.m_normal, v3World.m_normal };
                        glm::vec4 weightedNormals = poly.interpolateNormals(triangleCoords, normals, glm::vec2(x,y));
                        float n = std::clamp(glm::dot(glm::vec3(weightedNormals), glm::vec3(cam.getForward())), 0.0f, 1.0f);

                        std::array<glm::vec2, 3> uvs = { v1World.m_uv, v2World.m_uv, v3World.m_uv};
                        glm::vec2 weightedUVs = poly.interpolateUV(triangleCoords, uvs, glm::vec2(x,y));
                        glm::vec3 colors = n * GetImageColor(weightedUVs, poly.mp_texture);

                        result.setPixelColor(x, y, qRgb(colors[0],colors[1],colors[2]));

                    }
                }

            }
        }

    }

    // Make liberal use of helper functions; writing your rasterizer as one
    // long RenderScene function will make it (a) hard to debug and
    // (b) hard to write without copy-pasting. Also, Adam will be sad when
    // he reads your code.

    // Also! As per the style requirements for this assignment, make sure you
    // use std::arrays to store things like your line segments, Triangles, and
    // vertex coordinates. This lets you easily use loops to perform operations
    // on your scene components, rather than copy-pasting operations three times
    // each!
    return result;
}

//Create a way to compute and store the 2D axis-aligned bounding box of a triangle.
//You will use these bounding boxes to improve the efficiency of rendering each triangle.
//Rather than testing every single row of the screen against a triangle,
//you will only test the screen rows contained within the triangle's bounding box.
//Remember to clamp each bounding box to the screen's limits.

std::array<float,4> Rasterizer::makeBoundingBox(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3) {

    float xStart = std::min(v1[0], v2[0]);
    xStart = std::min(xStart, v3[0]);

    float xEnd = std::max(v1[0], v2[0]);
    xEnd = std::max(xEnd, v3[0]);

    float yStart = std::min(v1[1], v2[1]);
    yStart = std::min(yStart, v3[1]);

    float yEnd = std::max(v1[1], v2[1]);
    yEnd = std::max(yEnd, v3[1]);

    xStart = glm::clamp(xStart, 0.f, 511.f);
    xEnd = glm::clamp(xEnd, 0.f, 511.f);
    yStart = glm::clamp(yStart, 0.f, 511.f);
    yEnd = glm::clamp(yEnd, 0.f, 511.f);

    return {xStart, xEnd, yStart, yEnd};
    // result in the form of xStart, xEnd, yStart, yEnd
}

glm::vec4 Rasterizer::convertToCameraSpace(glm::vec4 p) {

    // converted to camera space:
    p = cam.returnViewMat() * p;
    return p;
}

glm::vec4 Rasterizer::convertToScreenSpace(glm::vec4 p) {

    // converted to unhomogenized screen space:
    p = cam.returnPersProjMat() * p;

    // convert to screen space:
    p /= p[3];

    return p;
}

glm::vec4 Rasterizer::convertToPixelSpace(glm::vec4 p) {

    p = convertToScreenSpace(p);

    //convert to pixel space:
    p[0] = ((p[0] + 1) / 2) * 512;
    p[1] = ((1 - p[1]) / 2) * 512;

    return p;
}


void Rasterizer::ClearScene()
{
    m_polygons.clear();
}
