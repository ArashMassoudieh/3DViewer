#ifndef QT3DVIEWER_H
#define QT3DVIEWER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

class Geo3DObjectSet;

class Qt3DViewer : public QWidget
{
    Q_OBJECT

public:
    explicit Qt3DViewer(QWidget* parent = nullptr);

    /**
     * @brief Sets the object set to be rendered
     * @param objectSet Pointer to the Geo3DObjectSet to display
     * @note The viewer does not take ownership of the object set
     */
    void setObjectSet(Geo3DObjectSet* objectSet);

    /**
     * @brief Gets the currently assigned object set
     * @return Pointer to the current object set, or nullptr if none is set
     */
    Geo3DObjectSet* getObjectSet() const;

private slots:
    void showObjects();

private:
    void setupUI();

    Geo3DObjectSet* m_objectSet;
};

#endif // QT3DVIEWER_H
