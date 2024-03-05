#include "textitem.h"
#include <QColor>
#include "containeritem.h"
#include "controlitem.h"
#include "qgraphicsscene.h"
#include "qlineedit.h"
#include "respanel.h"
#include "styles/resstyle.h"
#include "undoredo/undoitemmove.h"
#include "undoredo/undopropertychange.h"
#include "undoredo/undoitemresize.h"
#include "rsrescore.h"
#include "basescene.h"
#include "widgets/labeltexteditdlg.h"
#include <QPainter>
#include <QFont>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QUndoStack>
#include <QGraphicsView>

#define CHECK_STR "[ ]"
#define RADIO_STR "( )"

TextItem::TextItem(QGraphicsItem* parent) :
    CustomRectItem(parent),
    m_Type(TypeLabel),
    m_CheckRadioPos(-1)
{
    m_TextStyle = new EwTextStyle(this);

    setBrush(QBrush(Qt::darkBlue));
    setAvailableCorners(0);
    setZValue(100.0);
}

TextItem::~TextItem()
{

}

void TextItem::setTextStruct(TextStruct *value)
{
    m_Value = value->value;
    m_TextStyle = EwTextStyle::fromStyle(value->_text->St);
    detectType();

    setCoord(QPoint(value->x(), value->y()));
    setSize(QSize(value->len(), 1));
}

void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ResStyleOption opt;
    opt.init(this);
    opt.text = m_Value;

    style()->drawControl(ResStyle::Control_Label, painter, &opt);
    CustomRectItem::paint(painter, option, widget);
    drawIntersects(painter);

    painter->save();
    QColor br = style()->color(ResStyle::Color_TextBg, &opt);
    paintBevel(painter, QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue()));
    painter->restore();
}

const TextItem::TextItemType &TextItem::textType() const
{
    return m_Type;
}

void TextItem::detectType(const bool &fUpdateCorners)
{
    m_CheckRadioPos = m_Value.indexOf("[ ]");

    if (m_CheckRadioPos != -1)
        m_Type = TypeCheck;

    if (m_Type != TypeCheck)
    {
        m_CheckRadioPos = m_Value.indexOf("( )");

        if (m_CheckRadioPos != -1)
            m_Type = TypeRadio;
    }

    if (m_Type == TypeLabel)
    {
        if (m_Value.count(TEXTITEM_HORIZONTAL) == m_Value.size())
            m_Type = TypeHorizontal;
    }

    if (m_Type == TypeLabel)
    {
        if (m_Value.back() == TEXTITEM_HORIZONTAL)
            m_Type = TypeTextHorizontal;
    }

    if (fUpdateCorners)
    {
        if (IsIn(m_Type, 4, TypeCheck, TypeRadio, TypeHorizontal, TypeTextHorizontal))
            setAvailableCorners(RIGHT);
        else
            setAvailableCorners(RIGHT, false);
    }
}

bool TextItem::canResize(const QRectF &newRect, const ResizeCorners &corner) const
{
    bool fResize = CustomRectItem::canResize(newRect, corner);

    if (fResize)
    {
        if (m_Type == TypeCheck || m_Type == TypeRadio)
        {
            QSize gridSize = style()->gridSize();
            QString tmp = m_Value;

            if (m_CheckRadioPos == 0)
                tmp = tmp.remove(m_CheckRadioPos, 3);
            else
                tmp = tmp.mid(0, m_CheckRadioPos);

            tmp = tmp.trimmed();

            int newWidth = round(newRect.width() / gridSize.width());
            QString newText = checkRadioStr(m_Type) + QString(1, QChar::Space) + tmp;

            if (newWidth < newText.length())
                fResize = false;
        }
        else if (m_Type == TypeTextHorizontal)
        {
            QSize gridSize = style()->gridSize();
            int newWidth = round(newRect.width() / gridSize.width());
            int pos = findLastNonHorline() + 1;

            if (newWidth < pos + 1)
                fResize = false;
        }
    }

    return fResize;
}

bool TextItem::isIntersects(const QRectF &thisBound, QGraphicsItem *item, const QRectF &itemBound) const
{
    ControlItem *pIsControl = dynamic_cast<ControlItem*>(item);
    ContainerItem *pIsContainer = dynamic_cast<ContainerItem*>(item);

    if (pIsControl)
    {
        if (IsIn(pIsControl->fieldType(), 2, ControlItem::FBT, ControlItem::FVT) &&
            IsIn(pIsControl->dataType(), 2, ControlItem::CHAR, ControlItem::UCHAR)
            && pIsControl->uuid() == attachedControl())
        {
            return false;
        }
    }
    else if (pIsContainer)
        return false;

    return true;
}

