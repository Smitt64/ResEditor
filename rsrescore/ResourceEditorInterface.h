#ifndef RESOURCEEDITORINTERFACE_H
#define RESOURCEEDITORINTERFACE_H

#include <QtPlugin>

class ResLib;
class BaseEditorWindow;
class ResourceEditorInterface
{
public:
    virtual ~ResourceEditorInterface() = default;
    virtual QList<qint16> resTypes() const = 0;

    virtual BaseEditorWindow *editor(const qint16 &Type, const QString &name, ResLib *pResLib) = 0;
};

#define ResourceEditorInterface_iid "org.rstyle.ResourceEditorInterface"
Q_DECLARE_INTERFACE(ResourceEditorInterface, ResourceEditorInterface_iid)

#endif // RESOURCEEDITORINTERFACE_H
