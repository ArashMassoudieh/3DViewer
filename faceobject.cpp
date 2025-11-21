#include "faceobject.h"

#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QAttribute>
#include <Qt3DCore/QEntity>
#include <QJsonObject>
#include <QJsonArray>

// Static registration
static bool s_faceRegistered = []() {
    Geo3DObject::registerObjectType("Face", []() -> Geo3DObject* {
        return new FaceObject();
    });
    return true;
}();

FaceObject::FaceObject()
    : Geo3DObject()
    , m_elevation(0.0f)
{
}

FaceObject::FaceObject(float elevation)
    : Geo3DObject()
    , m_elevation(elevation)
{
}

FaceObject::FaceObject(const QVector<QVector2D>& vertices, float elevation)
    : Geo3DObject()
    , m_elevation(elevation)
    , m_vertices(vertices)
{
}

FaceObject::~FaceObject()
{
}

float FaceObject::getElevation() const
{
    return m_elevation;
}

void FaceObject::setElevation(float elevation)
{
    if (m_elevation != elevation) {
        m_elevation = elevation;
        recreateGeometryIfNeeded();
    }
}

QVector<QVector2D> FaceObject::getVertices() const
{
    return m_vertices;
}

void FaceObject::setVertices(const QVector<QVector2D>& vertices)
{
    m_vertices = vertices;
    recreateGeometryIfNeeded();
}

void FaceObject::addVertex(const QVector2D& vertex)
{
    m_vertices.append(vertex);
    recreateGeometryIfNeeded();
}

void FaceObject::addVertex(float x, float z)
{
    addVertex(QVector2D(x, z));
}

void FaceObject::clearVertices()
{
    m_vertices.clear();
    recreateGeometryIfNeeded();
}

int FaceObject::getVertexCount() const
{
    return m_vertices.size();
}

QVector<QVector3D> FaceObject::get3DVertices() const
{
    QVector<QVector3D> vertices3D;
    for (const QVector2D& vertex : m_vertices) {
        vertices3D.append(QVector3D(vertex.x(), m_elevation, vertex.y()));
    }
    return vertices3D;
}

Qt3DRender::QGeometryRenderer* FaceObject::createGeometry()
{
    if (m_vertices.size() < 3) {
        return nullptr;
    }

    Qt3DCore::QGeometry* geometry = new Qt3DCore::QGeometry();    // Changed from Qt3DRender::QGeometry

    // Get 3D vertices
    QVector<QVector3D> vertices3D = get3DVertices();

    // Triangulate the face
    QVector<unsigned int> indices = triangulate();

    // Create vertex buffer
    QByteArray vertexBufferData;
    vertexBufferData.resize(vertices3D.size() * 3 * sizeof(float));
    float* vertexPtr = reinterpret_cast<float*>(vertexBufferData.data());

    for (int i = 0; i < vertices3D.size(); ++i) {
        *vertexPtr++ = vertices3D[i].x();
        *vertexPtr++ = vertices3D[i].y();
        *vertexPtr++ = vertices3D[i].z();
    }

    Qt3DCore::QBuffer* vertexBuffer = new Qt3DCore::QBuffer(geometry);    // Changed from Qt3DRender::QBuffer
    vertexBuffer->setData(vertexBufferData);

    // Create index buffer
    QByteArray indexBufferData;
    indexBufferData.resize(indices.size() * sizeof(unsigned int));
    unsigned int* indexPtr = reinterpret_cast<unsigned int*>(indexBufferData.data());
    for (unsigned int index : indices) {
        *indexPtr++ = index;
    }

    Qt3DCore::QBuffer* indexBuffer = new Qt3DCore::QBuffer(geometry);    // Changed from Qt3DRender::QBuffer
    indexBuffer->setData(indexBufferData);

    // Position attribute
    Qt3DCore::QAttribute* positionAttribute = new Qt3DCore::QAttribute(geometry);    // Changed from Qt3DRender::QAttribute
    positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());    // Changed namespace
    positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);    // Changed namespace
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);    // Changed namespace
    positionAttribute->setBuffer(vertexBuffer);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(vertices3D.size());
    geometry->addAttribute(positionAttribute);

    // Index attribute
    Qt3DCore::QAttribute* indexAttribute = new Qt3DCore::QAttribute(geometry);    // Changed from Qt3DRender::QAttribute
    indexAttribute->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);    // Changed namespace
    indexAttribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedInt);    // Changed namespace
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(indices.size());
    geometry->addAttribute(indexAttribute);

    Qt3DRender::QGeometryRenderer* renderer = new Qt3DRender::QGeometryRenderer();
    renderer->setGeometry(geometry);
    renderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);

    return renderer;
}

QVector<unsigned int> FaceObject::triangulate() const
{
    QVector<unsigned int> indices;

    // Simple fan triangulation (works for convex polygons)
    // For complex polygons, you'd need a proper triangulation algorithm
    if (m_vertices.size() >= 3) {
        for (int i = 1; i < m_vertices.size() - 1; ++i) {
            indices.append(0);
            indices.append(i);
            indices.append(i + 1);
        }
    }

    return indices;
}

