#include "baseresourceeditor.h"
#include "reslibdirmodel.h"
#include "stdpaneleditor.h"
#include "respanel.h"
#include "reslib.h"
#include "rsrescore.h"

//Q_IMPORT_PLUGIN(BaseResourceEditor)

BaseResourceEditor::BaseResourceEditor() :
    ResourceEditorInterface()
{

}

BaseResourceEditor::~BaseResourceEditor()
{

}

QList<qint16> BaseResourceEditor::resTypes() const
{
    return QList<qint16>() << RES_PANEL;
}

BaseEditorWindow *BaseResourceEditor::editor(const qint16 &Type, const QString &name, ResLib *pResLib)
{
    BaseEditorWindow *wnd = nullptr;
    if (Type == RES_PANEL)
    {
        wnd = new StdPanelEditor();
        wnd->setWindowIcon(RsResCore::inst()->iconFromResType(Type));
        wnd->setupEditor();

        ResPanel *pan = new ResPanel(wnd);
        pan->load(name, pResLib);
        qobject_cast<StdPanelEditor*>(wnd)->setPanel(pan);

        wnd->setWindowTitle(QString("%1 [%2]: %3")
                            .arg(RsResCore::inst()->typeNameFromResType(Type))
                            .arg(name)
                            .arg(pan->title()));
    }
    return wnd;
}