QString TextItem::checkRadioStr(const TextItemType &type) const
{
    QString value;

    switch(type)
    {
    case TypeCheck:
        value = CHECK_STR;
        break;
    case TypeRadio:
        value = RADIO_STR;
        break;
    case TypeLabel:
    case TypeHorizontal:
        break;
    case TypeTextHorizontal:
        break;
    }

    return value;
}

void TextItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    CustomRectItem::mousePressEvent(event);

    if (isResizing())
        m_SaveValue = m_Value;

    m_AttachedControl = attachedControl();
    if (!m_AttachedControl.isNull())
    {
        BaseScene* customScene = qobject_cast<BaseScene*> (scene());
        ControlItem *rect = qobject_cast<ControlItem*>(customScene->findItem(m_AttachedControl));

        if (rect)
            m_AttachedPos = rect->pos();
    }
}

void TextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    CustomRectItem::mouseMoveEvent(event);

    if (isResizing())
    {
        if (m_Type == TypeCheck || m_Type == TypeRadio)
        {
            QRectF rect = boundingRect();

            QSize gridSize = style()->gridSize();
            QString tmp = m_Value;

            detectType(false);
            if (m_CheckRadioPos == 0)
                tmp = tmp.remove(m_CheckRadioPos, 3);
            else
                tmp = tmp.mid(0, m_CheckRadioPos);

            tmp = tmp.trimmed();

            int newWidth = round(rect.width() / gridSize.width());
            int spaceLen = abs(newWidth - tmp.length()) - 3;

            if (m_CheckRadioPos == 0)
            {
                QString newText = checkRadioStr(m_Type) + QString(spaceLen, QChar::Space) + tmp;
                m_Value = newText;
            }
            else
            {
                QString newText = tmp + QString(spaceLen, QChar::Space) + checkRadioStr(m_Type);
                m_Value = newText;
            }
        }
        else if (m_Type == TypeHorizontal)
        {
            QRectF rect = boundingRect();

            QSize gridSize = style()->gridSize();
            int newWidth = round(rect.width() / gridSize.width());

            m_Value = QString(newWidth, *QString(TEXTITEM_HORIZONTAL).begin());

            qDebug() << "isNonCharacter" << QChar(*QString(TEXTITEM_HORIZONTAL).begin()).isNonCharacter();
        }
        else if (m_Type == TypeTextHorizontal)
        {
            QRectF rect = boundingRect();
            int pos = findLastNonHorline() + 1;
            QSize gridSize = style()->gridSize();
            int newWidth = round(rect.width() / gridSize.width());

            QString textPart = m_Value.mid(0, pos);
            m_Value = textPart + QString(newWidth - textPart.size(), *QString(TEXTITEM_HORIZONTAL).begin());
        }
    }

    if (!m_AttachedControl.isNull())
    {
        BaseScene* customScene = qobject_cast<BaseScene*> (scene());
        CustomRectItem *attachedRect = customScene->findItem(m_AttachedControl);

        if (attachedRect && !attachedRect->isSelected())
        {
            int offset = attachedControlOffset();
            BaseScene* customScene = qobject_cast<BaseScene*> (scene());
            QSize gridSize = customScene->getGridSize();

            QPointF thisPos = pos();
            thisPos.setX(thisPos.x() + (offset + 1) * gridSize.width());

            attachedRect->setPos(thisPos);

            attachedRect->update();
            customScene->update();
        }
    }
}

int TextItem::findLastNonHorline(const QString &pattern) const
{
    int pos = -1;

    for (int i = 0; i < m_Value.size(); i++)
    {
        if (QString(m_Value[i]) != pattern)
            pos = i;
    }
    return pos;
}

void TextItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());

    if (isResizing())
    {
        QRectF actual = actualRect();
        QRectF bound = boundingRect();

        QPointF dPos = bound.topLeft() - actual.topLeft();
        moveBy(dPos.rx(), dPos.ry());
        QSizeF vsize = bound.size();

        bound.setTopLeft(QPointF(0, 0));
        bound.setSize(vsize);
        setBoundingRect(bound);

        UndoItemResize *undocmd = new UndoItemResize(customScene, uuid());
        UndoPropertyChange *undoprop = new UndoPropertyChange(customScene, uuid());\

        undocmd->setSizes(actual.size(), bound.size());
        undoprop->setPropertyName("text");
        undoprop->setValues(m_SaveValue, m_Value);

        undoStack()->beginMacro(undocmd->text());
        undoStack()->push(undocmd);
        undoStack()->push(undoprop);

        if (!m_AttachedControl.isNull())
        {
            CustomRectItem *attachedRect = customScene->findItem(m_AttachedControl);
            UndoItemMove *pAttachMove = new UndoItemMove(customScene, m_AttachedControl);
            pAttachMove->setPositions(m_AttachedPos, attachedRect->pos());
            undoStack()->push(pAttachMove);
        }
        undoStack()->endMacro();
    }
    else
        insertUndoRedoMove();

    m_AttachedControl = QUuid();
    afterReleaseMouse(event);
}

