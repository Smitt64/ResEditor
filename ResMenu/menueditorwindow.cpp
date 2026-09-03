#include "menueditorwindow.h"
#include "codeeditor/codeeditor.h"
#include "codeeditor/codehighlighter.h"
#include "codeeditor/highlighterstyle.h"
#include "editormenubar.h"
#include "editormenu.h"
#include "lbrobjectinterface.h"
#include "styles/mdiofficestyle.h"
#include "menuaction.h"
#include "menucodehighlighter.h"
#include "resbuffer.h"
#include "propertymodel.h"
#include "resmenu.h"
#include "rsrescore.h"
#include "toolbox/toolboxmodel.h"
#include "undoredo/menuundocommands.h"

#include "SARibbon.h"
#include "toolsruntime.h"
#include "widgets/codeinputdialog.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QClipboard>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QPixmap>
#include <QStyle>
#include <QStyleFactory>
#include <QToolBar>
#include <QUndoStack>
#include <QVBoxLayout>
#include <QXmlStreamReader>

// CodeEditor::setCodeHighlighter() — protected, поэтому применяем
// хайлайтер через наследника (аналог ToolApplyHighlighter, но для
// пользовательского CodeHighlighter).
class MenuCodeEditor : public CodeEditor
{
public:
    using CodeEditor::CodeEditor;

    void applyHighlighter(CodeHighlighter *highlighter)
    {
        if (!highlighter)
            return;

        highlighter->setStyle(QString());
        highlighter->setDocument(document());

        QSharedPointer<StyleItem> style = highlighter->style();
        setCurrentLineColor(style->editorCurrentLine());
        setCurrentWordColor(style->editorCurrentWord());

        QTextCharFormat def = style->format(FormatDefault);
        QFont defaultFont = def.font();
        defaultFont.setStyleHint(QFont::Courier);
        document()->setDefaultFont(defaultFont);

        QFontMetrics metrics(defaultFont);
        setTabStopWidth(4 * metrics.width(QLatin1Char(' ')));
        setCodeHighlighter(highlighter);

        const QColor background = style->editorBackground();
        setStyleSheet(QString("QPlainTextEdit { background-color: rgb(%1, %2, %3); color: rgb(%4, %5, %6) }")
                          .arg(background.red())
                          .arg(background.green())
                          .arg(background.blue())
                          .arg(def.foreground().color().red())
                          .arg(def.foreground().color().green())
                          .arg(def.foreground().color().blue()));
        setAutoFillBackground(true);
    }
};

MenuEditorWindow::MenuEditorWindow(QWidget *parent) :
    BaseEditorWindow(parent),
    m_Type(LbrObjectInterface::RES_MENU2)
{
    setupContainer();
}

MenuEditorWindow::~MenuEditorWindow()
{

}

void MenuEditorWindow::setupEditor()
{
    initUndoRedo();
    loadToolBox();
    updateToolBoxIcons();
}

void MenuEditorWindow::updateToolBoxIcons()
{
    ToolBoxModel *tb = toolBox();
    if (!tb)
        return;

    for (int g = 0; g < tb->rowCount(); ++g)
    {
        const QModelIndex group = tb->index(g, 0);
        for (int i = 0; i < tb->rowCount(group); ++i)
        {
            const QModelIndex idx = tb->index(i, 0, group);
            const QByteArray raw = tb->data(idx, ToolBoxModel::MimeDataRole).toByteArray();
            const QJsonObject tpl = QJsonDocument::fromJson(raw).object();

            // Иконка есть только у шаблонов обычных пунктов (type = item)
            const quint32 iconID = tpl.value(QStringLiteral("action"))
                                       .toObject()
                                       .value(QStringLiteral("iconID")).toInt(0);
            if (!iconID)
                continue;

            const QIcon icon = ResMenu::inst()->getResIcon(qint32(iconID));
            if (!icon.isNull())
                tb->setData(idx, QVariant::fromValue(icon), ToolBoxModel::DragIconRole);
        }
    }
}

void MenuEditorWindow::setMenu(ResBuffer *buffer)
{
    if (buffer)
    {
        m_menuName = buffer->name();
        m_menuComment = buffer->comment();
        m_menuDateTime = buffer->getResDateTime();
        m_Type = buffer->type();

        m_pMdiWnd->setWindowIcon(RsResCore::inst()->iconFromResType(m_Type));
        m_pMdiWnd->setWindowTitle(m_menuComment);
    }
    else
    {
        m_menuName.clear();
        m_menuComment.clear();
    }

    m_pMenuBar->setMenu(buffer);
    updateCodeEditor();

    // на момент setMenu ribbon ещё может быть не создан
    // (setRibbonBar вызывается позже) — setupNameLine тоже ставит текст
    if (m_pNameLineEdit)
        m_pNameLineEdit->setText(m_menuName);
}

void MenuEditorWindow::setMenuName(const QString &name)
{
    m_menuName = name;

    // имя выводится первой строкой текстового представления (MENU <name>)
    updateCodeEditor();

    // на момент вызова из newItemsAction ribbon ещё может быть не создан
    // (setRibbonBar вызывается позже) — setupNameLine тоже ставит текст
    if (m_pNameLineEdit)
        m_pNameLineEdit->setText(m_menuName);
}

// ---------------------------------------------------------------------------
// Ribbon-вкладка редактора меню
// ---------------------------------------------------------------------------

void MenuEditorWindow::setupNameLine()
{
    QFont font("TerminalVector", 10);
    font.setFixedPitch(true);

    m_pNameLineEdit = new SARibbonLineEdit(this);
    m_pNameLineEdit->setReadOnly(true);
    m_pNameLineEdit->setFont(font);
    m_pNameLineEdit->setObjectName("pNameLineEdit");
    m_pNameLineEdit->setText(m_menuName);

    QAction *copyAction = m_pNameLineEdit->addAction(QIcon::fromTheme("Copy"), QLineEdit::TrailingPosition);
    connect(copyAction, &QAction::triggered, [=]()
    {
        QClipboard *pClipboard = QApplication::clipboard();
        pClipboard->setText(m_pNameLineEdit->text(), QClipboard::Clipboard);
    });
}

