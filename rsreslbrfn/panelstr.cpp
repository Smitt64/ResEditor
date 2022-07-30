/*******************************************************************************
 FILE         :   PANELSTR.S362

 COPYRIGHT    :   R-Style Software Lab, 1998

 DESCRIPTION  :   String and char literal definitions for PANEL library

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   8 Sep 1998

 VERSION INFO :   $Archive: $
                  $Revision: $
                  $Date: $
                  $Author: $
                  $Modtime: $
*******************************************************************************/

#ifdef __ILEC400__
#  define RSL_FAR
#  define RUSSIAN
#else
#  include "panel.h"
//#  include "keys.h"

#ifndef RSL_MD_FLAT
#pragma option -zEPANEL_DATA
#endif  // __FLAT__
#endif

const char RSL_FAR _PANEL_CONFIRM_PRINTREP [] = "Напечатать отчёт?";

const char RSL_FAR _PANEL_STR_DATA1[] =  "Ошибка при загрузке ресурса|%s|%s" ;
const char RSL_FAR _PANEL_STR_DATA2[] =  "Вы действительно хотите прервать работу?" ;

const char RSL_FAR _PANEL_STR_CALC_HEAD[] = "Калькулятор" ;
const char RSL_FAR _PANEL_STR_CALC_LN1 [] = "────────────────────────────────" ;
const char RSL_FAR _PANEL_STR_CALC_LN2 [] = "\xFFОперации" ;
const char RSL_FAR _PANEL_STR_CALC_LN3 [] = "\xFFРазрядность" ;
const char RSL_FAR _PANEL_STR_CALC_LN4 [] = "\xFFВыход" ;
const char RSL_FAR _PANEL_STR_CALC_LN2A[] = "+ - * / % = C M R S\xFF" ;
const char RSL_FAR _PANEL_STR_CALC_LN3A[] = "Alt - 2 3 4\xFF" ;
const char RSL_FAR _PANEL_STR_CALC_LN4A[] = "Esc или ShiftF1\xFF" ;
const char RSL_FAR _PANEL_STR_CALC_KEYS[] = "mMмМьЬrRкКsSыЫcCсС+-*/%=" ;
const char RSL_FAR _PANEL_STR_CALC_OVERFLOW[]  = "Переполнение!" ;
const char RSL_FAR _PANEL_STR_CALC_DIVBYZERO[] = "Деление на ноль!" ;


const char RSL_FAR _PANEL_STR_APP1[] = "Минимальный размер экрана должен быть 80х25!" ;
const char RSL_FAR _PANEL_STR_APP2[] = "Нажмите любую клавишу для завершения программы..." ;
const char RSL_FAR _PANEL_STR_APP3[] = "%hu.%02hu.%03hu" ;
const char RSL_FAR _PANEL_STR_APP4[] = "%hu.%02hu.%03hu.%hu" ;
const char RSL_FAR _PANEL_STR_APP41[] = "%hu.%02hu.%03hu.%03hu.%02hu";
const char RSL_FAR _PANEL_STR_APP42[] = "%hu.%02hu.%03hu.%03hu (%02hu)";
const char RSL_FAR _PANEL_STR_APP43[] = "%hu.%02hu.%03hu.%03hu.%hu (%s %hu.%hu)";
const char RSL_FAR _PANEL_STR_APP5[] = "Версия: " ;
const char RSL_FAR _PANEL_STR_APP6[] = "Параметры командной строки:\n"
                                       "   /?  - вывести справку\n"
                                       "   /b  - отключить звук\n"
                                       "   /m  - работа в монохромном видеорежиме\n"
                                       "   /n  - работа в видеорежиме ноутбука\n"
                                       "   /w  - работа в EasyWin-режиме\n";

const char RSL_FAR _PANEL_STR_APP60[] = "   /wt - работа в текстовом режиме (по умолчанию)\n"
                                        "   /wd - работа в отладочном EasyWin-режиме\n"
                                        "   /wa - GUI debug appartment mode\n";

const char RSL_FAR _PANEL_STR_RPT1[] = "Выводится отчет ... ~CtrlBrk~ Прервать" ;
const char RSL_FAR _PANEL_STR_RPT2[] = "Ошибка при открытии отчета \"%s\"" ;

