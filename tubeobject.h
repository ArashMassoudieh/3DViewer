/**
 * @file tubeobject.h
 * @brief Header file for the TubeObject class
 */

#ifndef TUBEOBJECT_H
#define TUBEOBJECT_H

#include "geo3dobject.h"

QT_BEGIN_NAMESPACE
namespace Qt3DRender {
class QGeometryRenderer;
}
QT_END_NAMESPACE

/**
 * @class TubeObject
 * @brief A 3D hollow cylinder (tube) object with inner and outer radii
 *
 * The TubeObject class creates a tube/pipe shape - essentially a cylinder
 * with another cylinder removed from its center. It's defined by inner radius,
 * outer radius, and height.
 */
class TubeObject : public Geo3DObject
{
public:
    /**
     * @brief Default constructor
     * Creates a tube with default parameters
     */
    explicit TubeObject();

    /**
     * @brief Parameterized constructor
     * @param innerRadius Inner radius of the tube
     * @param outerRadius Outer radius of the tube
     * @param height Height of the tube
     */
    explicit TubeObject(float innerRadius, float outerRadius, float height);

    /**
     * @brief Full parameterized constructor
     * @param innerRadius Inner radius of the tube
     * @param outerRadius Outer radius of the tube
     * @param height Height of the tube
     * @param rings Number of rings for tessellation
     * @param slices Number of slices for tessellation
     */
    explicit TubeObject(float innerRadius, float outerRadius, float height, int rings, int slices);

    virtual ~TubeObject();

    // Tube-specific properties
    float getInnerRadius() const;
    void setInnerRadius(float radius);

    float getOuterRadius() const;
    void setOuterRadius(float radius);

    float getHeight() const;
    void setHeight(float height);

    int getRings() const;
    void setRings(int rings);

    int getSlices() const;
    void setSlices(int slices);

    void setDimensions(float innerRadius, float outerRadius, float height);
    void setTessellation(int rings, int slices);

    int getTriangleCount() const;

    // JSON Serialization
    QJsonObject toJson() const override;
    bool fromJson(const QJsonObject& json) override;
    QString getObjectType() const override;

protected:
    Qt3DRender::QGeometryRenderer* createGeometry() override;

private:
    float m_innerRadius;
    float m_outerRadius;
    float m_height;
    int m_rings;
    int m_slices;

    void recreateGeometryIfNeeded();
};

#endif // TUBEOBJECT_H
