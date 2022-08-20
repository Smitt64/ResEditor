#ifndef BASEEDITORVIEW_H
#define BASEEDITORVIEW_H

#include "rsrescore_global.h"
#include <QGraphicsView>

class BaseScene;
class RSRESCORE_EXPORT BaseEditorView : public QGraphicsView
{
    Q_OBJECT
public:
    BaseEditorView(QWidget *parent = nullptr);
    virtual ~BaseEditorView();

    virtual void setupScene();

    QSize gridSize() const;

protected:
    void setScene(BaseScene *scene);
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    BaseScene *pScene;
};

#endif // BASEEDITORVIEW_H