const char RSL_FAR _PANEL_STR_NOMEM[] = "~Не хватает памяти для работы программы~ нажмите ~ESC~" ;

const char RSL_FAR _PANEL_STR_INDC1[] = " ~CtrlBrk~ для прекращения обработки" ;
const char RSL_FAR _PANEL_STR_INDC2[] = "Всего:" ;
const char RSL_FAR _PANEL_STR_INDC3[] = "Обработано:" ;
const char RSL_FAR _PANEL_STR_INDC4[] = "Время выполнения:" ;
const char RSL_FAR _PANEL_STR_INDC5[] = "   до завершения:" ;

const char RSL_FAR _PANEL_STR_HLP1[] = "Ошибка загрузки страницы помощи|%s|страница #%d" ;
const char RSL_FAR _PANEL_STR_HLP2[] = "~AltF1~ - предыдущая помощь  ~ESC~ - закончить" ;
const char RSL_FAR _PANEL_STR_HLP3[] = "Помощь" ;

const unsigned char _PANEL_CHAR_LINE      = '─' ;
const unsigned char _PANEL_CHAR_UPPERBAR  = '▀' ;
const unsigned char _PANEL_CHAR_LOWERBAR  = '▄' ;

const char RSL_FAR * const _PANEL_STR_LEDIT_MSG[] = {
   "Вы действительно хотите удалить?",
   "Нет записей",
   " ~Поиск...~",
   "Введите значение поиска",
   "Не найдено"
} ;

const char RSL_FAR _PANEL_STR_MENULN[] = "───────────────────────────────────────────────────────────────────" ;

const char RSL_FAR *_PANEL_STR_RES_MSG[] = {

#ifdef RUSSIAN

# ifdef RSL_ANSI_STR
   "═хтючьюцэю юЄъЁ√Є№ яЁшышэъютрэ√щ ЁхёєЁё",
   "═хтючьюцэю юЄъЁ√Є№ сшсышюЄхъє ЁхёєЁёют",
   "═хтхЁхэ ЇюЁьрЄ Їрщыр",
   "═хтючьюцэю яЁюўшЄрЄ№ ърЄрыюу, Їрщы эх юЄъЁ√Є",
   "═хтючьюцэю яЁюўшЄрЄ№ ърЄрыюу, эхЄ ярь Єш",
   "╬°шсър ўЄхэш  ърЄрыюур",
   "╬°шсър чряшёш ърЄрыюур",
   "╩рЄрыюу яєёЄ",
   "▌ыхьхэЄ ърЄрыюур юЄёєЄёЄтєхЄ",
   "─єсышЁютрэшх ¤ыхьхэЄр ърЄрыюур",
   "═хтючьюцэю ёяючшЎшюэшЁютрЄ№ё  т сшсышюЄхъх",
   "═хтючьюцэю фюсртшЄ№ ¤ыхьхэЄ , эхЄ ярь Єш",
   "═хтючьюцэю яЁюўшЄрЄ№ ЁхёєЁё",
   "═хтючьюцэю чряшёрЄ№ ЁхёєЁё",
   "═хтхЁэ√щ ЁрчьхЁ чряшёрээюую ЁхёєЁёр",
   "═хтхЁэр  тхЁёш  ЁхёєЁёр",
   "╥ЁрэчръЎш  єцх ръЄштэр",
   "╬°шсър т ЄЁрэчръЎшш",
   "╬°шсър Ёрчфхыхэш  Їрщыр эр чряшё№",
   "═хтхЁэ√щ Ёхцшь юЄъЁ√Єш  ЁхёєЁёр",
   "╙ёЄрЁхт°шщ ЇюЁьрЄ Їрщыр"
# else
   "Невозможно открыть прилинкованый ресурс",
   "Невозможно открыть библиотеку ресурсов",
   "Неверен формат файла",
   "Невозможно прочитать каталог, файл не открыт",
   "Невозможно прочитать каталог, нет памяти",
   "Ошибка чтения каталога",
   "Ошибка записи каталога",
   "Каталог пуст",
   "Элемент каталога отсутствует",
   "Дублирование элемента каталога",
   "Невозможно спозиционироваться в библиотеке",
   "Невозможно добавить элемент , нет памяти",
   "Невозможно прочитать ресурс",
   "Невозможно записать ресурс",
   "Неверный размер записанного ресурса",
   "Неверная версия ресурса",
   "Транзакция уже активна",
   "Ошибка в транзакции",
   "Ошибка разделения файла на запись",
   "Неверный режим открытия ресурса",
   "Устаревший формат файла"
# endif
#else

   "Can not open linked resourse",
   "Can not open resource library",
   "Bad resourse file format",
   "Can not read directory - file is closed",
   "Can not read directory - no more memory",
   "Can not reading file",
   "Can not write file",
   "Directory is empty",
   "Missing directory entry",
   "Duplicate object in library",
   "Can not position in library",
   "Can not add object in library - out of memory",
   "Can not read resource",
   "Can not write resource",
   "Bad written resource size",
   "Bad resource version",
   "Transaction already active",
   "Error in transaction",
   "Sharing violation",
   "Bad open mode"

#endif

} ;

