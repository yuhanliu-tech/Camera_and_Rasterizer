#pragma once
#include <polygon.h>
#include <QImage>
#include <camera.h>

class Rasterizer
{
private:
    //This is the set of Polygons loaded from a JSON scene file
    std::vector<Polygon> m_polygons;

public:
    Rasterizer(const std::vector<Polygon>& polygons);

    QImage RenderScene();
    void ClearScene();

    Camera cam;


private:

    glm::vec4 convertToCameraSpace(glm::vec4 p);
    glm::vec4 convertToScreenSpace(glm::vec4 p);
    glm::vec4 convertToPixelSpace(glm::vec4 p);


    std::array<float,4> makeBoundingBox(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3);


};
