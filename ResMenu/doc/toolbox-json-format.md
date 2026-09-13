# Пользовательские элементы ToolBox (json)

> Пользовательские инструкции со списками свойств и значений:
> [редактор меню](../toolbox/menu/toolbox-menu-elements.md),
> [редактор панелей](../../rsbaseeditor/toolbox/panels/toolbox-panel-elements.md).

К встроенному набору элементов ToolBox (qrc-файл из `CLASSINFO_TOOLBOX_FILE`)
можно добавлять свои — json-файлами на диске, без пересборки плагинов.

## Каталоги поиска

Редактор объявляет идентификатор toolbox (`BaseEditorWindow::toolBoxId()`):
`"menu"` — редактор меню, `"panels"` — редактор панелей. Сканируются три
каталога (все `*.json`, алфавитный порядок, семантика аддитивная):

| Приоритет | Каталог |
|---|---|
| 1 | `%APPDATA%/<app>/toolbox/<id>` |
| 2 | `<cwd>/reseditor/toolbox/<id>` |
| 3 | `<appdir>/reseditor/toolbox/<id>` |

Инсталлятор раскладывает примеры в каталог 3 (`reseditor/toolbox/<id>`).
Файлы подхватываются при открытии нового окна редактора (hot-reload в уже
открытом окне нет).

## Формат файла

Тот же, что у встроенных `MenuToolBox.json` / `StdPanelEditorToolBox.json`:

```json
{
    "groups": [
        { "id": 100, "title": "Мои элементы" },
        { "id": 1,   "title": "Стандартные команды" }
    ],
    "templates": [
        {
            "alias": "Моя команда",
            "icon": "theme:Execute",
            "tooltip": "Подсказка в списке ToolBox",
            "mimetype": "application/x-menueditor-template",
            "groups": [100, 1],
            "mimedata": { ... }
        }
    ]
}
```

- `groups` — объявление групп. Если группа с таким `title` уже существует
  (встроенная или из ранее загруженного файла), она **переиспользуется** —
  так пользовательские элементы можно класть во встроенные группы;
- `groups` элемента — список id групп, куда попадёт элемент;
- `icon`:
  - `"theme:Имя"` — иконка из иконочной темы (`QIcon::fromTheme`);
  - `"\":/...\""` — qrc-ресурс;
  - относительный путь — резолвится от каталога json-файла (свои svg
    можно класть рядом с json);
- `mimetype` + `mimedata` — содержимое перетаскивания; mimetype должен
  соответствовать редактору (см. ниже), чужой mimetype drop-цель игнорирует.

## mimetype редакторов

### Редактор меню — `application/x-menueditor-template`

`mimedata` — шаблон пункта меню (формат `insertMenuTemplate`):

```json
{ "type": "item",     "action": { <MenuAction::serialize()> } }
{ "type": "separator" }
{ "type": "submenu",  "action": {"text", "statusTip", "helpId"},
  "items": [ ... ] }
```

Поля `action` пункта: `text`, `enabled`, `visible`, `shortcut`,
`statusTip`, `helpId`, `commandId`, `cmdEx`, `iconID`, `flags`
(бит 0x01 — `dFMENU_TOOLBAR`, 0x02 — `dFMENU_CONTEXT`,
0x04 — `DISINEMPTYSCROL`).

Пример: `ResMenu/toolbox/menu/example_toolbox.json`.

### Редактор панелей — `application/toolboxitem`

`mimedata` — `{ "items": [ { "class", "parent", "uuid", "properties" } ] }`,
как в `StdPanelEditorToolBox.json`.

Пример: `rsbaseeditor/toolbox/panels/example_toolbox.json`.

## Ограничения

- Переопределить или удалить встроенный элемент нельзя — только добавлять;
- `includes` поддерживаются (как и во встроенных файлах), пути — как есть
  (qrc или абсолютные);
- невалидный json пропускается с предупреждением в лог.

## Кастомная отрисовка плашки перетаскивания

По умолчанию `ToolBoxTreeView` рисует плашку перетаскивания в общем
office-стиле (голубая заливка, иконка + alias). Плагин может переопределить
`BaseEditorWindow::toolBoxDragPixmap(const QModelIndex &index) const` —
непустой `QPixmap` используется вместо стандартной отрисовки (пример:
`StdPanelEditor` возвращает просто иконку элемента 24x24 — как рисовалось
до появления office-плашки; прозрачный 1x1 = «без плашки»).