//char RSL_FAR _PANEL_STR_RES_NAME[] = "RЕSLIВ";

#ifdef RUSSIAN
const char RSL_FAR _PANEL_STR_CREWIN1[] = " ~О~тказаться ~П~овторить" ;
#else
const char RSL_FAR _PANEL_STR_CREWIN1[] = " ~I~gnore ~R~etry" ;
#endif

const char RSL_FAR _PANEL_STR_CREWIN2[] = "ОIПR" ;

#if !defined(__ILEC400__) && !defined(RSL_PL_AS400)
const unsigned char RSL_FAR rslGraphC[] =
   "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF\xC0\xC1\xC2\xC3"
   "\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7"
   "\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF" ;
#endif

const char RSL_FAR _EXECPGM_STR_1[] = "Выполняется программа...";
const char RSL_FAR _EXECPGM_STR_2[] = " ~Ctrl-Break~ Прервать";

const char RSL_FAR _RUNPROG_STR_Title[] =    "Выполнение программы";
const char RSL_FAR _RUNPROG_STR_Execute[] =  " Выполнение программы... ~Ctrl-Break~ Прервать";
const char RSL_FAR _RUNPROG_STR_Finish[] =   " Программа выполнена";
const char RSL_FAR _RUNPROG_STR_Break[] =    " Программа прервана";


const char RSL_FAR _SHOWV_STR_Status[] = " ~F2~ Блок  ~F3~ Позиция  ~F4~ Поиск  ~F7~ Печать  ~F10~ Меню  ~ESC~ Выход" ;
const char RSL_FAR _SHOWV_STR_szMV  [] = "Просмотр" ;
const char RSL_FAR _SHOWV_STR_szME  [] = "Редактор" ;
const char RSL_FAR _SHOWV_STR_szMVt [] = "Просмотр текстового файла" ;
const char RSL_FAR _SHOWV_STR_szMEt [] = "Редактирование текстового файла" ;
const char RSL_FAR _SHOWV_STR_szCODEDOS [] = "DOS" ;
const char RSL_FAR _SHOWV_STR_szCODEWIN [] = "WIN" ;
const char RSL_FAR _SHOWV_STR_szCODEDOSt[] = "Кодовая страница DOS" ;
const char RSL_FAR _SHOWV_STR_szCODEWINt[] = "Кодовая страница Windows" ;
const char RSL_FAR _SHOWV_STR_szOVR[] = "OVR" ;
const char RSL_FAR _SHOWV_STR_szINS[] = "INS" ;


const char RSL_FAR _SHOWV_STR_CLIPLIMIT[] = "Превышен размер данных для помещения в буфер обмена!" ;