void MenuEditorWindow::initRibbonPanels()
{
    setupNameLine();

    m_pMenuCategory = new SARibbonCategory(tr("Меню"), ribbon());
    MakeMenuRibbonCategory(m_pMenuCategory);
}

void MenuEditorWindow::MakeMenuRibbonCategory(SARibbonCategory *category)
{
    SARibbonPannel *menupanel = category->addPannel(tr("Меню"));

    m_pSaveToXml = createAction(tr("Сохранить в XML"), "XMLFile", QKeySequence("Ctrl+ALT+S"));
    toolAddActionWithTooltip(m_pSaveToXml,
                             tr("Сохраняет меню в XML-формат"),
                             QKeySequence("Ctrl+ALT+S"));
    connect(m_pSaveToXml, &QAction::triggered, this, &MenuEditorWindow::saveToXml);
    menupanel->addLargeAction(m_pSaveToXml);

    m_pActLoadFromXml = createAction(tr("Загрузить из XML"), "ImportXml");
    toolAddActionWithTooltip(m_pActLoadFromXml,
                             tr("Заменяет текущее меню содержимым XML-файла"));
    connect(m_pActLoadFromXml, &QAction::triggered, this, &MenuEditorWindow::loadFromXml);
    menupanel->addLargeAction(m_pActLoadFromXml);
    menupanel->addSeparator();

    menupanel->addSmallWidget(m_pNameLineEdit);

    // Редактирование комментария меню (m_menuComment) — под строкой имени
    m_pEditComment = createAction(tr("Изменить комментарий"), "Comment");
    toolAddActionWithTooltip(m_pEditComment,
                             tr("Редактирует комментарий меню"));
    connect(m_pEditComment, &QAction::triggered, this, &MenuEditorWindow::editComment);
    menupanel->addSmallAction(m_pEditComment);

    m_ConnectToCode = new QMenu(tr("Подключить в коде"), this);
    m_ConnectToCode->setIcon(QIcon::fromTheme("CodeEdit"));
    // Явный setStyle() здесь не задаём: виджет с собственным стилем
    // игнорирует stylesheet MainWindow, а меню должно быть в офисной теме
    menupanel->addSmallMenu(m_ConnectToCode);

    auto ShowConnectDialog = [=](const QString &src, const QString &title)
    {
        QString source = QString("SetResHorMenu(%1, &AppRes, \"%2\");").arg(src).arg(name());
        CodeInputDialog dlg(this, title, tr("Текст:"), source, HighlighterCpp);

        CodeEditor *pCode = dlg.findChild<CodeEditor*>();
        pCode->setReadOnly(true);
        dlg.setWindowIcon(QIcon::fromTheme("CodeEdit"));
        dlg.exec();
    };

    QAction *ConnectScrolAction = m_ConnectToCode->addAction(tr("Подключить к скролингу"));
    connect(ConnectScrolAction, &QAction::triggered, [=]()
    {
        ShowConnectDialog("&Bs->Mp", tr("Подключить к скролингу"));
    });

    QAction *ConnectPanelAction = m_ConnectToCode->addAction(tr("Подключить к панели"));
    connect(ConnectPanelAction, &QAction::triggered, [=]()
    {
        ShowConnectDialog("p", tr("Подключить к панели"));
    });

    // Переключатели флагов выделенного пункта меню. Названия — как алиасы
    // свойств в MenuItem.json. Запись идёт через сеттеры MenuAction —
    // undo-команды они пушат сами; состояние переключателей
    // синхронизирует updateItemRibbonState (выделение + NOTIFY-сигналы,
    // поэтому отражаются и undo/redo)
    SARibbonPannel *flagspanel = category->addPannel(tr("Пункт меню"));

    m_pActFlagToolbar = createAction(tr("Попадает на ToolBar"), "ToolBar");
    toolAddActionWithTooltip(m_pActFlagToolbar,
                             tr("Пункт отображается на панели инструментов (dFMENU_TOOLBAR)"));
    m_pActFlagToolbar->setCheckable(true);
    connect(m_pActFlagToolbar, &QAction::triggered, this, [this](bool checked)
    {
        if (!m_pRibbonTarget)
            return;
        MenuAction::ActionFlags flags = m_pRibbonTarget->actionFlags();
        flags.setFlag(MenuAction::ActionFlag::Toolbar, checked);
        m_pRibbonTarget->setActionFlags(flags);
    });
    flagspanel->addSmallAction(m_pActFlagToolbar);

    m_pActFlagContext = createAction(tr("Попадает в контекстное меню"), "ContextMenu");
    toolAddActionWithTooltip(m_pActFlagContext,
                             tr("Пункт попадает в контекстное меню (dFMENU_CONTEXT)"));
    m_pActFlagContext->setCheckable(true);
    connect(m_pActFlagContext, &QAction::triggered, this, [this](bool checked)
    {
        if (!m_pRibbonTarget)
            return;
        MenuAction::ActionFlags flags = m_pRibbonTarget->actionFlags();
        flags.setFlag(MenuAction::ActionFlag::Context, checked);
        m_pRibbonTarget->setActionFlags(flags);
    });
    flagspanel->addSmallAction(m_pActFlagContext);

    m_pActFlagDisInEmptyScroll = createAction(tr("Недоступен в пустом скроллинге"), "Lock");
    toolAddActionWithTooltip(m_pActFlagDisInEmptyScroll,
                             tr("Пункт недоступен в пустом скроллинге (DISINEMPTYSCROL)"));
    m_pActFlagDisInEmptyScroll->setCheckable(true);
    connect(m_pActFlagDisInEmptyScroll, &QAction::triggered, this, [this](bool checked)
    {
        if (m_pRibbonTarget)
            m_pRibbonTarget->setDisabledInEmptyScroll(checked);
    });
    flagspanel->addSmallAction(m_pActFlagDisInEmptyScroll);

    // Галерея иконок из ресурсной DLL + команда удаления иконки.
    // Запись — через MenuAction::setIconID (undo-команды пушатся сами),
    // выделение/доступность синхронизирует updateItemRibbonState
    SARibbonPannel *iconpanel = category->addPannel(tr("Иконка"));

    m_pActClearIcon = createAction(tr("Очистить\nиконку"), "CleanData");
    toolAddActionWithTooltip(m_pActClearIcon, tr("Убирает иконку с пункта меню (iconID = 0)"));
    iconpanel->addLargeAction(m_pActClearIcon);

    m_pIconGallery = iconpanel->addGallery();
    makeIconGallery(m_pIconGallery);

    connect(m_pActClearIcon, &QAction::triggered, this, [this]()
    {
        if (m_pRibbonTarget)
            m_pRibbonTarget->setIconID(0);
    });

    // Панель быстрого назначения сочетания клавиш выделенному пункту
    SARibbonPannel *keyspanel = category->addPannel(tr("Сочетания клавиш"));
    makeShortcutPanel(keyspanel);

    updateItemRibbonState();
}

