#include "tubeobject.h"

#include <Qt3DCore/QGeometry>
#include <Qt3DCore/QBuffer>
#include <Qt3DCore/QAttribute>
#include <Qt3DRender/QGeometryRenderer>
#include <QJsonObject>
#include <QtMath>

// Static registration
static bool s_tubeRegistered = []() {
    Geo3DObject::registerObjectType("Tube", []() -> Geo3DObject* {
        return new TubeObject();
    });
    return true;
}();

TubeObject::TubeObject()
    : Geo3DObject()
    , m_innerRadius(0.5f)
    , m_outerRadius(1.0f)
    , m_height(2.0f)
    , m_rings(20)
    , m_slices(36)
{
}

TubeObject::TubeObject(float innerRadius, float outerRadius, float height)
    : Geo3DObject()
    , m_innerRadius(innerRadius)
    , m_outerRadius(outerRadius)
    , m_height(height)
    , m_rings(20)
    , m_slices(36)
{
}

TubeObject::TubeObject(float innerRadius, float outerRadius, float height, int rings, int slices)
    : Geo3DObject()
    , m_innerRadius(innerRadius)
    , m_outerRadius(outerRadius)
    , m_height(height)
    , m_rings(rings)
    , m_slices(slices)
{
}

TubeObject::~TubeObject()
{
}

float TubeObject::getInnerRadius() const
{
    return m_innerRadius;
}

void TubeObject::setInnerRadius(float radius)
{
    if (m_innerRadius != radius) {
        m_innerRadius = radius;
        recreateGeometryIfNeeded();
    }
}

float TubeObject::getOuterRadius() const
{
    return m_outerRadius;
}

void TubeObject::setOuterRadius(float radius)
{
    if (m_outerRadius != radius) {
        m_outerRadius = radius;
        recreateGeometryIfNeeded();
    }
}

float TubeObject::getHeight() const
{
    return m_height;
}

void TubeObject::setHeight(float height)
{
    if (m_height != height) {
        m_height = height;
        recreateGeometryIfNeeded();
    }
}

int TubeObject::getRings() const
{
    return m_rings;
}

void TubeObject::setRings(int rings)
{
    if (m_rings != rings) {
        m_rings = rings;
        recreateGeometryIfNeeded();
    }
}

int TubeObject::getSlices() const
{
    return m_slices;
}

void TubeObject::setSlices(int slices)
{
    if (m_slices != slices) {
        m_slices = slices;
        recreateGeometryIfNeeded();
    }
}

void TubeObject::setDimensions(float innerRadius, float outerRadius, float height)
{
    bool changed = false;
    if (m_innerRadius != innerRadius) {
        m_innerRadius = innerRadius;
        changed = true;
    }
    if (m_outerRadius != outerRadius) {
        m_outerRadius = outerRadius;
        changed = true;
    }
    if (m_height != height) {
        m_height = height;
        changed = true;
    }

    if (changed) {
        recreateGeometryIfNeeded();
    }
}

void TubeObject::setTessellation(int rings, int slices)
{
    bool changed = false;
    if (m_rings != rings) {
        m_rings = rings;
        changed = true;
    }
    if (m_slices != slices) {
        m_slices = slices;
        changed = true;
    }

    if (changed) {
        recreateGeometryIfNeeded();
    }
}

int TubeObject::getTriangleCount() const
{
    // Outer surface + Inner surface + Top ring + Bottom ring
    return 2 * m_slices * m_rings + 2 * m_slices * 2;
}