const char RSL_FAR _SHOWV_STR_M1[] = "Обрабатывается строка %ld" ;
const char RSL_FAR _SHOWV_STR_M2[] = "Введите номер строки" ;
const char RSL_FAR _SHOWV_STR_M3[] = "Введите строку для поиска" ;
const char RSL_FAR _SHOWV_STR_M4[] = "Введите строку для замены" ;
const char RSL_FAR _SHOWV_STR_M5[] = "Введите строку для поиска" ;
const char RSL_FAR _SHOWV_STR_M6[] = "Нет строки для поиска" ;
const char RSL_FAR _SHOWV_STR_M7[] = "Строка не найдена" ;
const char RSL_FAR _SHOWV_STR_M8[] = "Изменений не было" ;
const char RSL_FAR _SHOWV_STR_M9[] = "Сохранить %s ?" ;
const char RSL_FAR _SHOWV_STR_M10[] = "Сохранено ~%.0lf%%~ файла %.40s" ;
const char RSL_FAR _SHOWV_STR_M11[] = "Файл записан" ;
const char RSL_FAR _SHOWV_STR_M12[] = "Файл %s существует.Перезаписать ?" ;
const char RSL_FAR _SHOWV_STR_M13[] = "Выполнено %d замен" ;
const char RSL_FAR _SHOWV_STR_M14[] = "Ошибка записи файла %s" ;
const char RSL_FAR _SHOWV_STR_M15[] = "Строка удалена в карман" ;
const char RSL_FAR _SHOWV_STR_M16[] = "Блок удален в карман" ;
const char RSL_FAR _SHOWV_STR_M17[] = "Строка скопирована в карман" ;
const char RSL_FAR _SHOWV_STR_M18[] = "Блок скопирован в карман" ;
const char RSL_FAR _SHOWV_STR_M19[] = "Блок удален" ;
const char RSL_FAR _SHOWV_STR_M20[] = "Блок вставлен из кармана" ;
const char RSL_FAR _SHOWV_STR_M21[] = "Заменить ?" ;
const char RSL_FAR _SHOWV_STR_M23[] = "Новый файл" ;
const char RSL_FAR _SHOWV_STR_M24[] = "Печать прервана" ;
const char RSL_FAR _SHOWV_STR_M25[] = "Печать закончена" ;
const char RSL_FAR _SHOWV_STR_M26[] = "Печатаю ..." ;
const char RSL_FAR _SHOWV_STR_M27[] = "Ошибка чтения файла %s, нажмите ~ESC~" ;

const char RSL_FAR _SHOWV_STR_M40[] = "Ошибка записи файла|%s" ;
const char RSL_FAR _SHOWV_STR_M41[] = "Ошибка открытия файла|%s" ;
const char RSL_FAR _SHOWV_STR_M42[] = "Ошибка (%d) записи файла|%s";
const char RSL_FAR _SHOWV_STR_M43[] = "Ошибка (%d) записи файла %s";

const char RSL_FAR _SHOWV_STR_M60[] = " Сохранить ~к~ак...       Alt-O " ;
const char RSL_FAR _SHOWV_STR_M61[] = " Блок ~п~отоком           Alt-A " ;
const char RSL_FAR _SHOWV_STR_M62[] = " Блок ~с~троками             F2 " ;
const char RSL_FAR _SHOWV_STR_M63[] = " Блок ко~л~онками         Alt-C " ;
const char RSL_FAR _SHOWV_STR_M64[] = " ~О~тмена блока         Ctrl-F2 " ;
const char RSL_FAR _SHOWV_STR_M65[] = " ~Р~едактировать         Alt-F9 " ;
const char RSL_FAR _SHOWV_STR_M87[] = " С~к~рыть <!---->           F12 " ;

const char RSL_FAR _SHOWV_STR_M66[] = " ~Н~айти            F4 " ;
const char RSL_FAR _SHOWV_STR_M67[] = " ~П~овторить   Ctrl-F4 " ;
const char RSL_FAR _SHOWV_STR_M68[] = " ~С~трока           F3 " ;

