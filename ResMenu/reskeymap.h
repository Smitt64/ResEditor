#ifndef RESKEYMAP_H
#define RESKEYMAP_H

#include <QHash>
#include <QKeySequence>
#include <QString>
#include <QVector>

// Таблица соответствия кодов клавиш наследуемого формата меню (enum TAllKeys
// из keys.h, модификаторы зашиты в код) и Qt-сочетаний QKeySequence.
//
// Коды < UserCodeBase — клавиши/служебные коды, >= UserCodeBase (K_USER) —
// пользовательские команды, сочетания для них хранятся отдельно
// (MenuAction::shortcutText).
//
// Таблица загружается один раз из :/json/KeyCodes.json. Записи двух видов:
//   { "code": 350, "seq": "Ctrl+F1" }  — клавиша, участвует в захвате сочетаний
//   { "code": 770, "name": "K_EDIT" }  — служебный код, только имя
// Если у одного кода несколько записей с seq, первая — каноническая
// (используется для code -> QKeySequence), остальные — только для
// обратного поиска (например Ctrl+H == Backspace == 8).
class ResKeyMap
{
public:
    struct Entry
    {
        int code = 0;
        QString seq;   // portable text сочетания (может быть пустым)
        QString name;  // символическое имя служебного кода (может быть пустым)
    };

    static constexpr int UserCodeBase = 1000; // K_USER

    static const QVector<Entry> &entries();

    // -1, если сочетание не представимо кодом клавиши
    static int codeFromKeySequence(const QKeySequence &seq);

    // Пустая последовательность, если код не клавиша (служебный/пользовательский)
    static QKeySequence keySequenceFromCode(int code);

    // Символическое имя ("K_EDIT") или пустая строка
    static QString nameFromCode(int code);

    // Текст для отображения: NativeText сочетания, имя служебного кода
    // или пустая строка для пользовательского/неизвестного кода
    static QString textFromCode(int code);

    static bool isUserCode(int code) { return code >= UserCodeBase; }

private:
    static void ensureLoaded();

    static bool m_loaded;
    static QVector<Entry> m_entries;
    static QHash<int, int> m_seqToCode;  // QKeySequence[0] -> code (обратный поиск)
    static QHash<int, int> m_codeToSeq;  // code -> канонический seq (индекс в m_entries)
};

#endif // RESKEYMAP_H