QJsonObject FaceObject::toJson() const
{
    QJsonObject json;
    json["type"] = getObjectType();

    // Base class properties - Transform
    QVector3D pos = getPosition();
    QVector3D rot = getRotation();
    QVector3D scale = getScale();

    QJsonObject transform;
    transform["position"] = QJsonObject{{"x", pos.x()}, {"y", pos.y()}, {"z", pos.z()}};
    transform["rotation"] = QJsonObject{{"x", rot.x()}, {"y", rot.y()}, {"z", rot.z()}};
    transform["scale"] = QJsonObject{{"x", scale.x()}, {"y", scale.y()}, {"z", scale.z()}};
    json["transform"] = transform;

    // Base class properties - Material
    QColor diffuse = getDiffuseColor();
    QColor ambient = getAmbientColor();
    QColor specular = getSpecularColor();

    QJsonObject material;
    material["diffuse"] = QJsonObject{{"r", diffuse.red()}, {"g", diffuse.green()}, {"b", diffuse.blue()}, {"a", diffuse.alpha()}};
    material["ambient"] = QJsonObject{{"r", ambient.red()}, {"g", ambient.green()}, {"b", ambient.blue()}, {"a", ambient.alpha()}};
    material["specular"] = QJsonObject{{"r", specular.red()}, {"g", specular.green()}, {"b", specular.blue()}, {"a", specular.alpha()}};
    material["shininess"] = getShininess();
    json["material"] = material;

    json["visible"] = isVisible();
    json["opacity"] = getOpacity();

    // Face-specific properties
    QJsonObject face;
    face["elevation"] = m_elevation;

    QJsonArray verticesArray;
    for (const QVector2D& vertex : m_vertices) {
        QJsonObject vertexObj;
        vertexObj["x"] = vertex.x();
        vertexObj["z"] = vertex.y();
        verticesArray.append(vertexObj);
    }
    face["vertices"] = verticesArray;
    json["face"] = face;

    return json;
}

bool FaceObject::fromJson(const QJsonObject& json)
{
    if (json["type"].toString() != getObjectType()) {
        return false;
    }

    // Load base class properties (same as CylinderObject)
    if (json.contains("transform")) {
        QJsonObject transform = json["transform"].toObject();
        if (transform.contains("position")) {
            QJsonObject pos = transform["position"].toObject();
            setPosition(pos["x"].toDouble(), pos["y"].toDouble(), pos["z"].toDouble());
        }
        if (transform.contains("rotation")) {
            QJsonObject rot = transform["rotation"].toObject();
            setRotation(rot["x"].toDouble(), rot["y"].toDouble(), rot["z"].toDouble());
        }
        if (transform.contains("scale")) {
            QJsonObject scale = transform["scale"].toObject();
            setScale(scale["x"].toDouble(), scale["y"].toDouble(), scale["z"].toDouble());
        }
    }

    if (json.contains("material")) {
        QJsonObject material = json["material"].toObject();
        if (material.contains("diffuse")) {
            QJsonObject diffuse = material["diffuse"].toObject();
            setDiffuseColor(QColor(diffuse["r"].toInt(), diffuse["g"].toInt(), diffuse["b"].toInt(), diffuse["a"].toInt()));
        }
        if (material.contains("ambient")) {
            QJsonObject ambient = material["ambient"].toObject();
            setAmbientColor(QColor(ambient["r"].toInt(), ambient["g"].toInt(), ambient["b"].toInt(), ambient["a"].toInt()));
        }
        if (material.contains("specular")) {
            QJsonObject specular = material["specular"].toObject();
            setSpecularColor(QColor(specular["r"].toInt(), specular["g"].toInt(), specular["b"].toInt(), specular["a"].toInt()));
        }
        if (material.contains("shininess")) {
            setShininess(material["shininess"].toDouble());
        }
    }

    if (json.contains("visible")) {
        setVisible(json["visible"].toBool());
    }

    if (json.contains("opacity")) {
        setOpacity(json["opacity"].toDouble());
    }

    // Load face-specific properties
    if (json.contains("face")) {
        QJsonObject face = json["face"].toObject();

        if (face.contains("elevation")) {
            m_elevation = face["elevation"].toDouble();
        }

        if (face.contains("vertices") && face["vertices"].isArray()) {
            QJsonArray verticesArray = face["vertices"].toArray();
            m_vertices.clear();

            for (const QJsonValue& vertexValue : verticesArray) {
                if (vertexValue.isObject()) {
                    QJsonObject vertexObj = vertexValue.toObject();
                    float x = vertexObj["x"].toDouble();
                    float z = vertexObj["z"].toDouble();
                    m_vertices.append(QVector2D(x, z));
                }
            }
        }
    }

    return true;
}

QString FaceObject::getObjectType() const
{
    return "Face";
}

void FaceObject::recreateGeometryIfNeeded()
{
    // Placeholder - same concept as CylinderObject
}