const char RSL_FAR PRNPRMFFSTR[]    = "Перевод формата [ ]";
const char RSL_FAR _SHOWV_STR_M69[] = "Инициализирующая последовательность" ;
const char RSL_FAR _SHOWV_STR_M70[] = "Число экземпляров в ширину" ;
const char RSL_FAR _SHOWV_STR_M71[] = "Устройство вывода" ;
const char RSL_FAR _SHOWV_STR_M72[] = "Дополнительный файл заголовка" ;
const char RSL_FAR _SHOWV_STR_M73[] = "Внешний редактор" ;
const char RSL_FAR _SHOWV_STR_M74[] = " ~У~становить" ;
const char RSL_FAR _SHOWV_STR_M75[] = "   ~В~ыход" ;
const char RSL_FAR _SHOWV_STR_M76[] =  " ~Р~ежимы печати  Alt-P " ;
const char RSL_FAR _SHOWV_STR_M77[] =  " ~П~ечатать блок     F7 " ;
const char RSL_FAR _SHOWV_ALTPRINT[] = " ~А~льт. печать  Alt-F7 " ;
const char RSL_FAR _SHOWV_STR_M78[] = " ~К~алькулятор  Alt-F1 " ;
const char RSL_FAR _SHOWV_STR_M79[] = " ~В~ыход           Esc " ;
const char RSL_FAR _SHOWV_STR_M80[] = " ~Р~едактирование " ;
const char RSL_FAR _SHOWV_STR_M81[] = " ~П~оиск " ;
const char RSL_FAR _SHOWV_STR_M82[] = " Пе~ч~ать " ;
const char RSL_FAR _SHOWV_STR_M83[] = " Р~а~зное " ;
const char RSL_FAR _SHOWV_STR_M84[] = "Новое имя файла" ;
const char RSL_FAR _SHOWV_STR_M85[] = "Ошибка изменения имени" ;
const char RSL_FAR _SHOWV_STR_M86[] = "" ;
const char RSL_FAR _SHOWV_STR_M88[] = "Имя файла баннера";
const char RSL_FAR _SHOWV_STR_M89[] = "Такого файла не существует";
const char RSL_FAR _SHOWV_STR_M90[] = "Неудачный ввод имени файла баннера";
const char RSL_FAR _SHOWV_STR_M91[] = "Невозможно открыть файл на запись" ;
const char RSL_FAR _SHOWV_STR_M92[] = "Файл был конкурентно изменен. Повторите операцию." ;
const char RSL_FAR _SHOWV_STR_M93[] = "Изменение отменено" ;
const char RSL_FAR _SHOWV_STR_M94[] = "Нечего отменять" ;
const char RSL_FAR _SHOWV_STR_M95[] = " П~е~реключить  DOS/WIN      F8 " ;
const char RSL_FAR _SHOWV_STR_M96[] = " Переслать на ~т~ерминал  Alt-S " ;
const char RSL_FAR _SHOWV_STR_SEP[] = " ──────────────────────────── ";

const char RSL_FAR __NEWFILE_STR[] = "Новый файл";

const char RSL_FAR _SHOWV_STR_46[] = " ~F1~ Помощь  ~F3~ Выбор  ~F9~ Ввод  ~ESC~ Отказ  ~Space~ Изменить" ;
const char RSL_FAR _SHOWV_STR_47[] = "Режимы печати" ;
const char RSL_FAR _SHOWV_STR_48[] = "Вы действительно хотите отказаться от своих изменений?" ;
const char RSL_FAR _SHOWV_STR_49[] = " ~С~охранить" ;
const char RSL_FAR _SHOWV_STR_50[] = "Должно быть число >0.";
const char RSL_FAR * const _SHOWV_STR_51[] = {"Параметры были изменены. Установить?",
                                              "Параметры были изменены. Сохранить?" } ;
const char RSL_FAR _SHOWV_STR_52[] = " ~F1~ Помощь  ~F3~ Выбор  ~F9~ Установить ~Ctrl-F9~ Сохранить  ~ESC~ Отказ ~Space~ Изменить" ;


const char RSL_FAR _FILECPY_STR_T0[] =  "Файл %s уже существует" ;
const char RSL_FAR _FILECPY_STR_T1[] =  "Исходный:" ;
const char RSL_FAR _FILECPY_STR_T2[] = "Перезаписываемый:" ;
const char RSL_FAR _FILECPY_STR_T3[] = "Вы действительно хотите перезаписать существующий файл?" ;

const char RSL_FAR _FILECPY_STR_GETPATH[] = "Введите путь для сохранения файла на терминале" ;
const char RSL_FAR _FILECPY_STR_ERRTEXT[] = "Ошибка пересылки файла на терминал" ;