void MenuEditorWindow::makeShortcutPanel(SARibbonPannel *panel)
{
    // Популярные сочетания по скану меню Renewal (IconTags.json).
    // Сочетание лежит в data() команды (portable text); на кнопке —
    // NativeText ("Ctrl+Return" -> "Ctrl+Enter"). Сравнение идёт по коду
    // клавиши, поэтому совпадает и с производным от commandId сочетанием
    // (ResKeyMap хранит каноническое написание "Return"/"Ctrl+Return")
    const QVector<QPair<QString, QString>> entries =
    {
        { QStringLiteral("Return"),      tr("Enter — редактирование/просмотр записи") },
        { QStringLiteral("Ctrl+Return"), tr("Ctrl+Enter — просмотр записи/параметров") },
        { QStringLiteral("F2"),          tr("F2 — внести/включить в книгу") },
        { QStringLiteral("F4"),          tr("F4 — поиск по значению текущего поля") },
        { QStringLiteral("F5"),          tr("F5 — фильтр") },
        { QStringLiteral("F6"),          tr("F6 — шаги операции") },
        { QStringLiteral("Ctrl+F6"),     tr("Ctrl+F6 — проводки") },
        { QStringLiteral("F7"),          tr("F7 — печать / функция пользователя") },
        { QStringLiteral("F8"),          tr("F8 — удаление") },
        { QStringLiteral("Alt+F8"),      tr("Alt+F8 — откат операции / в отложенные") },
        { QStringLiteral("F9"),          tr("F9 — ввод/сохранение") },
        { QStringLiteral("Alt+F9"),      tr("Alt+F9") },
        { QStringLiteral("Shift+F9"),    tr("Shift+F9") },
        { QStringLiteral("Alt+F10"),     tr("Alt+F10 — подсчет суммы") },
        { QStringLiteral("Esc"),         tr("Esc — выход") },
    };

    m_pShortcutGroup = new QActionGroup(this);
    m_pShortcutGroup->setExclusive(true);

    for (const auto &entry : entries)
    {
        const QKeySequence seq(entry.first);

        QAction *act = createAction(seq.toString(QKeySequence::NativeText), "");
        act->setCheckable(true);
        act->setData(entry.first);
        toolAddActionWithTooltip(act, entry.second);
        m_pShortcutGroup->addAction(act);
        panel->addSmallAction(act);
    }

    connect(m_pShortcutGroup, &QActionGroup::triggered, this, [this](QAction *act)
    {
        if (!m_pRibbonTarget || !act)
            return;

        const QString seq = act->data().toString();

        // повторный клик по текущему явно назначенному сочетанию —
        // снимаем его (если сочетание производное от commandId,
        // shortcutText и так пуст — запись совпадающего значения
        // просто закрепит его явно)
        if (m_pRibbonTarget->shortcutText() == seq)
            m_pRibbonTarget->setShortcutText(QString());
        else
            m_pRibbonTarget->setShortcutText(seq);
    });
}

void MenuEditorWindow::updateShortcutChecks()
{
    if (!m_pShortcutGroup)
        return;

    // отражаем фактическое сочетание пункта: явное (shortcutText),
    // а если его нет — производное от commandId (ResKeyMap)
    const QKeySequence cur = m_pRibbonTarget
            ? m_pRibbonTarget->effectiveShortcut() : QKeySequence();
    const int curKey = (cur.count() == 1) ? cur[0] : 0;
    const bool en = (m_pRibbonTarget != nullptr);

    for (QAction *act : m_pShortcutGroup->actions())
    {
        const QKeySequence seq = QKeySequence::fromString(
                    act->data().toString(), QKeySequence::PortableText);

        act->setEnabled(en);
        act->setChecked(en && curKey && seq.count() == 1 && seq[0] == curKey);
    }
}

void MenuEditorWindow::makeIconGallery(SARibbonGallery *gallery)
{
    QList<QAction*> galleryActions;

    ResMenu *res = ResMenu::inst();
    const QMap<quint32, QPixmap> pixmaps = res->resPixMaps();

    for (auto it = pixmaps.constBegin(); it != pixmaps.constEnd(); ++it)
    {
        const quint32 id = it.key();

        // 24px-двойник (ресурс id + 2000) отдельно не показываем —
        // та же логика, что в IconBrowser
        if (it.value().width() >= 20 && pixmaps.contains(id - 2000))
            continue;

        const QPixmap pm = pixmaps.contains(id + 2000)
                ? pixmaps.value(id + 2000) : it.value();

        QAction *act = createAction(QString::number(id), "");
        act->setIcon(QIcon(pm));
        act->setData(id);

        QString tip = tr("ICON_%1").arg(id);
        const QStringList tags = res->resIconTags(id);
        if (!tags.isEmpty())
            tip += tr("\nТеги: %1").arg(tags.join(tr(", ")));
        act->setToolTip(tip);

        galleryActions.append(act);
    }

    m_pIconGalleryGroup = gallery->addCategoryActions(tr("Иконки"), galleryActions);
    m_pIconGalleryGroup->setGalleryGroupStyle(SARibbonGalleryGroup::IconOnly);
    m_pIconGalleryGroup->setDisplayRow(SARibbonGalleryGroup::DisplayTwoRow);

    connect(m_pIconGalleryGroup, &SARibbonGalleryGroup::triggered,
            this, [this](QAction *act)
    {
        if (m_pRibbonTarget && act)
            m_pRibbonTarget->setIconID(act->data().toUInt());
    });
}

