/**
 * @file faceobject.h
 * @brief Header file for the FaceObject class
 */

#ifndef FACEOBJECT_H
#define FACEOBJECT_H

#include "geo3dobject.h"
#include <QVector>
#include <QVector3D>
#include <QVector2D>

QT_BEGIN_NAMESPACE
namespace Qt3DRender {
class QGeometryRenderer;
class QGeometry;
class QBuffer;
class QAttribute;
}
QT_END_NAMESPACE

/**
 * @class FaceObject
 * @brief A 3D horizontal face object with custom vertex coordinates
 *
 * The FaceObject class creates a horizontal face (polygon) at a specified elevation
 * using custom vertex coordinates. The face is triangulated automatically for rendering.
 * All vertices share the same Y-coordinate (elevation).
 */
class FaceObject : public Geo3DObject
{
public:
    /**
     * @brief Default constructor
     * Creates an empty face at elevation 0
     */
    explicit FaceObject();

    /**
     * @brief Constructor with elevation
     * @param elevation Y-coordinate for all vertices
     */
    explicit FaceObject(float elevation);

    /**
     * @brief Constructor with vertices and elevation
     * @param vertices 2D vertices (X,Z coordinates)
     * @param elevation Y-coordinate for all vertices
     */
    explicit FaceObject(const QVector<QVector2D>& vertices, float elevation);

    /**
     * @brief Virtual destructor
     */
    virtual ~FaceObject();

    // Face-specific properties

    /**
     * @brief Gets the elevation (Y-coordinate) of the face
     */
    float getElevation() const;

    /**
     * @brief Sets the elevation of the face
     */
    void setElevation(float elevation);

    /**
     * @brief Gets the 2D vertices of the face
     */
    QVector<QVector2D> getVertices() const;

    /**
     * @brief Sets the 2D vertices of the face
     */
    void setVertices(const QVector<QVector2D>& vertices);

    /**
     * @brief Adds a vertex to the face
     */
    void addVertex(const QVector2D& vertex);

    /**
     * @brief Adds a vertex to the face
     */
    void addVertex(float x, float z);

    /**
     * @brief Clears all vertices
     */
    void clearVertices();

    /**
     * @brief Gets the number of vertices
     */
    int getVertexCount() const;

    /**
     * @brief Gets 3D vertices (including elevation)
     */
    QVector<QVector3D> get3DVertices() const;

    // JSON Serialization
    QJsonObject toJson() const override;
    bool fromJson(const QJsonObject& json) override;
    QString getObjectType() const override;

protected:
    Qt3DRender::QGeometryRenderer* createGeometry() override;

private:
    float m_elevation;
    QVector<QVector2D> m_vertices;

    /**
     * @brief Triangulates the face vertices
     * @return Indices for triangulation
     */
    QVector<unsigned int> triangulate() const;

    void recreateGeometryIfNeeded();
};

#endif // FACEOBJECT_H