#ifdef RUSSIAN
const char RSL_FAR _FILEDLG_STR_W1 [] = "Диалог выбора файла" ;
const char RSL_FAR _FILEDLG_STR_W2 [] = " Чтение %s%s" ;
const char RSL_FAR _FILEDLG_STR_W3 [] = " Сортировка~...~" ;
const char RSL_FAR _FILEDLG_STR_W4 [] = "Путь не существует|%s" ;
const char RSL_FAR _FILEDLG_STR_T1 [] = " Диски ";
const char RSL_FAR _FILEDLG_STR_T2 [] = " Файлы " ;
const char RSL_FAR _FILEDLG_STR_Fs1 [] = "  ~В~ыбор   " ;
const char RSL_FAR _FILEDLG_STR_Fs2 [] = "  ~О~тказ   " ;
const char RSL_FAR _FILEDLG_STR_Fs3 [] = " ~П~росмотр " ;
const char RSL_FAR _FILEDLG_STR_Fs4 [] = " П~р~осмотр ОС " ;
const char RSL_FAR _FILEDLG_STR_Ts0 [] = "Имя        ( )" ;
const char RSL_FAR _FILEDLG_STR_Ts1 [] = "Расширение ( )" ;
const char RSL_FAR _FILEDLG_STR_Ts2 [] = "Время      ( )" ;
const char RSL_FAR _FILEDLG_STR_Ts3 [] = "Размер     ( )" ;
const char RSL_FAR _FILEDLG_STR_Hts [] = "Сортировка" ;
const char RSL_FAR _FILEDLG_STR_Fm1 [] = "<Папка>" ;
const char RSL_FAR _FILEDLG_STR_Fm2 [] = "<Вверх>" ;
#else
const char RSL_FAR _FILEDLG_STR_W1 [] = "Open File" ;
const char RSL_FAR _FILEDLG_STR_W2 [] = " Reading %s%s" ;
const char RSL_FAR _FILEDLG_STR_W3 [] = " Sorting~...~" ;
const char RSL_FAR _FILEDLG_STR_W4 [] = "Path not exist|%s" ;
const char RSL_FAR _FILEDLG_STR_T1 [] = " Drives ";
const char RSL_FAR _FILEDLG_STR_T2 [] = " Files " ;
const char RSL_FAR _FILEDLG_STR_Fs1 [] = "   ~O~pen  " ;
const char RSL_FAR _FILEDLG_STR_Fs2 [] = "  ~C~ancel " ;
const char RSL_FAR _FILEDLG_STR_Fs3 [] = "   ~V~iew  " ;
const char RSL_FAR _FILEDLG_STR_Fs4 [] = "   Vie~w~ OS  " ;
const char RSL_FAR _FILEDLG_STR_Ts0 [] = "Name       ( )" ;
const char RSL_FAR _FILEDLG_STR_Ts1 [] = "Extension  ( )" ;
const char RSL_FAR _FILEDLG_STR_Ts2 [] = "Time       ( )" ;
const char RSL_FAR _FILEDLG_STR_Ts3 [] = "Size       ( )" ;
const char RSL_FAR _FILEDLG_STR_Hts [] = "Sort by" ;
const char RSL_FAR _FILEDLG_STR_Fm1 [] = "<Folder>" ;
const char RSL_FAR _FILEDLG_STR_Fm2 [] = "<  Up  >" ;
#endif

const char RSL_FAR _FILEDLG_STR_NF1[] = "Объект";
const char RSL_FAR _FILEDLG_STR_NF2[] = "Размер";
const char RSL_FAR _FILEDLG_STR_NF3[] = "Дата/Время";
const char RSL_FAR _FILEDLG_STR_NF4[] = "Диски";

const char RSL_FAR *_YESNOWIN_BTEXT_R [] = {  " ~Н~ет ", "  ~Д~а " } ;
const char RSL_FAR *_YESNOWIN_BTEXT_E [] = {  " ~N~o  ", " ~Y~es " } ;

const char RSL_FAR _PROCFLD_STR_0[] = "Сообщение сервера приложений";

const char RSL_FAR _MSGLIST_STR_0[] = "~ESC~ Выход";
const char RSL_FAR _MSGLIST_STR_1[] = "    OK   ";