void MenuEditorWindow::updateIconGallerySelection()
{
    if (!m_pIconGalleryGroup)
        return;

    const quint32 currentId = m_pRibbonTarget ? m_pRibbonTarget->iconID() : 0;

    int select = -1;
    SARibbonGalleryGroupModel *model = m_pIconGalleryGroup->groupModel();
    if (currentId)
    {
        for (int i = 0; i < model->rowCount(QModelIndex()); ++i)
        {
            SARibbonGalleryItem *item = model->at(i);
            if (item->action() && item->action()->data().toUInt() == currentId)
            {
                select = i;
                break;
            }
        }
    }

    // выделение дублируем в видимую группу галереи — как в
    // StdPanelEditor::ApplyBorderStyleToGallary
    if (select >= 0)
    {
        const QModelIndex idx = model->index(select, 0, QModelIndex());
        m_pIconGalleryGroup->setCurrentIndex(idx);

        if (SARibbonGalleryGroup *view = m_pIconGallery->currentViewGroup())
            view->setCurrentIndex(idx);
    }
    else
    {
        m_pIconGalleryGroup->clearSelection();

        if (SARibbonGalleryGroup *view = m_pIconGallery->currentViewGroup())
            view->clearSelection();
    }
}

void MenuEditorWindow::updateRibbonTabs()
{
    SARibbonContextCategory *context = findCategoryByName(tr("Ресурс"));

    if (!context)
        return;

    QList<SARibbonCategory*> oldCategories = context->categoryList();
    for (SARibbonCategory *cat : qAsConst(oldCategories))
        context->takeCategory(cat);

    if (m_pMenuCategory)
    {
        if (!context->isHaveCategory(m_pMenuCategory))
            context->addCategoryPage(m_pMenuCategory);
    }

    ribbon()->showContextCategory(context);
    ribbon()->showCategory(m_pMenuCategory);
}

void MenuEditorWindow::clearRibbonTabs()
{
    SARibbonContextCategory *context = findCategoryByName(tr("Ресурс"));

    if (!context)
        return;

    if (m_pMenuCategory)
    {
        ribbon()->removeCategory(m_pMenuCategory);
        ribbon()->hideCategory(m_pMenuCategory);
    }

    bool hasVisible = false;
    QList<SARibbonCategory*> oldCategories = context->categoryList();
    for (SARibbonCategory *cat : qAsConst(oldCategories))
    {
        if (ribbon()->isCategoryVisible(cat))
        {
            hasVisible = true;
            break;
        }
    }

    if (!hasVisible)
        ribbon()->hideContextCategory(context);
}

bool MenuEditorWindow::save(ResBuffer *res, QString *error)
{
    // Тело всегда пишем в версии 2 (с полями cmdEx/iconID/flags у пунктов),
    // иначе при повторной загрузке расширенные поля не прочитаются
    res->setResVersion(2);

    // комментарий в заголовок ресурса (в теле его запишет writeMenu)
    res->setComment(m_menuComment);

    if (!m_pMenuBar->writeMenu(res, m_menuComment, error))
        return false;

    // сброс признака модификации (m_UndoIndexUnchanged + modifyChanged)
    return BaseEditorWindow::save(res, error);
}

void MenuEditorWindow::editComment()
{
    bool ok = false;
    const QString text = QInputDialog::getMultiLineText(this,
        tr("Комментарий меню"), tr("Комментарий:"), m_menuComment, &ok);

    if (!ok || text == m_menuComment)
        return;

    if (undoStack())
        undoStack()->push(new MenuUndo::ChangeMenuCommentCommand(this, text));
    else
        applyComment(text);
}

void MenuEditorWindow::applyComment(const QString &text)
{
    if (m_menuComment == text)
        return;

    m_menuComment = text;

    if (m_pMdiWnd)
        m_pMdiWnd->setWindowTitle(m_menuComment);

    // заголовок вкладки в основном приложении (MainWindow::titleChanged)
    emit titleChanged(title());

    // комментарий выводится первой строкой в текстовом представлении
    updateCodeEditor();
}

void MenuEditorWindow::saveToXml()
{
    // Имя файла по умолчанию — как в старой утилите: <name>_<type>.xml
    // (для меню суффикс "M")
    QString baseName = m_menuName;
    const char *prefix = RsResCore::inst()->resTypePrefix(m_Type);
    if (prefix[0] != 'N')
        baseName += QLatin1Char('_') + QLatin1String(prefix);

    const QString filename = QFileDialog::getSaveFileName(this,
        tr("Сохранение меню в XML"), baseName + QStringLiteral(".xml"),
        tr("XML файлы (*.xml)"));

    if (filename.isEmpty())
        return;

    QFile f(filename);
    if (!f.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("Сохранение в XML"),
                             tr("Не удалось открыть файл для записи:\n%1").arg(filename));
        return;
    }

    QTextStream out(&f);
    out.setCodec("UTF-8");
    out << menuToXml();
}

void MenuEditorWindow::loadFromXml()
{
    const QString filename = QFileDialog::getOpenFileName(this,
        tr("Загрузка меню из XML"), QString(),
        tr("XML файлы (*.xml)"));

    if (filename.isEmpty())
        return;

    QString error;
    if (!loadFromXmlFile(filename, &error))
        QMessageBox::warning(this, tr("Загрузка из XML"),
                             tr("Не удалось загрузить меню из файла:\n%1\n%2")
                                 .arg(filename, error));
}

