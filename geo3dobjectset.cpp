#include "geo3dobjectset.h"
#include "geo3dobject.h"

#include <Qt3DCore/QEntity>
#include <QJsonDocument>
#include <QFile>
#include <QIODevice>


Geo3DObjectSet::Geo3DObjectSet()
    : m_ownsObjects(true)
{
}

Geo3DObjectSet::~Geo3DObjectSet()
{
    clear();
}

void Geo3DObjectSet::addObject(const QString& name, Geo3DObject* object)
{
    if (!object) {
        return;
    }

    // If an object with this name already exists, remove it first
    if (m_objects.contains(name)) {
        removeObject(name);
    }

    m_objects.insert(name, object);
}

bool Geo3DObjectSet::removeObject(const QString& name)
{
    auto it = m_objects.find(name);
    if (it != m_objects.end()) {
        if (m_ownsObjects && it.value()) {
            delete it.value();
        }
        m_objects.erase(it);
        return true;
    }
    return false;
}

void Geo3DObjectSet::clear()
{
    if (m_ownsObjects) {
        for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
            delete it.value();
        }
    }
    m_objects.clear();
}

Geo3DObject* Geo3DObjectSet::getObject(const QString& name) const
{
    auto it = m_objects.find(name);
    return (it != m_objects.end()) ? it.value() : nullptr;
}

bool Geo3DObjectSet::contains(const QString& name) const
{
    return m_objects.contains(name);
}

QStringList Geo3DObjectSet::getObjectNames() const
{
    return m_objects.keys();
}

int Geo3DObjectSet::count() const
{
    return m_objects.size();
}

bool Geo3DObjectSet::isEmpty() const
{
    return m_objects.isEmpty();
}

QMap<QString, Geo3DObject*>::iterator Geo3DObjectSet::begin()
{
    return m_objects.begin();
}

QMap<QString, Geo3DObject*>::iterator Geo3DObjectSet::end()
{
    return m_objects.end();
}

QMap<QString, Geo3DObject*>::const_iterator Geo3DObjectSet::begin() const
{
    return m_objects.begin();
}

QMap<QString, Geo3DObject*>::const_iterator Geo3DObjectSet::end() const
{
    return m_objects.end();
}

QMap<QString, Geo3DObject*>::const_iterator Geo3DObjectSet::constBegin() const
{
    return m_objects.constBegin();
}

QMap<QString, Geo3DObject*>::const_iterator Geo3DObjectSet::constEnd() const
{
    return m_objects.constEnd();
}

void Geo3DObjectSet::createEntities(Qt3DCore::QEntity* parentEntity)
{
    if (!parentEntity) {
        return;
    }

    for (auto it = m_objects.constBegin(); it != m_objects.constEnd(); ++it) {
        if (it.value()) {
            it.value()->createEntity(parentEntity);
        }
    }
}

void Geo3DObjectSet::updateAllTransforms()
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it.value()) {
            // This would trigger the transform update in the object
            // Since updateTransform is protected, we'll call it indirectly by setting position
            QVector3D currentPos = it.value()->getPosition();
            it.value()->setPosition(currentPos);
        }
    }
}

void Geo3DObjectSet::updateAllMaterials()
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it.value()) {
            // This would trigger the material update in the object
            // Since updateMaterial is protected, we'll call it indirectly by setting a material property
            QColor currentColor = it.value()->getDiffuseColor();
            it.value()->setDiffuseColor(currentColor);
        }
    }
}

void Geo3DObjectSet::setAllVisible(bool visible)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it.value()) {
            it.value()->setVisible(visible);
        }
    }
}

void Geo3DObjectSet::setObjectVisible(const QString& name, bool visible)
{
    Geo3DObject* obj = getObject(name);
    if (obj) {
        obj->setVisible(visible);
    }
}

void Geo3DObjectSet::setAllDiffuseColor(const QColor& color)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it.value()) {
            it.value()->setDiffuseColor(color);
        }
    }
}

void Geo3DObjectSet::setAllScale(float uniformScale)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it.value()) {
            it.value()->setScale(uniformScale);
        }
    }
}

void Geo3DObjectSet::setAllScale(const QVector3D& scale)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it) {
        if (it.value()) {
            it.value()->setScale(scale);
        }
    }
}

const QMap<QString, Geo3DObject*>& Geo3DObjectSet::getObjectMap() const
{
    return m_objects;
}

QJsonObject Geo3DObjectSet::toJson() const
{
    QJsonObject json;
    json["version"] = "1.0";
    json["objectCount"] = m_objects.size();

    QJsonObject objectsJson;
    for (auto it = m_objects.constBegin(); it != m_objects.constEnd(); ++it) {
        if (it.value()) {
            objectsJson[it.key()] = it.value()->toJson();
        }
    }

    json["objects"] = objectsJson;
    return json;
}

bool Geo3DObjectSet::fromJson(const QJsonObject& json)
{
    // Clear existing objects
    clear();

    // Check version (for future compatibility)
    if (!json.contains("version")) {
        return false;
    }

    if (!json.contains("objects") || !json["objects"].isObject()) {
        return false;
    }

    QJsonObject objectsJson = json["objects"].toObject();
    for (auto it = objectsJson.begin(); it != objectsJson.end(); ++it) {
        QString name = it.key();

        if (!it.value().isObject()) {
            continue;
        }

        QJsonObject data = it.value().toObject();

        // Use factory method to create object
        Geo3DObject* object = Geo3DObject::createFromJson(data);
        if (object) {
            addObject(name, object);
        }
    }

    return true;
}

bool Geo3DObjectSet::saveToFile(const QString& filePath) const
{
    QJsonObject json = toJson();
    QJsonDocument doc(json);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Could not open file for writing:" << filePath;
        return false;
    }

    qint64 bytesWritten = file.write(doc.toJson());
    file.close();

    if (bytesWritten == -1) {
        qWarning() << "Error writing to file:" << filePath;
        return false;
    }

    return true;
}

bool Geo3DObjectSet::loadFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open file for reading:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << error.errorString();
        return false;
    }

    return fromJson(doc.object());
}
