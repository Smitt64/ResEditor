#include "toolboxtreeview.h"
#include "qmimedata.h"
#include "toolbox/toolboxmodel.h"
#include "baseeditorwindow.h"
#include <QMouseEvent>
#include <QDrag>
#include <QGuiApplication>
#include <QIcon>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QStyleOptionToolBox>
#include <QPainter>
#include <QPainterPath>
#include <QFontDatabase>

class ToolBoxDelegate : public QStyledItemDelegate
{
public:
    ToolBoxDelegate(QObject *parent = nullptr) :
        QStyledItemDelegate(parent),
        m_hoveredRow(-1)
    {
    }

    virtual ~ToolBoxDelegate()
    {
    }

    // Обновляем hoveredRow при движении мыши (нужно вызывать извне)
    void setHoveredRow(int row) {
        m_hoveredRow = row;
    }

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &opt, const QModelIndex &index) const Q_DECL_FINAL
    {
        if (!index.parent().isValid()) // Это элемент верхнего уровня (группа)
        {

            QString groupName = index.data(Qt::DisplayRole).toString();

            // Проверяем, наведен ли курсор мыши на эту строку
            bool isHovered = opt.state & QStyle::State_MouseOver ||
                             opt.state & QStyle::QStyle::State_HasFocus;

            // ========== ФОН С ЗАМЕТНЫМ ЦВЕТОМ ==========
            QColor bgColor("#f8f8f8");

            // Если наведен курсор - делаем фон немного светлее
            if (isHovered)
            {
                bgColor = bgColor.lighter(105);
            }

            // Мягкий градиент для объема
            QLinearGradient gradient(opt.rect.topLeft(), opt.rect.bottomLeft());
            gradient.setColorAt(0.0, bgColor.lighter(105)); // Светлее вверху
            gradient.setColorAt(0.5, bgColor);               // Основной цвет
            gradient.setColorAt(1.0, bgColor.darker(105));   // Темнее внизу

            painter->fillRect(opt.rect, gradient);

            // ========== АКЦЕНТНАЯ ЛИНИЯ СЛЕВА ==========
            /*QColor accentLineColor;

            if (isHovered)
            {
                // При наведении - синий цвет #0072C6
                accentLineColor = QColor("#0072C6");
            }
            else
            {
                // Обычно - цвет группы
                accentLineColor = PropertyTreeItem::groupColor(groupIndex);

                // Делаем линию заметнее
                if (accentLineColor.lightness() > 200)
                    accentLineColor = accentLineColor.darker(130);
                else
                    accentLineColor = accentLineColor.darker(115);
            }

            // Рисуем акцентную линию
            QRect accentRect(opt.rect.left(), opt.rect.top(), 3, opt.rect.height());
            painter->fillRect(accentRect, accentLineColor);

            // Белая линия справа от акцентной для объема
            painter->setPen(QColor(255, 255, 255, 180));
            painter->drawLine(opt.rect.left() + 3, opt.rect.top(),
                              opt.rect.left() + 3, opt.rect.bottom());*/

            // ========== ГРАНИЦЫ В СТИЛЕ OFFICE 2013 ==========
            // Верхняя тонкая серая линия
            QColor topBorderColor = isHovered ? QColor("#c5d2e0") : QColor(220, 220, 220);
            painter->setPen(topBorderColor);
            painter->drawLine(opt.rect.topLeft(), opt.rect.topRight());

            // Нижняя линия
            painter->setPen(QColor(210, 210, 210));
            painter->drawLine(opt.rect.bottomLeft(), opt.rect.bottomRight());

            // ========== ТЕКСТ ГРУППЫ ==========
            QFont font = painter->font();
            font.setBold(true);
            font.setPointSize(font.pointSize());

            // Используем Segoe UI если доступен
            if (QFontDatabase().families().contains("Segoe UI"))
                font.setFamily("Segoe UI");

            painter->setFont(font);

            // Цвет текста зависит от наведения
            QColor textColor;
            if (isHovered)
            {
                textColor = QColor("#0072C6"); // Синий при наведении
            }
            else
            {
                textColor = QColor(102, 102, 102); // Серый обычно (#666666)
            }

            // Легкая тень текста для объема (только если не наведен курсор)
            if (!isHovered)
            {
                painter->setPen(QColor(255, 255, 255, 150));
                painter->drawText(opt.rect.adjusted(13, 1, -7, 1),
                                  Qt::AlignLeft | Qt::AlignVCenter, groupName);
            }

            // Основной текст
            painter->setPen(textColor);

            // Текст с отступом от акцентной линии
            QRect textRect = opt.rect.adjusted(12, 0, -40, 0);
            painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, groupName);

            // Светлая подсветка верхнего края
            painter->setPen(QColor(255, 255, 255, 120));
            painter->drawLine(opt.rect.topLeft() + QPoint(0, 1),
                              opt.rect.topRight() + QPoint(0, 1));

            // ========== ДОПОЛНИТЕЛЬНЫЙ ЭФФЕКТ ПРИ НАВЕДЕНИИ ==========
            if (isHovered)
            {
                // Легкая голубая подсветка всей строки
                painter->setPen(Qt::NoPen);
                painter->setBrush(QColor(230, 242, 252, 30)); // Очень прозрачный голубой
                painter->drawRect(opt.rect);

                // Тонкая синяя линия снизу при наведении
                painter->setPen(QPen(QColor("#0072C6"), 1));
                painter->drawLine(opt.rect.bottomLeft(), opt.rect.bottomRight());
            }
            //painter->restore();
        }
        else
        {
            // Элементы внутри группы - стандартная отрисовка
            QStyledItemDelegate::paint(painter, opt, index);
        }
    }

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_FINAL
    {
        if (!index.parent().isValid()) {
            QSize size = QStyledItemDelegate::sizeHint(option, index);
            size.setHeight(22); // Увеличиваем высоту для групп
            return size;
        }
        return QStyledItemDelegate::sizeHint(option, index);
    }