bool MenuEditorWindow::loadFromXmlFile(const QString &filename, QString *error)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
    {
        if (error)
            *error = tr("Не удалось открыть файл");
        return false;
    }

    QXmlStreamReader xml(&f);
    QString err;

    if (!xml.readNextStartElement() || xml.name() != QLatin1String("reslib"))
        err = tr("корневой элемент <reslib> не найден");
    else if (!xml.readNextStartElement() || xml.name() != QLatin1String("menu"))
        err = tr("элемент <menu> не найден");

    QString name, comment;
    QDateTime dt;

    if (err.isEmpty())
    {
        const QXmlStreamAttributes attrs = xml.attributes();
        name    = attrs.value(QStringLiteral("name")).toString();
        comment = attrs.value(QStringLiteral("comment")).toString();
        dt      = QDateTime::fromString(attrs.value(QStringLiteral("dt")).toString(),
                                        Qt::ISODate);

        // выделение ссылается на заменяемые пункты — сбрасываем заранее
        m_pMenuBar->clearSelection();

        if (!m_pMenuBar->setMenuFromXml(xml, &err))
            err = tr("ошибка разбора XML: %1").arg(err);
    }

    if (!err.isEmpty())
    {
        if (error)
            *error = err;
        return false;
    }

    m_menuName    = name;
    m_menuComment = comment;
    if (dt.isValid())
        m_menuDateTime = dt;

    // undo-команды ссылаются на удалённые пункты — историю очищаем
    if (undoStack())
        undoStack()->clear();

    if (m_pNameLineEdit)
        m_pNameLineEdit->setText(m_menuName);

    if (m_pMdiWnd)
        m_pMdiWnd->setWindowTitle(m_menuComment);

    // заголовок окна — в формате ResMenuEditorPlugin::SetupEditorTitle
    setWindowTitle(QStringLiteral("%1 [%2]: %3")
                       .arg(RsResCore::inst()->typeNameFromResType(m_Type),
                            m_menuName, QString()));

    updateCodeEditor();
    return true;
}

QAbstractItemModel *MenuEditorWindow::propertyModel()
{
    return m_pPropertyModel;
}

void MenuEditorWindow::updateItemRibbonState()
{
    // ribbon-панель может быть ещё не создана (setRibbonBar вызывается
    // позже setMenu)
    if (!m_pActFlagToolbar)
        return;

    // Переключатели работают только с обычными пунктами (MenuAction);
    // у подменю (EditorMenu) этих флагов нет
    MenuAction *ma = qobject_cast<MenuAction*>(m_pCurrentPropertyTarget);

    if (m_pRibbonTarget != ma)
    {
        if (m_pRibbonTarget)
            disconnect(m_pRibbonTarget, nullptr, this, nullptr);

        m_pRibbonTarget = ma;

        if (ma)
        {
            // NOTIFY-сигналы приходят и из сеттеров, и из undo/redo
            // (ChangeMenuPropertyCommand -> setProperty -> сеттер)
            connect(ma, &MenuAction::actionFlagsChanged, this,
                    [this](MenuAction::ActionFlags flags)
            {
                m_pActFlagToolbar->setChecked(flags.testFlag(MenuAction::ActionFlag::Toolbar));
                m_pActFlagContext->setChecked(flags.testFlag(MenuAction::ActionFlag::Context));

                // бит DISINEMPTYSCROL живёт в том же поле flags
                if (m_pRibbonTarget)
                    m_pActFlagDisInEmptyScroll->setChecked(m_pRibbonTarget->disabledInEmptyScroll());
            });
            connect(ma, &MenuAction::disabledInEmptyScrollChanged, this,
                    [this](bool disabled)
            {
                m_pActFlagDisInEmptyScroll->setChecked(disabled);
            });
            connect(ma, &MenuAction::iconIDChanged, this,
                    [this](quint32)
            {
                updateIconGallerySelection();

                // команда удаления активна, только когда иконка есть
                if (m_pRibbonTarget)
                    m_pActClearIcon->setEnabled(m_pRibbonTarget->iconID() != 0);
            });
            // сочетание меняется и явно (shortcutText), и через смену
            // commandId (производное сочетание из ResKeyMap)
            connect(ma, &MenuAction::shortcutTextChanged, this,
                    [this](const QString &) { updateShortcutChecks(); });
            connect(ma, &MenuAction::commandIdChanged, this,
                    [this](int) { updateShortcutChecks(); });
        }
    }

    m_pActFlagToolbar->setEnabled(ma != nullptr);
    m_pActFlagContext->setEnabled(ma != nullptr);
    m_pActFlagDisInEmptyScroll->setEnabled(ma != nullptr);

    const MenuAction::ActionFlags flags = ma ? ma->actionFlags() : MenuAction::ActionFlags();
    m_pActFlagToolbar->setChecked(flags.testFlag(MenuAction::ActionFlag::Toolbar));
    m_pActFlagContext->setChecked(flags.testFlag(MenuAction::ActionFlag::Context));
    m_pActFlagDisInEmptyScroll->setChecked(ma ? ma->disabledInEmptyScroll() : false);

    // галерея иконок + команда удаления (панель "Иконка" может быть
    // ещё не создана — ribbon строится позже)
    if (m_pIconGallery)
    {
        m_pIconGallery->setEnabled(ma != nullptr);
        m_pActClearIcon->setEnabled(ma && ma->iconID() != 0);
        updateIconGallerySelection();
    }

    updateShortcutChecks();
}

void MenuEditorWindow::onMenuItemSelected(QWidget *container, QAction *action)
{
    Q_UNUSED(container)

    if (!action) {
        onMenuSelectionCleared();
        return;
    }

    // Плейсхолдеры и разделители не имеют редактируемых свойств
    if (action == m_pMenuBar->placeHolder() || action->isSeparator()) {
        onMenuSelectionCleared();
        return;
    }

    QObject *target = nullptr;

    if (auto *ma = qobject_cast<MenuAction *>(action))
        target = ma;
    else if (auto *sub = qobject_cast<EditorMenu *>(action->menu()))
        target = sub;

    if (target == m_pCurrentPropertyTarget)
        return;

    m_pCurrentPropertyTarget = target;
    PropertyModel *oldModel = m_pPropertyModel;
    m_pPropertyModel = target ? new PropertyModel(target, this) : nullptr;

    emit propertyModelChanged(m_pPropertyModel);
    delete oldModel;

    updateToolBarSelection();
    updateItemRibbonState();
}

