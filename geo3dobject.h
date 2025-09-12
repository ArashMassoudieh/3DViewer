#ifndef GEO3DOBJECT_H
#define GEO3DOBJECT_H

#include <QVector3D>
#include <QColor>
#include <QJsonObject>
#include <functional>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Qt3DCore {
class QEntity;
class QTransform;
}
namespace Qt3DRender {
class QGeometryRenderer;
}
namespace Qt3DExtras {
class QPhongMaterial;
}
QT_END_NAMESPACE

class Geo3DObject
{
public:
    explicit Geo3DObject();
    virtual ~Geo3DObject();

    // Transform properties
    QVector3D getPosition() const;
    void setPosition(const QVector3D& position);
    void setPosition(float x, float y, float z);

    QVector3D getRotation() const;
    void setRotation(const QVector3D& rotation);
    void setRotation(float x, float y, float z);

    QVector3D getScale() const;
    void setScale(const QVector3D& scale);
    void setScale(float uniformScale);
    void setScale(float x, float y, float z);

    // Material properties
    QColor getDiffuseColor() const;
    void setDiffuseColor(const QColor& color);

    QColor getAmbientColor() const;
    void setAmbientColor(const QColor& color);

    QColor getSpecularColor() const;
    void setSpecularColor(const QColor& color);

    float getShininess() const;
    void setShininess(float shininess);

    // Qt3D Entity creation
    virtual Qt3DCore::QEntity* createEntity(Qt3DCore::QEntity* parent = nullptr);

    // Visibility
    bool isVisible() const;
    void setVisible(bool visible);

    // JSON Serialization
    virtual QJsonObject toJson() const = 0;
    virtual bool fromJson(const QJsonObject& json) = 0;
    virtual QString getObjectType() const = 0;

    /**
     * @brief Creates a Geo3DObject from JSON data
     *
     * Factory method that creates the appropriate object type based on the
     * "type" field in the JSON data and deserializes it.
     *
     * @param json QJsonObject containing the serialized object data
     * @return Pointer to the created object, or nullptr if creation failed
     */
    static Geo3DObject* createFromJson(const QJsonObject& json);

    // Factory function type
    typedef std::function<Geo3DObject*()> ObjectFactory;

    /**
     * @brief Registers a factory function for an object type
     *
     * @param typeName Type name identifier (e.g., "Cylinder")
     * @param factory Factory function that creates the object
     */
    static void registerObjectType(const QString& typeName, ObjectFactory factory);

protected:
    // Pure virtual method for creating geometry - must be implemented by derived classes
    virtual Qt3DRender::QGeometryRenderer* createGeometry() = 0;

    // Update methods - called when properties change
    virtual void updateTransform();
    virtual void updateMaterial();

private:
    // Transform data
    QVector3D m_position;
    QVector3D m_rotation;
    QVector3D m_scale;

    // Material data
    QColor m_diffuseColor;
    QColor m_ambientColor;
    QColor m_specularColor;
    float m_shininess;

    bool m_visible;

    // Qt3D components (created when needed)
    Qt3DCore::QEntity* m_entity;
    Qt3DCore::QTransform* m_transform;
    Qt3DExtras::QPhongMaterial* m_material;
    Qt3DRender::QGeometryRenderer* m_geometryRenderer;
};

#endif // GEO3DOBJECT_H
