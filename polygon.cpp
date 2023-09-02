#include "polygon.h"
#include <glm/gtx/transform.hpp>

void Polygon::Triangulate()
{
    //TODO: Populate list of triangles
    for (int i = 0; i < m_verts.size() - 2; i++)
    {
        Triangle newT = {0, i + 1, i + 2};
        m_tris.push_back(newT);
    }
}

// for a given Triangle and point within the Triangle, returns the barycentric influence
// that each vertex exerts on any attribute interpolated at the input point.
glm::vec3 Polygon::barycentricInterpolate(const Triangle& tri, glm::vec2 coord) const {

    Vertex v1 = m_verts[tri.m_indices[0]];
    Vertex v2 = m_verts[tri.m_indices[1]];
    Vertex v3 = m_verts[tri.m_indices[2]];

    float areaS = triArea(v1, v2, m_verts[tri.m_indices[2]].m_pos[0], m_verts[tri.m_indices[2]].m_pos[1]);

    float area1 = triArea(v1, v2, coord[0], coord[1]);

    float area2 = triArea(v2, v3, coord[0], coord[1]);

    float area3 = triArea(v3, v1, coord[0], coord[1]);

    float r = (v3.m_color[0] * (area1 / areaS)) + (v1.m_color[0] * (area2 / areaS)) + (v2.m_color[0] * (area3 / areaS));

    float g = (v3.m_color[1] * (area1 / areaS)) + (v1.m_color[1] * (area2 / areaS)) + (v2.m_color[1] * (area3 / areaS));

    float b = (v3.m_color[2] * (area1 / areaS)) + (v1.m_color[2] * (area2 / areaS)) + (v2.m_color[2] * (area3 / areaS));

    r = glm::clamp(r, 0.f, 255.f);

    g = glm::clamp(g, 0.f, 255.f);

    b = glm::clamp(b, 0.f, 255.f);

    return glm::vec3(r, g, b);

}

float Polygon::depthInterpolate(const Triangle& tri, glm::vec2 coord) const {

    Vertex v1 = m_verts[tri.m_indices[0]];
    Vertex v2 = m_verts[tri.m_indices[1]];
    Vertex v3 = m_verts[tri.m_indices[2]];

    float areaS = triArea(v1, v2, m_verts[tri.m_indices[2]].m_pos[0], m_verts[tri.m_indices[2]].m_pos[1]);

    float area1 = triArea(v1, v2, coord[0], coord[1]);

    float area2 = triArea(v2, v3, coord[0], coord[1]);

    float area3 = triArea(v3, v1, coord[0], coord[1]);

    float depth = (v3.m_pos[2] * (area1 / areaS)) + (v1.m_pos[2] * (area2 / areaS)) + (v2.m_pos[2] * (area3 / areaS));

    return depth;

}

float Polygon::persInterpolate(std::array<glm::vec4, 3>& triangleCoords, std::array<float, 3> zCoords, glm::vec2 coord) const {

    float areaS = subTriArea(glm::vec2(triangleCoords[0]), glm::vec2(triangleCoords[1]), glm::vec2(triangleCoords[2]));

    float area1 = subTriArea(glm::vec2(triangleCoords[0]), glm::vec2(triangleCoords[1]), coord);

    float area2 = subTriArea(glm::vec2(triangleCoords[1]), glm::vec2(triangleCoords[2]), coord);

    float area3 = subTriArea(glm::vec2(triangleCoords[2]), glm::vec2(triangleCoords[0]), coord);

    return 1 / ((area1 / (zCoords[2] * areaS)) + (area2 / (zCoords[0] * areaS)) + (area3 / (zCoords[1] * areaS)) );

}

glm::vec2 Polygon::interpolateUV(std::array<glm::vec4, 3>& triangleCoords, std::array<glm::vec2, 3> uv, glm::vec2 coord) const {

    glm::vec2 u0 = uv[0];
    glm::vec2 u1 = uv[1];
    glm::vec2 u2 = uv[2];

    float areaS = subTriArea(glm::vec2(triangleCoords[0]), glm::vec2(triangleCoords[1]), glm::vec2(triangleCoords[2]));

    float area1 = subTriArea(glm::vec2(triangleCoords[0]), glm::vec2(triangleCoords[1]), coord);

    float area2 = subTriArea(glm::vec2(triangleCoords[1]), glm::vec2(triangleCoords[2]), coord);

    float area3 = subTriArea(glm::vec2(triangleCoords[2]), glm::vec2(triangleCoords[0]), coord);

    return  glm::vec2( ((area1 * u2[0]) / areaS) + ((area2 * u0[0]) / areaS) + ((area3 * u1[0]) / areaS),
                        ((area1 * u2[1]) / areaS) + ((area2 * u0[1]) / areaS) + ((area3 * u1[1]) / areaS) );

}

