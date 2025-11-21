#include <QApplication>
#include <QDebug>
#include "qt3dviewer.h"
#include "geo3dobjectset.h"
#include "cylinderobject.h"
#include "tubeobject.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "=== Creating Cylinder and Tube Scene ===";

    Geo3DObjectSet* scene = new Geo3DObjectSet();

    // Create upper grey cylinder
    // Top elevation = 0, Bottom elevation = -7
    // Height = 7, so center is at y = -3.5
    CylinderObject* upperCylinder = new CylinderObject(1.0f, 7.0f);  // radius=1.0, height=7.0
    upperCylinder->setPosition(0.0f, -3.5f, 0.0f);  // Center at -3.5 to get top at 0, bottom at -7
    upperCylinder->setDiffuseColor(QColor(128, 128, 128));  // Grey
    upperCylinder->setAmbientColor(QColor(64, 64, 64));     // Darker grey
    upperCylinder->setOpacity(0.5f);
    scene->addObject("upperCylinder", upperCylinder);

    qDebug() << "Created upper grey cylinder:";
    qDebug() << "  - Radius: 1.0m";
    qDebug() << "  - Top elevation: 0m";
    qDebug() << "  - Bottom elevation: -7m";
    qDebug() << "  - Opacity: 0.5";

    // Create lower cylinder
    // Top elevation = -7, Bottom elevation = -21
    // Height = 14, so center is at y = -14
    CylinderObject* lowerCylinder = new CylinderObject(1.0f, 14.0f);  // radius=1.0, height=14.0
    lowerCylinder->setPosition(0.0f, -14.0f, 0.0f);  // Center at -14 to get top at -7, bottom at -21
    lowerCylinder->setDiffuseColor(QColor(100, 100, 120));  // Slightly bluish grey
    lowerCylinder->setAmbientColor(QColor(50, 50, 60));     // Darker bluish grey
    lowerCylinder->setOpacity(0.5f);
    scene->addObject("lowerCylinder", lowerCylinder);

    qDebug() << "Created lower cylinder:";
    qDebug() << "  - Radius: 1.0m";
    qDebug() << "  - Top elevation: -7m";
    qDebug() << "  - Bottom elevation: -21m";
    qDebug() << "  - Opacity: 0.5";

    // Create upper surrounding tube (soil layer 1)
    // Height = 7, from 0 to -7m
    TubeObject* upperTube = new TubeObject(1.0f, 12.0f, 7.0f);  // inner=1.0, outer=12.0, height=7.0
    upperTube->setPosition(0.0f, -3.5f, 0.0f);  // Same center as upper cylinder
    upperTube->setDiffuseColor(QColor(139, 90, 43));   // Soil brown color (SaddleBrown)
    upperTube->setAmbientColor(QColor(90, 60, 30));    // Darker brown
    upperTube->setOpacity(0.2f);
    upperTube->setTessellation(20, 48);  // Good quality for large tube
    scene->addObject("upperTube", upperTube);

    qDebug() << "Created upper soil tube:";
    qDebug() << "  - Inner radius: 1.0m";
    qDebug() << "  - Outer radius: 12.0m";
    qDebug() << "  - Height: 7.0m (0 to -7m)";
    qDebug() << "  - Color: Soil brown";
    qDebug() << "  - Opacity: 0.2";

    // Create lower soil as full cylinder (transparent so inner cylinder is visible)
    // Top elevation = -7, Bottom elevation = -80
    // Height = 73, so center is at y = -43.5
    CylinderObject* lowerSoil = new CylinderObject(12.0f, 73.0f);  // radius=12.0, height=73.0
    lowerSoil->setPosition(0.0f, -43.5f, 0.0f);  // Center at -43.5 to get top at -7, bottom at -80
    lowerSoil->setDiffuseColor(QColor(120, 80, 50));   // Reddish-brown soil
    lowerSoil->setAmbientColor(QColor(80, 50, 30));    // Darker reddish brown
    lowerSoil->setOpacity(0.2f);  // Transparent so you can see the inner cylinder
    lowerSoil->setTessellation(20, 48);  // Good quality
    scene->addObject("lowerSoil", lowerSoil);

    qDebug() << "Created lower soil cylinder:";
    qDebug() << "  - Radius: 12.0m (full cylinder)";
    qDebug() << "  - Height: 73.0m (-7 to -80m)";
    qDebug() << "  - Color: Reddish-brown soil";
    qDebug() << "  - Opacity: 0.2 (transparent to see inner cylinder)";

    // Save scene to file
    qDebug() << "\n=== Saving Scene to File ===";
    QString fileName = "cylinder_tube_scene.json";
    bool saveSuccess = scene->saveToFile(fileName);
    qDebug() << "Save to file" << fileName << ":" << (saveSuccess ? "SUCCESS" : "FAILED");

    // Create viewer and display
    qDebug() << "\n=== Opening 3D Viewer ===";
    Qt3DViewer viewer;
    viewer.setObjectSet(scene);
    viewer.show();

    qDebug() << "\nScene ready! Click 'Show 3D Objects' to visualize.";
    qDebug() << "You should see:";
    qDebug() << "  - An upper semi-transparent grey cylinder (0 to -7m)";
    qDebug() << "  - A lower semi-transparent bluish-grey cylinder (-7 to -21m)";
    qDebug() << "  - An upper brown tube surrounding upper cylinder (0 to -7m)";
    qDebug() << "  - A lower transparent reddish-brown soil cylinder (-7 to -80m)";
    qDebug() << "  - The 1.0m cylinder should be visible through the transparent soil";
    qDebug() << "Use mouse to orbit, zoom, and pan the view.";

    return app.exec();
}
