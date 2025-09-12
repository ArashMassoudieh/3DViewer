#include "geo3dobject.h"

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DExtras/QPhongMaterial>

Geo3DObject::Geo3DObject()
    : m_position(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f, 0.0f, 0.0f)
    , m_scale(1.0f, 1.0f, 1.0f)
    , m_diffuseColor(QColor(102, 84, 35))  // Brown color
    , m_ambientColor(QColor(68, 51, 17))   // Darker brown
    , m_specularColor(QColor(255, 255, 255)) // White
    , m_shininess(50.0f)
    , m_visible(true)
    , m_entity(nullptr)
    , m_transform(nullptr)
    , m_material(nullptr)
    , m_geometryRenderer(nullptr)
{
}

Geo3DObject::~Geo3DObject()
{
    // Qt3D entities are automatically cleaned up by parent-child relationships
    // No explicit cleanup needed here
}

QVector3D Geo3DObject::getPosition() const
{
    return m_position;
}

void Geo3DObject::setPosition(const QVector3D& position)
{
    m_position = position;
    updateTransform();
}

void Geo3DObject::setPosition(float x, float y, float z)
{
    setPosition(QVector3D(x, y, z));
}

QVector3D Geo3DObject::getRotation() const
{
    return m_rotation;
}

void Geo3DObject::setRotation(const QVector3D& rotation)
{
    m_rotation = rotation;
    updateTransform();
}

void Geo3DObject::setRotation(float x, float y, float z)
{
    setRotation(QVector3D(x, y, z));
}

QVector3D Geo3DObject::getScale() const
{
    return m_scale;
}

void Geo3DObject::setScale(const QVector3D& scale)
{
    m_scale = scale;
    updateTransform();
}

void Geo3DObject::setScale(float uniformScale)
{
    setScale(QVector3D(uniformScale, uniformScale, uniformScale));
}

void Geo3DObject::setScale(float x, float y, float z)
{
    setScale(QVector3D(x, y, z));
}

QColor Geo3DObject::getDiffuseColor() const
{
    return m_diffuseColor;
}

void Geo3DObject::setDiffuseColor(const QColor& color)
{
    m_diffuseColor = color;
    updateMaterial();
}

QColor Geo3DObject::getAmbientColor() const
{
    return m_ambientColor;
}

void Geo3DObject::setAmbientColor(const QColor& color)
{
    m_ambientColor = color;
    updateMaterial();
}

QColor Geo3DObject::getSpecularColor() const
{
    return m_specularColor;
}

void Geo3DObject::setSpecularColor(const QColor& color)
{
    m_specularColor = color;
    updateMaterial();
}

float Geo3DObject::getShininess() const
{
    return m_shininess;
}

void Geo3DObject::setShininess(float shininess)
{
    m_shininess = shininess;
    updateMaterial();
}

bool Geo3DObject::isVisible() const
{
    return m_visible;
}

void Geo3DObject::setVisible(bool visible)
{
    m_visible = visible;
    if (m_entity) {
        m_entity->setEnabled(visible);
    }
}

Qt3DCore::QEntity* Geo3DObject::createEntity(Qt3DCore::QEntity* parent)
{
    if (!m_entity) {
        m_entity = new Qt3DCore::QEntity(parent);

        // Create geometry
        m_geometryRenderer = createGeometry();
        m_entity->addComponent(m_geometryRenderer);

        // Create transform
        m_transform = new Qt3DCore::QTransform();
        updateTransform();
        m_entity->addComponent(m_transform);

        // Create material
        m_material = new Qt3DExtras::QPhongMaterial();
        updateMaterial();
        m_entity->addComponent(m_material);

        // Set visibility
        m_entity->setEnabled(m_visible);
    }

    return m_entity;
}

void Geo3DObject::updateTransform()
{
    if (m_transform) {
        m_transform->setTranslation(m_position);
        m_transform->setRotationX(m_rotation.x());
        m_transform->setRotationY(m_rotation.y());
        m_transform->setRotationZ(m_rotation.z());
        m_transform->setScale3D(m_scale);
    }
}

void Geo3DObject::updateMaterial()
{
    if (m_material) {
        m_material->setDiffuse(m_diffuseColor);
        m_material->setAmbient(m_ambientColor);
        m_material->setSpecular(m_specularColor);
        m_material->setShininess(m_shininess);
    }
}

// Static registry for object factories
static QMap<QString, Geo3DObject::ObjectFactory> s_objectFactories;

void Geo3DObject::registerObjectType(const QString& typeName, ObjectFactory factory)
{
    s_objectFactories[typeName] = factory;
}

Geo3DObject* Geo3DObject::createFromJson(const QJsonObject& json)
{
    if (!json.contains("type")) {
        return nullptr;
    }

    QString objectType = json["type"].toString();

    auto it = s_objectFactories.find(objectType);
    if (it == s_objectFactories.end()) {
        return nullptr; // Unknown object type
    }

    Geo3DObject* object = it.value()(); // Call factory function
    if (object && object->fromJson(json)) {
        return object;
    } else {
        delete object;
        return nullptr;
    }
}