glm::vec4 Polygon::interpolateNormals(std::array<glm::vec4, 3>& triangleCoords, std::array<glm::vec4, 3> normals, glm::vec2 coord) const {

    glm::vec4 n0 = normals[0];
    glm::vec4 n1 = normals[1];
    glm::vec4 n2 = normals[2];

    float areaS = subTriArea(glm::vec2(triangleCoords[0]), glm::vec2(triangleCoords[1]), glm::vec2(triangleCoords[2]));

    float area1 = subTriArea(glm::vec2(triangleCoords[0]), glm::vec2(triangleCoords[1]), coord);

    float area2 = subTriArea(glm::vec2(triangleCoords[1]), glm::vec2(triangleCoords[2]), coord);

    float area3 = subTriArea(glm::vec2(triangleCoords[2]), glm::vec2(triangleCoords[0]), coord);

    return  glm::vec4(((area1 * n2[0]) / areaS) + ((area2 * n0[0]) / areaS) + ((area3 * n1[0]) / areaS),
                    ((area1 * n2[1]) / areaS) + ((area2 * n0[1]) / areaS) + ((area3 * n1[1]) / areaS),
                    ((area1 * n2[2]) / areaS) + ((area2 * n0[2]) / areaS) + ((area3 * n1[2]) / areaS),
                    ((area1 * n2[3]) / areaS) + ((area2 * n0[3]) / areaS) + ((area3 * n1[3]) / areaS));
}

float Polygon::subTriArea(glm::vec2 a, glm::vec2 b, glm::vec2 c) const {

    return 0.5*((a[0] * (b[1] - c[1])) + (b[0] * (c[1] - a[1])) + (c[0] * (a[1] - b[1])));

}

float Polygon::triArea(const Vertex& v1, const Vertex& v2, const float v3x, const float v3y) const {

    float ax = v1.m_pos[0];
    float ay = v1.m_pos[1];
    float bx = v2.m_pos[0];
    float by = v2.m_pos[1];
    float cx = v3x;
    float cy = v3y;

    return 0.5*((ax * (by - cy)) + (bx * (cy - ay)) + (cx * (ay - by)));

}

glm::vec3 GetImageColor(const glm::vec2 &uv_coord, const QImage* const image)
{
    if(image)
    {
        int X = glm::min(image->width() * uv_coord.x, image->width() - 1.0f);
        int Y = glm::min(image->height() * (1.0f - uv_coord.y), image->height() - 1.0f);
        QColor color = image->pixel(X, Y);
        return glm::vec3(color.red(), color.green(), color.blue());
    }
    return glm::vec3(255.f, 255.f, 255.f);
}


// Creates a polygon from the input list of vertex positions and colors
Polygon::Polygon(const QString& name, const std::vector<glm::vec4>& pos, const std::vector<glm::vec3>& col)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    for(unsigned int i = 0; i < pos.size(); i++)
    {
        m_verts.push_back(Vertex(pos[i], col[i], glm::vec4(), glm::vec2()));
    }
    Triangulate();
}

// Creates a regular polygon with a number of sides indicated by the "sides" input integer.
// All of its vertices are of color "color", and the polygon is centered at "pos".
// It is rotated about its center by "rot" degrees, and is scaled from its center by "scale" units
Polygon::Polygon(const QString& name, int sides, glm::vec3 color, glm::vec4 pos, float rot, glm::vec4 scale)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    glm::vec4 v(0.f, 1.f, 0.f, 1.f);
    float angle = 360.f / sides;
    for(int i = 0; i < sides; i++)
    {
        glm::vec4 vert_pos = glm::translate(glm::vec3(pos.x, pos.y, pos.z))
                           * glm::rotate(rot, glm::vec3(0.f, 0.f, 1.f))
                           * glm::scale(glm::vec3(scale.x, scale.y, scale.z))
                           * glm::rotate(i * angle, glm::vec3(0.f, 0.f, 1.f))
                           * v;
        m_verts.push_back(Vertex(vert_pos, color, glm::vec4(), glm::vec2()));
    }

    Triangulate();
}

Polygon::Polygon(const QString &name)
    : m_tris(), m_verts(), m_name(name), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon()
    : m_tris(), m_verts(), m_name("Polygon"), mp_texture(nullptr), mp_normalMap(nullptr)
{}

Polygon::Polygon(const Polygon& p)
    : m_tris(p.m_tris), m_verts(p.m_verts), m_name(p.m_name), mp_texture(nullptr), mp_normalMap(nullptr)
{
    if(p.mp_texture != nullptr)
    {
        mp_texture = new QImage(*p.mp_texture);
    }
    if(p.mp_normalMap != nullptr)
    {
        mp_normalMap = new QImage(*p.mp_normalMap);
    }
}

Polygon::~Polygon()
{
    delete mp_texture;
}

void Polygon::SetTexture(QImage* i)
{
    mp_texture = i;
}

void Polygon::SetNormalMap(QImage* i)
{
    mp_normalMap = i;
}

void Polygon::AddTriangle(const Triangle& t)
{
    m_tris.push_back(t);
}

void Polygon::AddVertex(const Vertex& v)
{
    m_verts.push_back(v);
}

void Polygon::ClearTriangles()
{
    m_tris.clear();
}

Triangle& Polygon::TriAt(unsigned int i)
{
    return m_tris[i];
}

Triangle Polygon::TriAt(unsigned int i) const
{
    return m_tris[i];
}

Vertex &Polygon::VertAt(unsigned int i)
{
    return m_verts[i];
}

Vertex Polygon::VertAt(unsigned int i) const
{
    return m_verts[i];
}