Qt3DRender::QGeometryRenderer* TubeObject::createGeometry()
{
    Qt3DCore::QGeometry* geometry = new Qt3DCore::QGeometry();

    // Calculate vertex count
    // We need vertices for: outer surface, inner surface, top ring, bottom ring
    int vertexCount = (m_rings + 1) * (m_slices + 1) * 2  // Outer and inner surfaces
                    + (m_slices + 1) * 2 * 2;              // Top and bottom rings (duplicated for normals)

    // Vertex data: position (3 floats) + normal (3 floats)
    QByteArray vertexBufferData;
    vertexBufferData.resize(vertexCount * 6 * sizeof(float));
    float* vertexPtr = reinterpret_cast<float*>(vertexBufferData.data());

    QVector<unsigned int> indices;

    float halfHeight = m_height / 2.0f;
    int vertexIndex = 0;

    // Generate outer surface vertices
    for (int ring = 0; ring <= m_rings; ++ring) {
        float y = -halfHeight + (m_height * ring) / m_rings;
        for (int slice = 0; slice <= m_slices; ++slice) {
            float theta = 2.0f * M_PI * slice / m_slices;
            float x = m_outerRadius * qCos(theta);
            float z = m_outerRadius * qSin(theta);

            // Position
            *vertexPtr++ = x;
            *vertexPtr++ = y;
            *vertexPtr++ = z;

            // Normal (pointing outward)
            *vertexPtr++ = qCos(theta);
            *vertexPtr++ = 0.0f;
            *vertexPtr++ = qSin(theta);

            vertexIndex++;
        }
    }

    // Generate indices for outer surface
    int outerBaseVertex = 0;
    for (int ring = 0; ring < m_rings; ++ring) {
        for (int slice = 0; slice < m_slices; ++slice) {
            int current = outerBaseVertex + ring * (m_slices + 1) + slice;
            int next = current + m_slices + 1;

            indices.append(current);
            indices.append(next);
            indices.append(current + 1);

            indices.append(current + 1);
            indices.append(next);
            indices.append(next + 1);
        }
    }

    // Generate inner surface vertices
    int innerBaseVertex = vertexIndex;
    for (int ring = 0; ring <= m_rings; ++ring) {
        float y = -halfHeight + (m_height * ring) / m_rings;
        for (int slice = 0; slice <= m_slices; ++slice) {
            float theta = 2.0f * M_PI * slice / m_slices;
            float x = m_innerRadius * qCos(theta);
            float z = m_innerRadius * qSin(theta);

            // Position
            *vertexPtr++ = x;
            *vertexPtr++ = y;
            *vertexPtr++ = z;

            // Normal (pointing inward)
            *vertexPtr++ = -qCos(theta);
            *vertexPtr++ = 0.0f;
            *vertexPtr++ = -qSin(theta);

            vertexIndex++;
        }
    }

    // Generate indices for inner surface (reversed winding)
    for (int ring = 0; ring < m_rings; ++ring) {
        for (int slice = 0; slice < m_slices; ++slice) {
            int current = innerBaseVertex + ring * (m_slices + 1) + slice;
            int next = current + m_slices + 1;

            indices.append(current);
            indices.append(current + 1);
            indices.append(next);

            indices.append(current + 1);
            indices.append(next + 1);
            indices.append(next);
        }
    }

    // Generate top ring (annulus)
    int topRingBaseVertex = vertexIndex;
    for (int slice = 0; slice <= m_slices; ++slice) {
        float theta = 2.0f * M_PI * slice / m_slices;

        // Outer edge
        *vertexPtr++ = m_outerRadius * qCos(theta);
        *vertexPtr++ = halfHeight;
        *vertexPtr++ = m_outerRadius * qSin(theta);
        *vertexPtr++ = 0.0f;
        *vertexPtr++ = 1.0f;  // Normal pointing up
        *vertexPtr++ = 0.0f;
        vertexIndex++;

        // Inner edge
        *vertexPtr++ = m_innerRadius * qCos(theta);
        *vertexPtr++ = halfHeight;
        *vertexPtr++ = m_innerRadius * qSin(theta);
        *vertexPtr++ = 0.0f;
        *vertexPtr++ = 1.0f;  // Normal pointing up
        *vertexPtr++ = 0.0f;
        vertexIndex++;
    }

    // Generate indices for top ring
    for (int slice = 0; slice < m_slices; ++slice) {
        int outerCurrent = topRingBaseVertex + slice * 2;
        int innerCurrent = outerCurrent + 1;
        int outerNext = outerCurrent + 2;
        int innerNext = innerCurrent + 2;

        indices.append(outerCurrent);
        indices.append(innerCurrent);
        indices.append(outerNext);

        indices.append(outerNext);
        indices.append(innerCurrent);
        indices.append(innerNext);
    }

    // Generate bottom ring (annulus)
    int bottomRingBaseVertex = vertexIndex;
    for (int slice = 0; slice <= m_slices; ++slice) {
        float theta = 2.0f * M_PI * slice / m_slices;

        // Outer edge
        *vertexPtr++ = m_outerRadius * qCos(theta);
        *vertexPtr++ = -halfHeight;
        *vertexPtr++ = m_outerRadius * qSin(theta);
        *vertexPtr++ = 0.0f;
        *vertexPtr++ = -1.0f;  // Normal pointing down
        *vertexPtr++ = 0.0f;
        vertexIndex++;

        // Inner edge
        *vertexPtr++ = m_innerRadius * qCos(theta);
        *vertexPtr++ = -halfHeight;
        *vertexPtr++ = m_innerRadius * qSin(theta);
        *vertexPtr++ = 0.0f;
        *vertexPtr++ = -1.0f;  // Normal pointing down
        *vertexPtr++ = 0.0f;
        vertexIndex++;
    }

    // Generate indices for bottom ring (reversed winding)
    for (int slice = 0; slice < m_slices; ++slice) {
        int outerCurrent = bottomRingBaseVertex + slice * 2;
        int innerCurrent = outerCurrent + 1;
        int outerNext = outerCurrent + 2;
        int innerNext = innerCurrent + 2;

        indices.append(outerCurrent);
        indices.append(outerNext);
        indices.append(innerCurrent);

        indices.append(outerNext);
        indices.append(innerNext);
        indices.append(innerCurrent);
    }

    // Create vertex buffer
    Qt3DCore::QBuffer* vertexBuffer = new Qt3DCore::QBuffer(geometry);
    vertexBuffer->setData(vertexBufferData);

    // Create index buffer
    QByteArray indexBufferData;
    indexBufferData.resize(indices.size() * sizeof(unsigned int));
    unsigned int* indexPtr = reinterpret_cast<unsigned int*>(indexBufferData.data());
    for (unsigned int index : indices) {
        *indexPtr++ = index;
    }

    Qt3DCore::QBuffer* indexBuffer = new Qt3DCore::QBuffer(geometry);
    indexBuffer->setData(indexBufferData);

    // Position attribute
    Qt3DCore::QAttribute* positionAttribute = new Qt3DCore::QAttribute(geometry);
    positionAttribute->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexBuffer);
    positionAttribute->setByteStride(6 * sizeof(float));
    positionAttribute->setCount(vertexIndex);
    geometry->addAttribute(positionAttribute);

    // Normal attribute
    Qt3DCore::QAttribute* normalAttribute = new Qt3DCore::QAttribute(geometry);
    normalAttribute->setName(Qt3DCore::QAttribute::defaultNormalAttributeName());
    normalAttribute->setVertexBaseType(Qt3DCore::QAttribute::Float);
    normalAttribute->setVertexSize(3);
    normalAttribute->setAttributeType(Qt3DCore::QAttribute::VertexAttribute);
    normalAttribute->setBuffer(vertexBuffer);
    normalAttribute->setByteStride(6 * sizeof(float));
    normalAttribute->setByteOffset(3 * sizeof(float));
    normalAttribute->setCount(vertexIndex);
    geometry->addAttribute(normalAttribute);

    // Index attribute
    Qt3DCore::QAttribute* indexAttribute = new Qt3DCore::QAttribute(geometry);
    indexAttribute->setAttributeType(Qt3DCore::QAttribute::IndexAttribute);
    indexAttribute->setVertexBaseType(Qt3DCore::QAttribute::UnsignedInt);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(indices.size());
    geometry->addAttribute(indexAttribute);

    Qt3DRender::QGeometryRenderer* renderer = new Qt3DRender::QGeometryRenderer();
    renderer->setGeometry(geometry);
    renderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);

    return renderer;
}