private:
    int m_hoveredRow; // Для отслеживания наведения мыши
};

ToolBoxTreeView::ToolBoxTreeView(QWidget *parent) :
    QTreeView(parent)
{
    setIconSize(QSize(24, 24));

    m_pDelegate = new ToolBoxDelegate(this);
    setItemDelegate(m_pDelegate);
    setItemsExpandable(false);
    setRootIsDecorated(false);
}

ToolBoxTreeView::~ToolBoxTreeView()
{

}

QPixmap ToolBoxTreeView::dragPixmap(const QModelIndex &index) const
{
    // Редактор может переопределить отрисовку плашки
    // (BaseEditorWindow::toolBoxDragPixmap). Модель toolbox создаётся с
    // parent = редактор (BaseEditorWindow), поэтому достаём его оттуда.
    // Пустой pixmap — отрисовка по умолчанию
    if (QAbstractItemModel *m = model())
    {
        if (BaseEditorWindow *wnd = qobject_cast<BaseEditorWindow*>(m->parent()))
        {
            const QPixmap custom = wnd->toolBoxDragPixmap(index);
            if (!custom.isNull())
                return custom;
        }
    }

    // Если редактор задал отдельную иконку для перетаскивания
    // (DragIconRole) — берем её, иначе иконку элемента из списка
    QIcon icon = index.data(ToolBoxModel::DragIconRole).value<QIcon>();
    if (icon.isNull())
        icon = index.data(Qt::DecorationRole).value<QIcon>();

    const QString text = index.data(Qt::DisplayRole).toString();

    const int iconSize = 16;
    const int hMargin = 6;
    const int vMargin = 4;
    const int spacing = 4;

    const QFontMetrics fm(font());
    const QString elided = fm.elidedText(text, Qt::ElideRight, 220);

    int w = hMargin + fm.horizontalAdvance(elided) + hMargin;
    if (!icon.isNull())
        w += iconSize + spacing;

    const int h = qMax(iconSize, fm.height()) + 2 * vMargin;

    QPixmap pm(w, h);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    // Плашка в стиле office: голубая заливка, синяя рамка
    p.setPen(QColor("#0072C6"));
    p.setBrush(QColor("#E5F1FB"));
    p.drawRoundedRect(pm.rect().adjusted(0, 0, -1, -1), 3, 3);

    int x = hMargin;
    if (!icon.isNull())
    {
        icon.paint(&p, x, (h - iconSize) / 2, iconSize, iconSize);
        x += iconSize + spacing;
    }

    p.setPen(QColor("#1E1E1E"));
    p.drawText(QRect(x, 0, w - x - hMargin, h),
               Qt::AlignLeft | Qt::AlignVCenter, elided);

    return pm;
}

void ToolBoxTreeView::mousePressEvent(QMouseEvent *event)
{
    if (!model())
    {
        QTreeView::mousePressEvent(event);
        return;
    }

    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
    {
        QTreeView::mousePressEvent(event);
        return;
    }

    QModelIndex parentIndex = model()->parent(index);
    if (!parentIndex.isValid())
    {
        QTreeView::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData();

        QString mimetype = model()->data(index, ToolBoxModel::MimeTypeRole).toString();
        QByteArray mimidata = model()->data(index, ToolBoxModel::MimeDataRole).toByteArray();
        mimeData->setData(mimetype, mimidata);

        const QPixmap pm = dragPixmap(index);
        drag->setPixmap(pm);
        drag->setHotSpot(QPoint(8, pm.height() / 2));

        drag->setMimeData(mimeData);
        //qDebug() << mimetype << mimidata;
        /*Qt::DropAction dropAction = */drag->exec();
    }
}
