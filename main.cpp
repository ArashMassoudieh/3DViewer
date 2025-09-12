#include <QApplication>
#include <QDebug>
#include "qt3dviewer.h"
#include "geo3dobjectset.h"
#include "cylinderobject.h"
#include "faceobject.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Test 1: Create objects and object set
    qDebug() << "=== Test 1: Creating Objects ===";
    Geo3DObjectSet* originalSet = new Geo3DObjectSet();

    // Create cylinder 1
    CylinderObject* cyl1 = new CylinderObject(1.5f, 3.0f);
    cyl1->setPosition(-2.0f, 0.0f, 0.0f);
    cyl1->setRotation(0.0f, 0.0f, 30.0f);
    cyl1->setDiffuseColor(QColor(Qt::red));
    originalSet->addObject("redCylinder", cyl1);

    // Create cylinder 2
    CylinderObject* cyl2 = new CylinderObject(1.0f, 2.5f);
    cyl2->setPosition(2.0f, 1.0f, 0.0f);
    cyl2->setRotation(45.0f, 0.0f, 0.0f);
    cyl2->setDiffuseColor(QColor(Qt::blue));
    cyl2->setTessellation(30, 16); // Lower quality
    originalSet->addObject("blueCylinder", cyl2);

    // Create cylinder 3
    CylinderObject* cyl3 = new CylinderObject(0.8f, 4.0f);
    cyl3->setPosition(0.0f, -1.0f, 2.0f);
    cyl3->setRotation(90.0f, 45.0f, 0.0f);
    cyl3->setDiffuseColor(QColor(Qt::green));
    cyl3->setVisible(true);
    originalSet->addObject("greenCylinder", cyl3);

    // Create a rectangular face
    FaceObject* face = new FaceObject(1.0f); // elevation = 1.0
    face->addVertex(-2.0f, -2.0f);  // bottom-left
    face->addVertex(2.0f, -2.0f);   // bottom-right
    face->addVertex(2.0f, 2.0f);    // top-right
    face->addVertex(-2.0f, 2.0f);   // top-left

    // Or create with vertices directly
    QVector<QVector2D> vertices;
    vertices << QVector2D(-1, -1) << QVector2D(1, -1) << QVector2D(0, 1);
    FaceObject* triangle = new FaceObject(vertices, 0.5f); // triangular face

    // Add to object set
    originalSet->addObject("floor", face);
    originalSet->addObject("triangle", triangle);

    qDebug() << "Created object set with" << originalSet->count() << "objects";
    qDebug() << "Object names:" << originalSet->getObjectNames();

    // Test 2: Save to JSON file
    qDebug() << "\n=== Test 2: Saving to File ===";
    QString fileName = "test_scene.json";
    bool saveSuccess = originalSet->saveToFile(fileName);
    qDebug() << "Save to file" << fileName << ":" << (saveSuccess ? "SUCCESS" : "FAILED");

    // Test 3: JSON serialization test
    qDebug() << "\n=== Test 3: JSON Serialization ===";
    QJsonObject json = originalSet->toJson();
    qDebug() << "JSON object count:" << json["objectCount"].toInt();
    qDebug() << "JSON version:" << json["version"].toString();

    // Test 4: Load from JSON file
    qDebug() << "\n=== Test 4: Loading from File ===";
    Geo3DObjectSet* loadedSet = new Geo3DObjectSet();
    bool loadSuccess = loadedSet->loadFromFile(fileName);
    qDebug() << "Load from file:" << (loadSuccess ? "SUCCESS" : "FAILED");
    qDebug() << "Loaded object count:" << loadedSet->count();
    qDebug() << "Loaded object names:" << loadedSet->getObjectNames();

    // Test 5: Verify loaded objects
    qDebug() << "\n=== Test 5: Verifying Loaded Objects ===";
    if (loadSuccess) {
        Geo3DObject* loadedCyl1 = loadedSet->getObject("redCylinder");
        if (loadedCyl1) {
            qDebug() << "Red cylinder position:" << loadedCyl1->getPosition();
            qDebug() << "Red cylinder color:" << loadedCyl1->getDiffuseColor();
            qDebug() << "Red cylinder type:" << loadedCyl1->getObjectType();
        }

        // Test cylinder-specific properties
        CylinderObject* loadedCylinder = dynamic_cast<CylinderObject*>(loadedSet->getObject("blueCylinder"));
        if (loadedCylinder) {
            qDebug() << "Blue cylinder radius:" << loadedCylinder->getRadius();
            qDebug() << "Blue cylinder length:" << loadedCylinder->getLength();
            qDebug() << "Blue cylinder rings:" << loadedCylinder->getRings();
            qDebug() << "Blue cylinder slices:" << loadedCylinder->getSlices();
        }
    }

    // Test 6: Show in 3D viewer
    qDebug() << "\n=== Test 6: 3D Visualization ===";
    Qt3DViewer viewer;
    viewer.setObjectSet(loadedSet); // Use loaded set to verify it works
    viewer.show();

    qDebug() << "3D Viewer opened. Click 'Show 3D Objects' to see the loaded scene.";
    qDebug() << "You should see 3 cylinders with different colors, positions, and orientations.";



    return app.exec();
}
