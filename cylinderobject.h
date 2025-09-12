/**
 * @file cylinderobject.h
 * @brief Header file for the CylinderObject class
 * @author Your Name
 * @date Current Date
 */

#ifndef CYLINDEROBJECT_H
#define CYLINDEROBJECT_H

#include "geo3dobject.h"

QT_BEGIN_NAMESPACE
namespace Qt3DRender {
class QGeometryRenderer;
}
QT_END_NAMESPACE

/**
 * @class CylinderObject
 * @brief A 3D cylinder object implementation derived from Geo3DObject
 *
 * The CylinderObject class provides a concrete implementation of a 3D cylinder
 * geometry. It inherits all transform and material properties from Geo3DObject
 * and implements the cylinder-specific geometry creation.
 *
 * The cylinder is oriented along the Y-axis by default, with its center at the origin.
 * The cylinder's dimensions can be customized through radius, length (height), and
 * tessellation parameters (rings and slices) for controlling mesh quality.
 *
 * Example usage:
 * @code
 * CylinderObject* cylinder = new CylinderObject();
 * cylinder->setRadius(2.0f);
 * cylinder->setLength(4.0f);
 * cylinder->setPosition(0, 0, 0);
 * cylinder->setDiffuseColor(QColor::blue);
 * cylinder->createEntity(parentEntity);
 * @endcode
 */
class CylinderObject : public Geo3DObject
{
public:
    /**
     * @brief Default constructor
     *
     * Creates a cylinder with default parameters:
     * - Radius: 1.0
     * - Length: 2.0
     * - Rings: 50
     * - Slices: 20
     */
    explicit CylinderObject();

    /**
     * @brief Parameterized constructor
     *
     * Creates a cylinder with specified radius and length.
     *
     * @param radius The radius of the cylinder (must be positive)
     * @param length The length (height) of the cylinder (must be positive)
     */
    explicit CylinderObject(float radius, float length);

    /**
     * @brief Full parameterized constructor
     *
     * Creates a cylinder with all parameters specified.
     *
     * @param radius The radius of the cylinder (must be positive)
     * @param length The length (height) of the cylinder (must be positive)
     * @param rings Number of rings for tessellation (affects quality)
     * @param slices Number of slices for tessellation (affects quality)
     */
    explicit CylinderObject(float radius, float length, int rings, int slices);

    /**
     * @brief Virtual destructor
     */
    virtual ~CylinderObject();

    // Cylinder-specific properties

    /**
     * @brief Gets the cylinder's radius
     *
     * @return Current radius of the cylinder
     */
    float getRadius() const;

    /**
     * @brief Sets the cylinder's radius
     *
     * If the geometry has already been created, it will be recreated with the new radius.
     *
     * @param radius New radius (must be positive)
     *
     * @warning Setting a non-positive radius may result in undefined behavior
     */
    void setRadius(float radius);

    /**
     * @brief Gets the cylinder's length (height)
     *
     * @return Current length of the cylinder
     */
    float getLength() const;

    /**
     * @brief Sets the cylinder's length (height)
     *
     * If the geometry has already been created, it will be recreated with the new length.
     *
     * @param length New length (must be positive)
     *
     * @warning Setting a non-positive length may result in undefined behavior
     */
    void setLength(float length);

    /**
     * @brief Gets the number of rings used for tessellation
     *
     * @return Current number of rings
     */
    int getRings() const;

    /**
     * @brief Sets the number of rings for tessellation
     *
     * More rings provide smoother curves but increase polygon count.
     * If the geometry has already been created, it will be recreated.
     *
     * @param rings New number of rings (should be > 1 for meaningful geometry)
     *
     * @note Typical values range from 10-100 depending on quality needs
     */
    void setRings(int rings);

    /**
     * @brief Gets the number of slices used for tessellation
     *
     * @return Current number of slices
     */
    int getSlices() const;

    /**
     * @brief Sets the number of slices for tessellation
     *
     * More slices provide smoother circumference but increase polygon count.
     * If the geometry has already been created, it will be recreated.
     *
     * @param slices New number of slices (should be >= 3 for meaningful geometry)
     *
     * @note Typical values range from 8-50 depending on quality needs
     */
    void setSlices(int slices);

    /**
     * @brief Sets both radius and length simultaneously
     *
     * This is more efficient than setting them individually if both need to change,
     * as it only triggers one geometry recreation.
     *
     * @param radius New radius (must be positive)
     * @param length New length (must be positive)
     */
    void setDimensions(float radius, float length);

    /**
     * @brief Sets all tessellation parameters simultaneously
     *
     * This is more efficient than setting them individually if both need to change,
     * as it only triggers one geometry recreation.
     *
     * @param rings New number of rings
     * @param slices New number of slices
     */
    void setTessellation(int rings, int slices);

    /**
     * @brief Gets the approximate number of triangles in the mesh
     *
     * This can be useful for performance estimation.
     *
     * @return Approximate triangle count based on current tessellation settings
     */
    int getTriangleCount() const;

    // JSON Serialization
    QJsonObject toJson() const override;
    bool fromJson(const QJsonObject& json) override;
    QString getObjectType() const override;

protected:
    /**
     * @brief Creates the cylinder geometry
     *
     * Implements the pure virtual method from Geo3DObject.
     * Creates a Qt3D cylinder mesh with the current parameters.
     *
     * @return Pointer to the created QGeometryRenderer containing cylinder mesh
     */
    Qt3DRender::QGeometryRenderer* createGeometry() override;

private:
    /**
     * @brief Radius of the cylinder
     */
    float m_radius;

    /**
     * @brief Length (height) of the cylinder
     */
    float m_length;

    /**
     * @brief Number of rings for tessellation
     */
    int m_rings;

    /**
     * @brief Number of slices for tessellation
     */
    int m_slices;

    /**
     * @brief Recreates the geometry if it has already been created
     *
     * This method is called when cylinder parameters change after the
     * geometry has been created and attached to an entity.
     */
    void recreateGeometryIfNeeded();
};

#endif // CYLINDEROBJECT_H