const char RSL_FAR * const _PANEL_STR_CONFMSG[] =
{
   "    Да    ",
   "    Нет   ",
   "Продолжить"
} ;

const char RSL_FAR _VARMENU_STR_VAR[] = " Р~а~зное " ;

const char RSL_FAR _VARMENU_STR_SEP[]    = " ─────────────────────────────── ";

const char RSL_FAR _VARMENU_STR_MENU1[]  = " Помо~щ~ь                        F1 ";
const char RSL_FAR _VARMENU_STR_MENU2[]  = " ~К~алькулятор               Alt-F1 ";
const char RSL_FAR _VARMENU_STR_MENU3[]  = " К~а~лькулятор              Ctrl-F1 ";
const char RSL_FAR _VARMENU_STR_MENU4[]  = " ~В~ыход                        Esc ";
                                         //" ─────────────────────────────── ";
const char RSL_FAR _VARMENU_STR_MENU5[]  = " ~Б~ыстрый фильтр      Alt-Shift-F4 ";
const char RSL_FAR _VARMENU_STR_MENU6[]  = " ~Ф~ильтр              Alt-Shift-F5 ";
const char RSL_FAR _VARMENU_STR_MENU7[]  = " ~С~ортировка          Alt-Shift-F6 ";
                                         //" ─────────────────────────────── ";
const char RSL_FAR _VARMENU_STR_MENU8[]  = " ~П~ечать списка       Alt-Shift-F7 ";
                                         //" ─────────────────────────────── ";
const char RSL_FAR _VARMENU_STR_MENU9[]  = " Выделить ~з~апись          Alt-Ins ";
const char RSL_FAR _VARMENU_STR_MENU10[] = " Выделить вс~е~           Alt-Gray+ ";
const char RSL_FAR _VARMENU_STR_MENU11[] = " Сн~я~ть выделение        Alt-Gray- ";
const char RSL_FAR _VARMENU_STR_MENU14[] = " Об~р~аботка записей  Alt-Shift-F12 ";
                                         //" ─────────────────────────────── ";
const char RSL_FAR _VARMENU_STR_MENU12[] = " ~О~бновить список           Ctrl-R ";
const char RSL_FAR _VARMENU_STR_MENU13[] = " ~Э~кспорт в Excel    Alt-Shift-F11 ";


const char RSL_FAR _VARMENU_STR_HELP1[]  = " Контекстная подсказка" ;
const char RSL_FAR _VARMENU_STR_HELP2[]  = " Калькулятор" ;
const char RSL_FAR _VARMENU_STR_HELP3[]  = " Калькулятор со взятием значения из поля" ;
const char RSL_FAR _VARMENU_STR_HELP4[]  = " Выход в предыдущий режим" ;

const char RSL_FAR _VARMENU_STR_HELP5[]  = " Отфильтровать записи по значению текущей колонки" ;
const char RSL_FAR _VARMENU_STR_HELP6[]  = " Просмотр и изменение условий фильтрации записей" ;
const char RSL_FAR _VARMENU_STR_HELP7[]  = " Просмотр и изменение сортировки записей" ;

const char RSL_FAR _VARMENU_STR_HELP8[]  = " Распечатать все или выделенные записи" ;
const char RSL_FAR _VARMENU_STR_HELP13[]  = " Экспортировать все или выделенные записи в Excel" ;

const char RSL_FAR _VARMENU_STR_HELP9[]  = " Выделить текущую запись или наоборот снять с нее выделение" ;
const char RSL_FAR _VARMENU_STR_HELP10[] = " Выделить все записи в списке" ;
const char RSL_FAR _VARMENU_STR_HELP11[] = " Снять выделение со всех записей в списке" ;
const char RSL_FAR _VARMENU_STR_HELP14[] = " Обработать выделенные записи пользовательским макросом ";

const char RSL_FAR _VARMENU_STR_HELP12[] = " Прочитать все записи в списке из базы данных ";

const char RSL_FAR _SCROL_STR_NOREC[] = "Нет записей";