void MenuEditorWindow::onMenuSelectionCleared()
{
    if (!m_pCurrentPropertyTarget)
        return;

    m_pCurrentPropertyTarget = nullptr;
    PropertyModel *oldModel = m_pPropertyModel;
    m_pPropertyModel = nullptr;

    emit propertyModelChanged(nullptr);
    delete oldModel;

    updateToolBarSelection();
    updateItemRibbonState();
}

void MenuEditorWindow::setupContainer()
{
    m_pMdi = new QMdiArea();
    m_pMdi->setStyle(MdiOfficeStyle::inst());
    m_pMdi->setBackground(QColor("#EBEFF2")); // как в FmtLib
    m_pMdi->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_pMdi->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(m_pMdi);

    m_pContainer = new QMainWindow(this);
    m_pContainer->setMinimumSize(800, 600);

    QStyle *vistaStyle = QStyleFactory::create(QStringLiteral("windowsvista"));
    if (vistaStyle)
    {
        // Парентим на this, а НЕ на m_pContainer: QSS-прокси потомков
        // контейнера ссылаются на vistaStyle как на base. Дети QObject
        // удаляются в порядке создания — vistaStyle (создан после
        // m_pContainer) умрёт позже всех виджетов контейнера, иначе
        // при закрытии редактора QPlainTextEdit::updateRequest в
        // деструкторе CodeEditor дёргал бы styleHint по висячему стилю
        vistaStyle->setParent(this);
    }

    m_pContainer->setStyle(vistaStyle);
    m_pContainer->setPalette(vistaStyle->standardPalette());

    // Перекрываем офисные правила QMenu/QMenuBar из темы MainWindow:
    // stylesheet контейнера "ближе" к всплывающим меню предпросмотра,
    // поэтому при равной специфичности его правила выигрывают у каскада
    // с MainWindow. Предпросмотр должен выглядеть нативно (компактно),
    // как меню в целевом приложении, а не как ribbon-попапы.
    m_pContainer->setStyleSheet(QStringLiteral(
        "QMenu {"
        "  background-color: palette(button);"
        "  border: 1px solid palette(mid);"
        "  color: palette(text);"
        "  padding: 2px;"
        "  margin: 0px;"
        "}"
        "QMenu QMenu {"
        "  background-color: palette(button);"
        "  border: 1px solid palette(mid);"
        "  margin-left: 0px;"
        "}"
        "QMenu::item {"
        "  background-color: transparent;"
        "  color: palette(text);"
        "  padding: 2px 20px 2px 24px;"
        "  margin: 0px;"
        "  border: none;"
        "  min-height: 0px;"
        "}"
        "QMenu::item:selected {"
        "  background-color: palette(highlight);"
        "  color: palette(highlighted-text);"
        "  border: none;"
        "}"
        "QMenu::item:checked {"
        "  background-color: palette(midlight);"
        "  border: none;"
        "}"
        "QMenu::item:disabled {"
        "  color: #6d6d6d;"
        "  background-color: transparent;"
        "}"
        "QMenu::separator {"
        "  height: 1px;"
        "  background-color: palette(mid);"
        "  margin: 2px 4px;"
        "}"
        "QMenuBar {"
        "  background-color: palette(button);"
        "  color: palette(text);"
        "  padding: 0px;"
        "  border: none;"
        "}"
        "QMenuBar::item {"
        "  background-color: transparent;"
        "  padding: 2px 8px;"
        "  margin: 0px;"
        "  border: none;"
        "  min-height: 0px;"
        "}"
        "QMenuBar::item:selected,"
        "QMenuBar::item:pressed {"
        "  background-color: palette(highlight);"
        "  color: palette(highlighted-text);"
        "  border: none;"
        "}"));

    MenuCodeEditor *codeEditor = new MenuCodeEditor(this);
    codeEditor->setReadOnly(true);

    // Отступы по бокам и снизу, чтобы редактор не прилипал к рамке MDI-окна
    QWidget *editorHolder = new QWidget(m_pContainer);
    QVBoxLayout *editorLay = new QVBoxLayout(editorHolder);
    editorLay->setContentsMargins(2, 0, 2, 2);
    editorLay->setSpacing(0);
    editorLay->addWidget(codeEditor);
    m_pContainer->setCentralWidget(editorHolder);

    pCodeEditor = codeEditor;
    pCodeEditor->setFrameShape(QFrame::NoFrame);
    pCodeEditor->setFrameShadow(QFrame::Plain);

    // Подсветка синтаксиса текстового представления меню
    codeEditor->applyHighlighter(new MenuCodeHighlighter(codeEditor));

    m_pMenuBar = new EditorMenuBar(m_pContainer);
    m_pMenuBar->setStyle(vistaStyle);
    m_pMenuBar->setPalette(vistaStyle->standardPalette());
    // После setStyle(): подчёркивания мнемоник (&Обслуживание) показываем
    // всегда — отдельный экземпляр windowsvista сам Alt не отслеживает
    forceUnderlineShortcuts(m_pMenuBar);
    m_pContainer->setMenuBar(m_pMenuBar);
    m_pMenuBar->setUndoStack(undoStack());

    // Панель с предпросмотром пунктов, у которых установлен флаг Toolbar
    m_pToolBar = new QToolBar(tr("Панель инструментов меню"), m_pContainer);
    m_pToolBar->setObjectName(QStringLiteral("menuPreviewToolBar"));
    m_pContainer->addToolBar(Qt::TopToolBarArea, m_pToolBar);
    m_pToolBar->setMovable(false);

    // Подсветка текущего выделенного пункта (свойство "current"
    // выставляется в updateToolBarSelection)
    m_pToolBar->setStyleSheet(QStringLiteral(
        "QToolButton[current=\"true\"] {"
        "  background-color: #BDD7EE;"
        "  border: 1px solid #0072C6;"
        "  border-radius: 3px;"
        "}"));

    connect(undoStack(), &QUndoStack::indexChanged,
            this, &MenuEditorWindow::updateCodeEditor);
    connect(undoStack(), &QUndoStack::indexChanged,
            this, &MenuEditorWindow::resetPropertyModel, Qt::QueuedConnection);

    connect(m_pMenuBar, &EditorMenuBar::itemSelected,
            this, &MenuEditorWindow::onMenuItemSelected);
    connect(m_pMenuBar, &EditorMenuBar::selectionCleared,
            this, &MenuEditorWindow::onMenuSelectionCleared);

    // Клик по пункту на панели инструментов — тоже выделение:
    // показываем его свойства в редакторе свойств
    connect(m_pToolBar, &QToolBar::actionTriggered,
            this, [this](QAction *a) { onMenuItemSelected(m_pToolBar, a); });

    m_pMdiWnd = m_pMdi->addSubWindow(m_pContainer);
    m_pMdiWnd->setStyle(MdiOfficeStyle::inst());
    m_pMdiWnd->setWindowIcon(RsResCore::inst()->iconFromResType(m_Type));
    m_pMdiWnd->setWindowTitle(tr("Новое меню"));
    m_pMdiWnd->setWindowFlags(m_pMdiWnd->windowFlags() & ~Qt::WindowSystemMenuHint);
    m_pMdiWnd->setWindowFlags(m_pMdiWnd->windowFlags() & ~Qt::WindowMinMaxButtonsHint);
    m_pMdiWnd->setWindowFlags(m_pMdiWnd->windowFlags() & ~Qt::WindowCloseButtonHint);
    m_pMdiWnd->show();
}

