#include "cylinderobject.h"

#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DCore/QEntity>
#include <QJsonObject>

CylinderObject::CylinderObject()
    : Geo3DObject()
    , m_radius(1.0f)
    , m_length(2.0f)
    , m_rings(50)
    , m_slices(20)
{
}

CylinderObject::CylinderObject(float radius, float length)
    : Geo3DObject()
    , m_radius(radius)
    , m_length(length)
    , m_rings(50)
    , m_slices(20)
{
}

CylinderObject::CylinderObject(float radius, float length, int rings, int slices)
    : Geo3DObject()
    , m_radius(radius)
    , m_length(length)
    , m_rings(rings)
    , m_slices(slices)
{
}

CylinderObject::~CylinderObject()
{
    // Base class destructor will handle cleanup
}

float CylinderObject::getRadius() const
{
    return m_radius;
}

void CylinderObject::setRadius(float radius)
{
    if (m_radius != radius) {
        m_radius = radius;
        recreateGeometryIfNeeded();
    }
}

float CylinderObject::getLength() const
{
    return m_length;
}

void CylinderObject::setLength(float length)
{
    if (m_length != length) {
        m_length = length;
        recreateGeometryIfNeeded();
    }
}

int CylinderObject::getRings() const
{
    return m_rings;
}

void CylinderObject::setRings(int rings)
{
    if (m_rings != rings) {
        m_rings = rings;
        recreateGeometryIfNeeded();
    }
}

int CylinderObject::getSlices() const
{
    return m_slices;
}

void CylinderObject::setSlices(int slices)
{
    if (m_slices != slices) {
        m_slices = slices;
        recreateGeometryIfNeeded();
    }
}

void CylinderObject::setDimensions(float radius, float length)
{
    bool changed = false;
    if (m_radius != radius) {
        m_radius = radius;
        changed = true;
    }
    if (m_length != length) {
        m_length = length;
        changed = true;
    }

    if (changed) {
        recreateGeometryIfNeeded();
    }
}

void CylinderObject::setTessellation(int rings, int slices)
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

int CylinderObject::getTriangleCount() const
{
    // Approximation: cylinder has roughly 2 * slices * rings triangles
    // This includes the sides and caps
    return 2 * m_slices * m_rings + 2 * m_slices; // sides + caps
}

Qt3DRender::QGeometryRenderer* CylinderObject::createGeometry()
{
    Qt3DExtras::QCylinderMesh* cylinderMesh = new Qt3DExtras::QCylinderMesh();

    // Set cylinder parameters
    cylinderMesh->setRadius(m_radius);
    cylinderMesh->setLength(m_length);
    cylinderMesh->setRings(m_rings);
    cylinderMesh->setSlices(m_slices);

    return cylinderMesh;
}

void CylinderObject::recreateGeometryIfNeeded()
{
    // This is a simplified approach. In a more complete implementation,
    // you would need access to the entity and geometry renderer to replace them.
    // For now, this serves as a placeholder for the concept.
    // The actual implementation would need to:
    // 1. Check if createEntity() has been called (geometry exists)
    // 2. Remove old geometry component from entity
    // 3. Create new geometry with createGeometry()
    // 4. Add new geometry component to entity

    // Since we don't have direct access to these from the base class,
    // a more complete implementation might require additional methods
    // in the base class or a different architecture.
}

QJsonObject CylinderObject::toJson() const
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

    // Cylinder properties
    QJsonObject cylinder;
    cylinder["radius"] = m_radius;
    cylinder["length"] = m_length;
    cylinder["rings"] = m_rings;
    cylinder["slices"] = m_slices;
    json["cylinder"] = cylinder;

    return json;
}

bool CylinderObject::fromJson(const QJsonObject& json)
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

    // Load cylinder properties
    if (json.contains("cylinder")) {
        QJsonObject cylinder = json["cylinder"].toObject();
        if (cylinder.contains("radius") && cylinder.contains("length")) {
            setDimensions(cylinder["radius"].toDouble(), cylinder["length"].toDouble());
        }
        if (cylinder.contains("rings") && cylinder.contains("slices")) {
            setTessellation(cylinder["rings"].toInt(), cylinder["slices"].toInt());
        }
    }

    return true;
}

QString CylinderObject::getObjectType() const
{
    return "Cylinder";
}

// Static registration - runs when the program starts
static bool s_cylinderRegistered = []() {
    Geo3DObject::registerObjectType("Cylinder", []() -> Geo3DObject* {
        return new CylinderObject();
    });
    return true;
}();
