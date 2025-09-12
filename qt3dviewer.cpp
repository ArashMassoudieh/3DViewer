#include "qt3dviewer.h"
#include "geo3dobjectset.h"
#include "cylinderobject.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <QGuiApplication>
#include <Qt3DExtras/QForwardRenderer>

Qt3DViewer::Qt3DViewer(QWidget* parent) : QWidget(parent), m_objectSet(nullptr)
{
    setWindowTitle("Qt3D Object Set Viewer");
    setMinimumSize(800, 600);
    setupUI();
}

void Qt3DViewer::setObjectSet(Geo3DObjectSet* objectSet)
{
    m_objectSet = objectSet;
}

Geo3DObjectSet* Qt3DViewer::getObjectSet() const
{
    return m_objectSet;
}

void Qt3DViewer::showObjects()
{
    // Create Qt3D window
    Qt3DExtras::Qt3DWindow *view = new Qt3DExtras::Qt3DWindow();
    view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));

    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();

    // If no object set is provided, create a default demonstration with cylinders
    if (!m_objectSet || m_objectSet->isEmpty()) {
        // Create a demo object set with some cylinders
        Geo3DObjectSet* demoSet = new Geo3DObjectSet();

        // Create cylinder 1
        CylinderObject* cylinder1 = new CylinderObject(1.0f, 2.0f);
        cylinder1->setPosition(-2.0f, 0.0f, 0.0f);
        cylinder1->setRotation(0.0f, 0.0f, 30.0f);
        cylinder1->setDiffuseColor(QColor(102, 84, 35));  // Brown
        demoSet->addObject("cylinder1", cylinder1);

        // Create cylinder 2
        CylinderObject* cylinder2 = new CylinderObject(0.8f, 3.0f);
        cylinder2->setPosition(2.0f, 0.0f, 0.0f);
        cylinder2->setRotation(30.0f, 45.0f, 0.0f);
        cylinder2->setDiffuseColor(QColor(50, 120, 200));  // Blue
        demoSet->addObject("cylinder2", cylinder2);

        // Create cylinder 3
        CylinderObject* cylinder3 = new CylinderObject(0.6f, 1.5f);
        cylinder3->setPosition(0.0f, 0.0f, 2.0f);
        cylinder3->setRotation(90.0f, 0.0f, 0.0f);
        cylinder3->setDiffuseColor(QColor(200, 50, 50));  // Red
        demoSet->addObject("cylinder3", cylinder3);

        // Create entities for demo objects
        demoSet->createEntities(rootEntity);

        // Clean up demo set (but not the objects, as they're now owned by Qt3D)
        // Note: In a real application, you'd need better memory management
    } else {
        // Use the provided object set
        m_objectSet->createEntities(rootEntity);
    }

    // Camera
    Qt3DRender::QCamera *cameraEntity = view->camera();
    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 8.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    // Camera controller
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setCamera(cameraEntity);

    // Light
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(QVector3D(0, 0, 10));
    lightEntity->addComponent(lightTransform);

    // Set root entity
    view->setRootEntity(rootEntity);
    view->show();
}

void Qt3DViewer::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* title = new QLabel("Qt3D Object Set Viewer");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 18px; font-weight: bold; padding: 10px; background-color: lightblue;");
    layout->addWidget(title);

    QLabel* info = new QLabel(
        "This viewer displays a collection of 3D objects from a Geo3DObjectSet.\n\n"
        "Features:\n"
        "• Real-time 3D rendering of multiple objects\n"
        "• Mouse controls (orbit, zoom, pan)\n"
        "• Support for any Geo3DObject subclasses\n"
        "• Automatic demo mode if no object set is provided"
        );
    info->setWordWrap(true);
    info->setStyleSheet("padding: 15px; background-color: #f0f0f0;");
    layout->addWidget(info);

    QPushButton* showButton = new QPushButton("Show 3D Objects");
    showButton->setMinimumHeight(50);
    showButton->setStyleSheet("font-size: 14px; background-color: #4CAF50; color: white;");
    connect(showButton, &QPushButton::clicked, this, &Qt3DViewer::showObjects);
    layout->addWidget(showButton);

    QPushButton* exitButton = new QPushButton("Exit");
    connect(exitButton, &QPushButton::clicked, this, &QWidget::close);
    layout->addWidget(exitButton);

    layout->addStretch();
}