// ---------------------------------------------------------------------------
// Синхронизация pCodeEditor с текущим состоянием меню
// ---------------------------------------------------------------------------

void MenuEditorWindow::updateCodeEditor()
{
    if (!pCodeEditor)
        return;

    const QString text = menuToText();
    if (pCodeEditor->toPlainText() != text)
        pCodeEditor->setPlainText(text);

    updateToolBar();
}

// ---------------------------------------------------------------------------
// Панель инструментов: пункты меню с флагом ActionFlag::Toolbar
// ---------------------------------------------------------------------------

void MenuEditorWindow::updateToolBar()
{
    if (!m_pToolBar || !m_pMenuBar)
        return;

    m_pToolBar->clear();
    collectToolBarActions(m_pMenuBar->actions());
    updateToolBarSelection();
}

// Помечаем кнопку текущего выделенного пункта динамическим
// свойством "current" — стиль задан в setupContainer
void MenuEditorWindow::updateToolBarSelection()
{
    if (!m_pToolBar)
        return;

    for (QAction *a : m_pToolBar->actions()) {
        QWidget *w = m_pToolBar->widgetForAction(a);
        if (!w)
            continue;

        const bool current = (a == m_pCurrentPropertyTarget);
        if (w->property("current").toBool() != current) {
            w->setProperty("current", current);
            w->style()->unpolish(w);
            w->style()->polish(w);
            w->update();
        }
    }
}

void MenuEditorWindow::collectToolBarActions(const QList<QAction *> &actions)
{
    for (QAction *a : actions) {
        if (auto *sub = qobject_cast<EditorMenu *>(a->menu())) {
            collectToolBarActions(sub->actions());
            continue;
        }

        if (a->isSeparator())
            continue;

        // Плейсхолдеры ("Добавить пункт", "Добавить разделитель") — обычные
        // QAction, через qobject_cast<MenuAction*> отсеиваются сами
        if (auto *ma = qobject_cast<MenuAction *>(a)) {
            if (ma->actionFlags().testFlag(MenuAction::ActionFlag::Toolbar))
                m_pToolBar->addAction(ma);
        }
    }
}

void MenuEditorWindow::resetPropertyModel()
{
    if (m_pPropertyModel)
        m_pPropertyModel->reset();
}

QString MenuEditorWindow::menuToText() const
{
    QString text;
    QTextStream ts(&text);

    if (!m_menuComment.isEmpty())
        ts << "MENU " << m_menuName << ", \"" << escapeMenuString(m_menuComment) << "\"\n";
    else
        ts << "MENU " << m_menuName << "\n";

    const QList<QAction *> acts = m_pMenuBar->actions();
    for (QAction *a : acts) {
        if (a == m_pMenuBar->placeHolder())
            continue;
        if (auto *sub = qobject_cast<EditorMenu *>(a->menu()))
            ts << popupToText(sub, 3);
    }

    ts << "END\n";
    return text;
}

QString MenuEditorWindow::popupToText(EditorMenu *menu, int indent) const
{
    QString text;
    QTextStream ts(&text);
    const QString spaces(indent, QLatin1Char(' '));

    ts << spaces << "POPUP \"" << escapeMenuString(encodeMnemonic(menu->title())) << "\", ";

    const QAction *menuAction = menu->menuAction();
    const QString par = menuAction ? menuAction->statusTip() : QString();
    if (!par.isEmpty())
        ts << "\"" << escapeMenuString(par) << "\"";
    ts << ", " << menu->helpId() << "\n";

    const QList<QAction *> acts = menu->actions();
    for (QAction *a : acts) {
        if (a == menu->placeHolder() || a == menu->sepPlaceholder())
            continue;

        if (a->isSeparator()) {
            ts << spaces << "DELIM\n";
            continue;
        }

        if (auto *sub = qobject_cast<EditorMenu *>(a->menu())) {
            ts << popupToText(sub, indent + 3);
            continue;
        }

        if (auto *ma = qobject_cast<MenuAction *>(a)) {
            ts << spaces << "ITEM \""
               << escapeMenuString(formatItemText(ma->text(), ma->shortcut()))
               << "\", ";
            ts << ma->commandId() << ", ";

            if (!ma->statusTip().isEmpty())
                ts << "\"" << escapeMenuString(ma->statusTip()) << "\"";
            ts << ", " << ma->helpId();

            // Расширенные поля версии 2 (как в старом WritePopUpTxt)
            if (ma->iconID() || ma->actionFlags())
                ts << ", " << ma->iconID() << ", " << static_cast<quint32>(ma->actionFlags());

            ts << "\n";
        }
    }

    ts << spaces << "END\n";
    return text;
}

