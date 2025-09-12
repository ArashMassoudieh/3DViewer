/**
 * @file geo3dobjectset.h
 * @brief Header file for the Geo3DObjectSet class
 * @author Your Name
 * @date Current Date
 */

#ifndef GEO3DOBJECTSET_H
#define GEO3DOBJECTSET_H

#include <QMap>
#include <QString>
#include <QStringList>
#include <QColor>
#include <QVector3D>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Qt3DCore {
class QEntity;
}
QT_END_NAMESPACE

class Geo3DObject;

/**
 * @class Geo3DObjectSet
 * @brief A collection class for managing multiple Geo3DObject instances using string identifiers
 *
 * The Geo3DObjectSet class provides a convenient way to manage a collection of 3D objects
 * using a QMap with string keys. It offers functionality for adding, removing, and manipulating
 * multiple 3D objects as a group, with built-in Qt3D integration for rendering.
 *
 * The class automatically manages memory for the objects it contains and provides both
 * individual object access and bulk operations on all objects in the set.
 *
 * @note This class assumes ownership of the Geo3DObject pointers added to it and will
 * delete them when they are removed or when the set is destroyed.
 *
 * Example usage:
 * @code
 * Geo3DObjectSet objectSet;
 * objectSet.addObject("cylinder1", new CylinderObject());
 * objectSet.addObject("sphere1", new SphereObject());
 * objectSet.setAllDiffuseColor(QColor::red);
 * objectSet.createEntities(parentEntity);
 * @endcode
 */
class Geo3DObjectSet
{
public:
    /**
     * @brief Default constructor
     *
     * Creates an empty Geo3DObjectSet with object ownership enabled.
     */
    explicit Geo3DObjectSet();

    /**
     * @brief Destructor
     *
     * Automatically cleans up all managed objects by calling clear().
     */
    ~Geo3DObjectSet();

    // Object management

    /**
     * @brief Adds a 3D object to the set with the specified name
     *
     * If an object with the same name already exists, it will be removed first.
     * The set takes ownership of the object pointer.
     *
     * @param name Unique identifier for the object
     * @param object Pointer to the Geo3DObject to add (must not be null)
     *
     * @warning If object is null, the function returns without adding anything
     * @warning If an object with the same name exists, it will be deleted and replaced
     */
    void addObject(const QString& name, Geo3DObject* object);

    /**
     * @brief Removes an object from the set by name
     *
     * The object will be deleted if the set owns it.
     *
     * @param name Name of the object to remove
     * @return true if the object was found and removed, false otherwise
     */
    bool removeObject(const QString& name);

    /**
     * @brief Removes all objects from the set
     *
     * All managed objects will be deleted.
     */
    void clear();

    // Object access

    /**
     * @brief Retrieves an object by name
     *
     * @param name Name of the object to retrieve
     * @return Pointer to the object if found, nullptr otherwise
     */
    Geo3DObject* getObject(const QString& name) const;

    /**
     * @brief Checks if an object with the given name exists in the set
     *
     * @param name Name to check for
     * @return true if an object with this name exists, false otherwise
     */
    bool contains(const QString& name) const;

    /**
     * @brief Gets a list of all object names in the set
     *
     * @return QStringList containing all object names
     */
    QStringList getObjectNames() const;

    /**
     * @brief Gets the number of objects in the set
     *
     * @return Number of objects currently stored
     */
    int count() const;

    /**
     * @brief Checks if the set is empty
     *
     * @return true if the set contains no objects, false otherwise
     */
    bool isEmpty() const;

    // Iteration support

    /**
     * @brief Returns an iterator to the beginning of the object map
     *
     * @return Mutable iterator pointing to the first element
     */
    QMap<QString, Geo3DObject*>::iterator begin();

    /**
     * @brief Returns an iterator to the end of the object map
     *
     * @return Mutable iterator pointing past the last element
     */
    QMap<QString, Geo3DObject*>::iterator end();

    /**
     * @brief Returns a const iterator to the beginning of the object map
     *
     * @return Const iterator pointing to the first element
     */
    QMap<QString, Geo3DObject*>::const_iterator begin() const;

    /**
     * @brief Returns a const iterator to the end of the object map
     *
     * @return Const iterator pointing past the last element
     */
    QMap<QString, Geo3DObject*>::const_iterator end() const;

    /**
     * @brief Returns a const iterator to the beginning of the object map
     *
     * @return Const iterator pointing to the first element
     */
    QMap<QString, Geo3DObject*>::const_iterator constBegin() const;