void TextItem::onInsertUndoRedoMove(const QMap<CustomRectItem *, QPointF> &MousePressPoint)
{
    if (!m_AttachedControl.isNull())
    {
        BaseScene* customScene = qobject_cast<BaseScene*> (scene());
        CustomRectItem *attachedRect = customScene->findItem(m_AttachedControl);
        UndoItemMove *pAttachMove = new UndoItemMove(customScene, m_AttachedControl);
        pAttachMove->setPositions(m_AttachedPos, attachedRect->pos());
        undoStack()->push(pAttachMove);
    }

    CustomRectItem::onInsertUndoRedoMove(MousePressPoint);
}

QString TextItem::text() const
{
    /*QUuid control = attachedControl();

    if (IsIn(m_Type, 2, TypeCheck, TypeRadio) && !control.isNull())
    {
        QString onlytext = m_Value;
        onlytext = onlytext.remove("[ ]").remove("( )");

        return onlytext.trimmed();
    }*/

    return m_Value;
}

void TextItem::setText(const QString &txt)
{
    checkPropSame("text", txt);

    QRect sz = geometry();
    sz.setWidth(txt.length());

    if (isSkipUndoStack() || !undoStack())
    {
        QUuid control = attachedControl();
        int offset = attachedControlOffset();

        m_Value = txt.trimmed();

        detectType();
        setGeometry(sz);

        if (IsIn(m_Type, 2, TypeCheck, TypeRadio) && !control.isNull())
        {
            BaseScene* customScene = qobject_cast<BaseScene*> (scene());

            int newOffset = attachedControlOffset();
            int offsetDiff = newOffset - offset;
            CustomRectItem *attachedItem = customScene->findItem(control);

            QPoint pt = attachedItem->getPoint();
            attachedItem->setCoord(QPoint(pt.x() + offsetDiff, pt.y()));
        }

        emit textChanged();
        emit geometryChanged();

        update();
        scene()->update();
    }
    else
    {
        QRect sz = geometry();
        sz.setWidth(txt.length());
        QString msg = UndoPropertyChange::ChangePropertyMsg("text", metaObject());

        undoStack()->beginMacro(msg);
        pushUndoPropertyData("text", txt);
        pushUndoPropertyData("geometry", sz);
        undoStack()->endMacro();
    }
}

EwTextStyle TextItem::textStyle()
{
    return m_TextStyle;
}

void TextItem::setTextStyle(const EwTextStyle &style)
{
    checkPropSame("textStyle", QVariant::fromValue(style));

    if (isSkipUndoStack() || !undoStack())
    {
        m_TextStyle = style;
        emit textStyleChanged();

        update();
        scene()->update();
    }
    else
    {
        pushUndoPropertyData("textStyle", QVariant::fromValue(style));
    }
}

int TextItem::attachedControlOffset() const
{
    if (!IsIn(m_Type, 2, TypeCheck, TypeRadio))
        return std::numeric_limits<int>::infinity();

    QString bracer = "[ ]";
    if (m_Type == TypeRadio)
        bracer = "( )";

    int pos = m_Value.indexOf(bracer);
    if (pos < 0)
        return std::numeric_limits<int>::infinity();

    return pos;
}

QUuid TextItem::attachedControl() const
{
    if (IsIn(m_Type, 2, TypeCheck, TypeRadio))
    {
        QSize gridSize = style()->gridSize();
        BaseScene* customScene = qobject_cast<BaseScene*> (scene());
        int offset = attachedControlOffset();

        QRectF rc;
        rc.setX((offset + 1) * gridSize.width());
        rc.setY(0.0);
        rc.setWidth(gridSize.width());
        rc.setHeight(gridSize.height());

        rc = mapRectToScene(rc);

        QList<ControlItem*> result = customScene->findItemsIntersect<ControlItem>(rc);
        if (!result.empty())
        {
            ControlItem *rectItem = result.first();

            if (IsIn(rectItem->fieldType(), 2, ControlItem::FBT, ControlItem::FVT) && IsIn(rectItem->dataLength(), 2, 0, 1))
                return result.first()->uuid();
        }
    }
    return QUuid();
}

QVariant TextItem::userAction(const qint32 &action, const QVariant &param)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    if (action == ActionKeyEnter)
    {
        LabelTextEditDlg dlg(customScene->views().first());
        dlg.editor()->setText(text());

        if (dlg.exec() == QDialog::Accepted)
            setText(dlg.editor()->text());
    }

    return QVariant();
}