void TubeObject::recreateGeometryIfNeeded()
{
    // Placeholder - same concept as CylinderObject
}

QJsonObject TubeObject::toJson() const
{
    QJsonObject json;
    json["type"] = getObjectType();

    // Transform
    QJsonObject transform;
    QVector3D pos = getPosition();
    QVector3D rot = getRotation();
    QVector3D scale = getScale();

    transform["position"] = QJsonObject{{"x", pos.x()}, {"y", pos.y()}, {"z", pos.z()}};
    transform["rotation"] = QJsonObject{{"x", rot.x()}, {"y", rot.y()}, {"z", rot.z()}};
    transform["scale"] = QJsonObject{{"x", scale.x()}, {"y", scale.y()}, {"z", scale.z()}};
    json["transform"] = transform;

    // Material
    QJsonObject material;
    QColor diffuse = getDiffuseColor();
    QColor ambient = getAmbientColor();
    QColor specular = getSpecularColor();

    material["diffuse"] = QJsonObject{{"r", diffuse.red()}, {"g", diffuse.green()}, {"b", diffuse.blue()}, {"a", diffuse.alpha()}};
    material["ambient"] = QJsonObject{{"r", ambient.red()}, {"g", ambient.green()}, {"b", ambient.blue()}, {"a", ambient.alpha()}};
    material["specular"] = QJsonObject{{"r", specular.red()}, {"g", specular.green()}, {"b", specular.blue()}, {"a", specular.alpha()}};
    material["shininess"] = getShininess();
    json["material"] = material;

    json["visible"] = isVisible();
    json["opacity"] = getOpacity();

    // Tube properties
    QJsonObject tube;
    tube["innerRadius"] = m_innerRadius;
    tube["outerRadius"] = m_outerRadius;
    tube["height"] = m_height;
    tube["rings"] = m_rings;
    tube["slices"] = m_slices;
    json["tube"] = tube;

    return json;
}

bool TubeObject::fromJson(const QJsonObject& json)
{
    if (json["type"].toString() != getObjectType()) {
        return false;
    }

    // Load transform
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

    // Load material
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

    // Load tube properties
    if (json.contains("tube")) {
        QJsonObject tube = json["tube"].toObject();
        if (tube.contains("innerRadius") && tube.contains("outerRadius") && tube.contains("height")) {
            setDimensions(tube["innerRadius"].toDouble(), tube["outerRadius"].toDouble(), tube["height"].toDouble());
        }
        if (tube.contains("rings") && tube.contains("slices")) {
            setTessellation(tube["rings"].toInt(), tube["slices"].toInt());
        }
    }

    return true;
}

QString TubeObject::getObjectType() const
{
    return "Tube";
}