    /**
     * @brief Returns a const iterator to the end of the object map
     *
     * @return Const iterator pointing past the last element
     */
    QMap<QString, Geo3DObject*>::const_iterator constEnd() const;

    // Qt3D integration

    /**
     * @brief Creates Qt3D entities for all objects in the set
     *
     * This method calls createEntity() on each object in the set, using the
     * provided parent entity. This is typically called once when setting up
     * the 3D scene for rendering.
     *
     * @param parentEntity Parent Qt3D entity under which all object entities will be created
     *
     * @warning If parentEntity is null, the function returns without creating any entities
     */
    void createEntities(Qt3DCore::QEntity* parentEntity);

    /**
     * @brief Forces an update of all object transforms
     *
     * This method triggers transform updates for all objects by temporarily
     * setting their position to their current position, which calls the
     * internal updateTransform() method.
     */
    void updateAllTransforms();

    /**
     * @brief Forces an update of all object materials
     *
     * This method triggers material updates for all objects by temporarily
     * setting their diffuse color to their current color, which calls the
     * internal updateMaterial() method.
     */
    void updateAllMaterials();

    // Visibility control

    /**
     * @brief Sets the visibility of all objects in the set
     *
     * @param visible true to make all objects visible, false to hide them
     */
    void setAllVisible(bool visible);

    /**
     * @brief Sets the visibility of a specific object
     *
     * @param name Name of the object to modify
     * @param visible true to make the object visible, false to hide it
     *
     * @note If no object with the given name exists, this function has no effect
     */
    void setObjectVisible(const QString& name, bool visible);

    // Bulk operations

    /**
     * @brief Sets the diffuse color for all objects in the set
     *
     * @param color The new diffuse color to apply to all objects
     */
    void setAllDiffuseColor(const QColor& color);

    /**
     * @brief Sets uniform scaling for all objects in the set
     *
     * @param uniformScale Scale factor to apply uniformly to all axes of all objects
     */
    void setAllScale(float uniformScale);

    /**
     * @brief Sets non-uniform scaling for all objects in the set
     *
     * @param scale Vector specifying scale factors for X, Y, and Z axes
     */
    void setAllScale(const QVector3D& scale);

    // Direct map access

    /**
     * @brief Provides direct read-only access to the internal object map
     *
     * This method is provided for advanced usage scenarios where direct
     * access to the underlying QMap is needed.
     *
     * @return Const reference to the internal QMap<QString, Geo3DObject*>
     *
     * @warning Use this method carefully as it bypasses the class's encapsulation
     */
    const QMap<QString, Geo3DObject*>& getObjectMap() const;

    // JSON Serialization

    /**
     * @brief Serializes the entire object set to JSON
     *
     * Creates a JSON object containing all objects in the set with their names
     * as keys and serialized data as values. Each object is serialized using
     * its toJson() method.
     *
     * @return QJsonObject containing the serialized object set
     */
    QJsonObject toJson() const;

    /**
     * @brief Deserializes the object set from JSON
     *
     * Clears the current set and loads objects from JSON data. Objects are
     * created based on their type field and deserialized using fromJson().
     * The JSON object keys become the object names in the set.
     *
     * @param json QJsonObject containing the serialized object set
     * @return true if deserialization was successful, false on error
     */
    bool fromJson(const QJsonObject& json);

    // File I/O

    /**
     * @brief Saves the object set to a JSON file
     *
     * Serializes the entire object set to JSON format and writes it to the
     * specified file path. The file will be created or overwritten.
     *
     * @param filePath Path to the file where the object set should be saved
     * @return true if the file was saved successfully, false on error
     */
    bool saveToFile(const QString& filePath) const;

    /**
     * @brief Loads the object set from a JSON file
     *
     * Reads a JSON file and deserializes the object set from it. This will
     * clear the current object set and replace it with the loaded objects.
     *
     * @param filePath Path to the file to load from
     * @return true if the file was loaded successfully, false on error
     */
    bool loadFromFile(const QString& filePath);
private:
    /**
     * @brief Internal storage for the 3D objects
     *
     * Maps string names to Geo3DObject pointers.
     */
    QMap<QString, Geo3DObject*> m_objects;

    /**
     * @brief Flag indicating whether this set owns the objects
     *
     * When true, objects will be deleted when removed from the set or when
     * the set is destroyed. Currently always true.
     */
    bool m_ownsObjects;
};

#endif // GEO3DOBJECTSET_H