QString MenuEditorWindow::escapeMenuString(const QString &s) const
{
    QString r = s;
    r.replace(QLatin1Char('\\'), QStringLiteral("\\\\"));
    r.replace(QLatin1Char('"'), QStringLiteral("\\\""));
    return r;
}

QString MenuEditorWindow::encodeMnemonic(const QString &s) const
{
    QString r;
    r.reserve(s.length() * 2);

    for (int i = 0; i < s.length(); ++i) {
        if (s.at(i) == QLatin1Char('&') && i + 1 < s.length()) {
            if (s.at(i + 1) == QLatin1Char('&')) {
                r.append(QLatin1Char('&'));
                ++i;
            } else {
                r.append(QLatin1Char('~')).append(s.at(i + 1)).append(QLatin1Char('~'));
                ++i;
            }
        } else {
            r.append(s.at(i));
        }
    }
    return r;
}

QString MenuEditorWindow::formatItemText(const QString &text, const QKeySequence &shortcut) const
{
    QString r = encodeMnemonic(text);

    if (!shortcut.isEmpty()) {
        QString sc = shortcut.toString(QKeySequence::PortableText);
        r.append(QStringLiteral("    ~")).append(sc).append(QLatin1Char('~'));
    }

    return r;
}

// ---------------------------------------------------------------------------
// Экспорт в XML — формат старого xmlexport.cpp (__GetMenu/__ExportPopUp),
// формируется форматированными строками, чтобы файл был 1 в 1
// ---------------------------------------------------------------------------

QString MenuEditorWindow::xmlEscape(const QString &s) const
{
    // как CharBuffer::xmlEscape: & < > " ' (амперсанд — первым!)
    QString r = s;
    r.replace(QLatin1Char('&'),  QStringLiteral("&amp;"));
    r.replace(QLatin1Char('<'),  QStringLiteral("&lt;"));
    r.replace(QLatin1Char('>'),  QStringLiteral("&gt;"));
    r.replace(QLatin1Char('"'),  QStringLiteral("&quot;"));
    r.replace(QLatin1Char('\''), QStringLiteral("&apos;"));
    return r;
}

QString MenuEditorWindow::menuToXml() const
{
    QDateTime dt = m_menuDateTime;
    if (!dt.isValid())
        dt = QDateTime::currentDateTime();

    QString xml = QStringLiteral(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<reslib xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n"
        "    xsi:schemaLocation=\"http://www.softlab.ru reslib.xsd\"\n"
        "    xmlns=\"http://www.softlab.ru\">\n");

    xml += QStringLiteral(" <menu name=\"%1\" dt=\"%2\" comment=\"%3\">\n")
        .arg(xmlEscape(m_menuName),
             dt.toString(QStringLiteral("yyyy-MM-ddTHH:mm:ss")),
             xmlEscape(m_menuComment));

    xml += itemsToXml(m_pMenuBar->actions(), 2);
    xml += QStringLiteral(" </menu>\n</reslib>\n");
    return xml;
}

QString MenuEditorWindow::popupToXml(EditorMenu *menu, int delta) const
{
    const QString margin(delta, QLatin1Char(' '));
    const QAction *menuAction = menu->menuAction();
    const QString par = menuAction ? menuAction->statusTip() : QString();

    QString xml = margin + QStringLiteral("<popup label=\"%1\" help=\"%2\" comment=\"%3\">\n")
        .arg(xmlEscape(encodeMnemonic(menu->title())))
        .arg(menu->helpId())
        .arg(xmlEscape(par));

    xml += itemsToXml(menu->actions(), delta + 1);
    xml += margin + QStringLiteral("</popup>\n");
    return xml;
}

QString MenuEditorWindow::itemsToXml(const QList<QAction *> &actions, int delta) const
{
    const QString margin(delta, QLatin1Char(' '));
    QString xml;

    for (QAction *a : actions) {
        if (auto *sub = qobject_cast<EditorMenu *>(a->menu())) {
            xml += popupToXml(sub, delta);
            continue;
        }

        if (a->isSeparator()) {
            xml += margin + QStringLiteral("<delim/>\n");
            continue;
        }

        // Плейсхолдеры ("Введите здесь", "Добавить разделитель") — обычные
        // QAction, через qobject_cast<MenuAction*> отсеиваются сами
        auto *ma = qobject_cast<MenuAction *>(a);
        if (!ma)
            continue;

        xml += margin + QStringLiteral("<item label=\"%1\" cmd=\"%2\" help=\"%3\"")
            .arg(xmlEscape(formatItemText(ma->text(), ma->shortcut())))
            .arg(ma->commandId())
            .arg(ma->helpId());

        // Расширенные поля версии 2 — как в старой утилите: атрибуты
        // выводятся только если заданы iconID или flags
        if (ma->iconID() || ma->actionFlags())
            xml += QStringLiteral(" cmdEx=\"%1\" iconID=\"%2\" flags=\"%3\"")
                .arg(ma->cmdEx())
                .arg(ma->iconID())
                .arg(static_cast<quint32>(ma->actionFlags()));

        xml += QStringLiteral(" comment=\"%1\"/>\n").arg(xmlEscape(ma->statusTip()));
    }

    return xml;
}

QString MenuEditorWindow::title() const
{
    if (!m_menuComment.isEmpty())
        return m_menuComment;

    return BaseEditorWindow::title();
}

qint16 MenuEditorWindow::type() const
{
    return m_Type;
}

QString MenuEditorWindow::name() const
{
    return m_menuName;
}