const char RSL_FAR *_KEYS_IN_MENU[] = {
   " Ctrl-A ",    /* 1 */
   " Ctrl-B ",
   " Ctrl-C ",
   " Ctrl-D ",
   " Ctrl-E ",
   " Ctrl-F ",
   " Ctrl-G ",
   " Ctrl-H ",
   " Ctrl-I ",
   " Ctrl-J ",
   " Ctrl-K ",
   " Ctrl-L ",
   " Ctrl-M ",
   " Ctrl-N ",
   " Ctrl-O ",
   " Ctrl-P ",
   " Ctrl-Q ",
   " Ctrl-R ",
   " Ctrl-S ",
   " Ctrl-T ",
   " Ctrl-U ",
   " Ctrl-V ",
   " Ctrl-W ",
   " Ctrl-X ",
   " Ctrl-Y ",
   " Ctrl-Z ",    /* 26 */

   " Alt-Q ",     /* 272 - 27 */
   " Alt-W ",
   " Alt-E ",
   " Alt-R ",
   " Alt-T ",
   " Alt-Y ",
   " Alt-U ",
   " Alt-I ",
   " Alt-O ",
   " Alt-P ",     /* 281 - 36 */

   " Alt-A ",     /* 286 - 37 */
   " Alt-S ",
   " Alt-D ",
   " Alt-F ",
   " Alt-G ",
   " Alt-H ",
   " Alt-J ",
   " Alt-K ",
   " Alt-L ",     /* 294 - 45 */

   " Alt-Z ",     /* 300 - 46 */
   " Alt-X ",
   " Alt-C ",
   " Alt-V ",
   " Alt-B ",
   " Alt-N ",
   " Alt-M ",     /* 306 - 52 */

   " F1 ",        /* 315 - 53 */
   " F2 ",
   " F3 ",
   " F4 ",
   " F5 ",
   " F6 ",
   " F7 ",
   " F8 ",
   " F9 ",
   " F10 ",       /* 324 - 62 */

   " Shft-F1 ",   /* 340 - 63 */
   " Shft-F2 ",
   " Shft-F3 ",
   " Shft-F4 ",
   " Shft-F5 ",
   " Shft-F6 ",
   " Shft-F7 ",
   " Shft-F8 ",
   " Shft-F9 ",
   " Shft-F10 ",
   " Ctrl-F1 ",
   " Ctrl-F2 ",
   " Ctrl-F3 ",
   " Ctrl-F4 ",
   " Ctrl-F5 ",
   " Ctrl-F6 ",
   " Ctrl-F7 ",
   " Ctrl-F8 ",
   " Ctrl-F9 ",
   " Ctrl-F10 ",
   " Alt-F1 ",
   " Alt-F2 ",
   " Alt-F3 ",
   " Alt-F4 ",
   " Alt-F5 ",
   " Alt-F6 ",
   " Alt-F7 ",
   " Alt-F8 ",
   " Alt-F9 ",
   " Alt-F10 ",   /* 369 - 92 */

   " Alt-1 ",     /* 376 - 93 */
   " Alt-2 ",
   " Alt-3 ",
   " Alt-4 ",
   " Alt-5 ",
   " Alt-6 ",
   " Alt-7 ",
   " Alt-8 ",
   " Alt-9 ",
   " Alt-0 ",     /* 385 - 102 */

   " F11 ",       /* 389 - 103 */
   " F12 ",
   " Shft-F11 ",
   " Shft-F12 ",
   " Ctrl-F11 ",
   " Ctrl-F12 ",
   " Alt-F11 ",
   " Alt-F12 ",   /* 396 - 110 */
};


#ifdef RUSSIAN
    const char  _INDICATOR_TITLE[] = "Индикатор";
    const char  _MSG_BOX_TITLE[]   = "Сообщение";
    const char  _INWIN_TITLE[]     = "Окно ввода";
#else
    const char  _INDICATOR_TITLE[] = "Indicator";
    const char  _MSG_BOX_TITLE[]   = "Message";
    const char  _INWIN_TITLE[]     = "Input box";
#endif

const char RSL_FAR  _SCROL_CH_FILT[] = " Ф ";

const char RSL_FAR  _NOTIFY_HISTORY_HEAD[] = "История уведомлений [%d из %d]";

const char RSL_FAR  _FIELD_ERR_BUFLEN[] = "Некорректная длина буфера поля";

/* <-- EOF --> */
