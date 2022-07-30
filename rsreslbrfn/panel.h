#ifndef __PANEL_HSS
#define __PANEL_HSS

#include <platform.h>

#define  STR_ERR_LOADING_RESOURCES  _PANEL_STR_DATA1
#define  STR_REQ_BREAK_WORK         _PANEL_STR_DATA2

extern const char RSL_FAR _PANEL_CONFIRM_PRINTREP[];

extern const char RSL_FAR _PANEL_STR_DATA1[];
extern const char RSL_FAR _PANEL_STR_DATA2[];

extern const char RSL_FAR _PANEL_STR_CALC_HEAD[];
extern const char RSL_FAR _PANEL_STR_CALC_LN1 [];
extern const char RSL_FAR _PANEL_STR_CALC_LN2 [];
extern const char RSL_FAR _PANEL_STR_CALC_LN3 [];
extern const char RSL_FAR _PANEL_STR_CALC_LN4 [];
extern const char RSL_FAR _PANEL_STR_CALC_LN2A[];
extern const char RSL_FAR _PANEL_STR_CALC_LN3A[];
extern const char RSL_FAR _PANEL_STR_CALC_LN4A[];
extern const char RSL_FAR _PANEL_STR_CALC_KEYS[];
extern const char RSL_FAR _PANEL_STR_CALC_OVERFLOW[];
extern const char RSL_FAR _PANEL_STR_CALC_DIVBYZERO[];

extern const char RSL_FAR _PANEL_STR_APP1[];
extern const char RSL_FAR _PANEL_STR_APP2[];
extern const char RSL_FAR _PANEL_STR_APP3[];
extern const char RSL_FAR _PANEL_STR_APP4[];
extern const char RSL_FAR _PANEL_STR_APP41[];
extern const char RSL_FAR _PANEL_STR_APP42[];
extern const char RSL_FAR _PANEL_STR_APP43[];
extern const char RSL_FAR _PANEL_STR_APP5[];
extern const char RSL_FAR _PANEL_STR_APP6[];
extern const char RSL_FAR _PANEL_STR_APP60[];

extern const char RSL_FAR _PANEL_STR_RPT1[];
extern const char RSL_FAR _PANEL_STR_RPT2[];

extern const char RSL_FAR _PANEL_STR_NOMEM[];

extern const char RSL_FAR _PANEL_STR_INDC1[];
extern const char RSL_FAR _PANEL_STR_INDC2[];
extern const char RSL_FAR _PANEL_STR_INDC3[];
extern const char RSL_FAR _PANEL_STR_INDC4[];
extern const char RSL_FAR _PANEL_STR_INDC5[];

extern const char RSL_FAR _PANEL_STR_HLP1[];
extern const char RSL_FAR _PANEL_STR_HLP2[];
extern const char RSL_FAR _PANEL_STR_HLP3[];

extern const unsigned char _PANEL_CHAR_LINE ;
extern const unsigned char _PANEL_CHAR_UPPERBAR ;
extern const unsigned char _PANEL_CHAR_LOWERBAR ;

extern const char RSL_FAR * const _PANEL_STR_LEDIT_MSG[];

extern char _PANEL_STR_RES_NAME[];
extern const char *_PANEL_STR_RES_MSG[];

extern const char RSL_FAR _PANEL_STR_CREWIN1[];
extern const char RSL_FAR _PANEL_STR_CREWIN2[];

extern const char RSL_FAR _PANEL_STR_MENULN[];

extern const char RSL_FAR _EXECPGM_STR_1[];
extern const char RSL_FAR _EXECPGM_STR_2[];

extern const char RSL_FAR _RUNPROG_STR_Title[];
extern const char RSL_FAR _RUNPROG_STR_Execute[];
extern const char RSL_FAR _RUNPROG_STR_Finish[];
extern const char RSL_FAR _RUNPROG_STR_Break[];

extern const char RSL_FAR _SHOWV_STR_Status[];
extern const char RSL_FAR _SHOWV_STR_szMV[];
extern const char RSL_FAR _SHOWV_STR_szME[];
extern const char RSL_FAR _SHOWV_STR_szMVt[];
extern const char RSL_FAR _SHOWV_STR_szMEt[];
extern const char RSL_FAR _SHOWV_STR_szCODEDOS[];
extern const char RSL_FAR _SHOWV_STR_szCODEWIN[];
extern const char RSL_FAR _SHOWV_STR_szCODEDOSt[];
extern const char RSL_FAR _SHOWV_STR_szCODEWINt[];
extern const char RSL_FAR _SHOWV_STR_szOVR[];
extern const char RSL_FAR _SHOWV_STR_szINS[];

extern const char RSL_FAR _SHOWV_STR_CLIPLIMIT[];

extern const char RSL_FAR _SHOWV_STR_M1[];
extern const char RSL_FAR _SHOWV_STR_M2[];
extern const char RSL_FAR _SHOWV_STR_M3[];
extern const char RSL_FAR _SHOWV_STR_M4[];
extern const char RSL_FAR _SHOWV_STR_M5[];
extern const char RSL_FAR _SHOWV_STR_M6[];
extern const char RSL_FAR _SHOWV_STR_M7[];
extern const char RSL_FAR _SHOWV_STR_M8[];
extern const char RSL_FAR _SHOWV_STR_M9[];
extern const char RSL_FAR _SHOWV_STR_M10[];
extern const char RSL_FAR _SHOWV_STR_M11[];
extern const char RSL_FAR _SHOWV_STR_M12[];
extern const char RSL_FAR _SHOWV_STR_M13[];
extern const char RSL_FAR _SHOWV_STR_M14[];
extern const char RSL_FAR _SHOWV_STR_M15[];
extern const char RSL_FAR _SHOWV_STR_M16[];
extern const char RSL_FAR _SHOWV_STR_M17[];
extern const char RSL_FAR _SHOWV_STR_M18[];
extern const char RSL_FAR _SHOWV_STR_M19[];
extern const char RSL_FAR _SHOWV_STR_M20[];
extern const char RSL_FAR _SHOWV_STR_M21[];
extern const char RSL_FAR _SHOWV_STR_M23[];
extern const char RSL_FAR _SHOWV_STR_M24[];
extern const char RSL_FAR _SHOWV_STR_M25[];
extern const char RSL_FAR _SHOWV_STR_M26[];
extern const char RSL_FAR _SHOWV_STR_M27[];

extern const char RSL_FAR _SHOWV_STR_M40[];
extern const char RSL_FAR _SHOWV_STR_M41[];
extern const char RSL_FAR _SHOWV_STR_M42[];
extern const char RSL_FAR _SHOWV_STR_M43[];

extern const char RSL_FAR _SHOWV_STR_M60[];
extern const char RSL_FAR _SHOWV_STR_M61[];
extern const char RSL_FAR _SHOWV_STR_M62[];
extern const char RSL_FAR _SHOWV_STR_M63[];
extern const char RSL_FAR _SHOWV_STR_M64[];
extern const char RSL_FAR _SHOWV_STR_M65[];
extern const char RSL_FAR _SHOWV_STR_M66[];
extern const char RSL_FAR _SHOWV_STR_M67[];
extern const char RSL_FAR _SHOWV_STR_M68[];

extern const char RSL_FAR PRNPRMFFSTR[];
extern const char RSL_FAR _SHOWV_STR_M69[];
extern const char RSL_FAR _SHOWV_STR_M70[];
extern const char RSL_FAR _SHOWV_STR_M71[];
extern const char RSL_FAR _SHOWV_STR_M72[];
extern const char RSL_FAR _SHOWV_STR_M73[];
extern const char RSL_FAR _SHOWV_STR_M74[];
extern const char RSL_FAR _SHOWV_STR_M75[];
extern const char RSL_FAR _SHOWV_STR_M76[];
extern const char RSL_FAR _SHOWV_STR_M77[];
extern const char RSL_FAR _SHOWV_STR_M78[];
extern const char RSL_FAR _SHOWV_STR_M79[];
extern const char RSL_FAR _SHOWV_STR_M80[];
extern const char RSL_FAR _SHOWV_STR_M81[];
extern const char RSL_FAR _SHOWV_STR_M82[];
extern const char RSL_FAR _SHOWV_STR_M83[];
extern const char RSL_FAR _SHOWV_STR_M84[];
extern const char RSL_FAR _SHOWV_STR_M85[];
extern const char RSL_FAR _SHOWV_STR_M86[];
extern const char RSL_FAR _SHOWV_STR_M87[];
extern const char RSL_FAR _SHOWV_STR_M88[];
extern const char RSL_FAR _SHOWV_STR_M89[];
extern const char RSL_FAR _SHOWV_STR_M90[];
extern const char RSL_FAR _SHOWV_STR_M91[];
extern const char RSL_FAR _SHOWV_STR_M92[];
extern const char RSL_FAR _SHOWV_STR_M93[];
extern const char RSL_FAR _SHOWV_STR_M94[];
extern const char RSL_FAR _SHOWV_STR_M95[];
extern const char RSL_FAR _SHOWV_STR_M96[];
extern const char RSL_FAR _SHOWV_STR_SEP[];
extern const char RSL_FAR _SHOWV_ALTPRINT[];

extern const char RSL_FAR __NEWFILE_STR[];

extern const char RSL_FAR _SHOWV_STR_46[];
extern const char RSL_FAR _SHOWV_STR_47[];
extern const char RSL_FAR _SHOWV_STR_48[];
extern const char RSL_FAR _SHOWV_STR_49[];
extern const char RSL_FAR _SHOWV_STR_50[];
extern const char RSL_FAR * const _SHOWV_STR_51[];
extern const char RSL_FAR _SHOWV_STR_52[];

extern const char RSL_FAR _FILECPY_STR_T0[];
extern const char RSL_FAR _FILECPY_STR_T1[];
extern const char RSL_FAR _FILECPY_STR_T2[];
extern const char RSL_FAR _FILECPY_STR_T3[];

extern const char RSL_FAR _FILECPY_STR_GETPATH[];
extern const char RSL_FAR _FILECPY_STR_ERRTEXT[];

extern const char RSL_FAR _FILEDLG_STR_W1 [];
extern const char RSL_FAR _FILEDLG_STR_W2 [];
extern const char RSL_FAR _FILEDLG_STR_W3 [];
extern const char RSL_FAR _FILEDLG_STR_W4 [];
extern const char RSL_FAR _FILEDLG_STR_T1 [];
extern const char RSL_FAR _FILEDLG_STR_T2 [];
extern const char RSL_FAR _FILEDLG_STR_NF1[];
extern const char RSL_FAR _FILEDLG_STR_NF2[];
extern const char RSL_FAR _FILEDLG_STR_NF3[];
extern const char RSL_FAR _FILEDLG_STR_NF4[];
extern const char RSL_FAR _FILEDLG_STR_Fs1 [];
extern const char RSL_FAR _FILEDLG_STR_Fs2 [];
extern const char RSL_FAR _FILEDLG_STR_Fs3 [];
extern const char RSL_FAR _FILEDLG_STR_Fs4 [] ;
extern const char RSL_FAR _FILEDLG_STR_Ts0 [];
extern const char RSL_FAR _FILEDLG_STR_Ts1 [];
extern const char RSL_FAR _FILEDLG_STR_Ts2 [];
extern const char RSL_FAR _FILEDLG_STR_Ts3 [];
extern const char RSL_FAR _FILEDLG_STR_Hts [];
extern const char RSL_FAR _FILEDLG_STR_Fm1 [];
extern const char RSL_FAR _FILEDLG_STR_Fm2 [];

extern const char RSL_FAR *_YESNOWIN_BTEXT_R [];
extern const char RSL_FAR *_YESNOWIN_BTEXT_E [];

extern const char RSL_FAR _PROCFLD_STR_0[];

extern const char RSL_FAR _MSGLIST_STR_0[];
extern const char RSL_FAR _MSGLIST_STR_1[];
extern const char RSL_FAR * const _PANEL_STR_CONFMSG[];

extern const char RSL_FAR _VARMENU_STR_VAR[];

extern const char RSL_FAR _VARMENU_STR_SEP[];

extern const char RSL_FAR _VARMENU_STR_MENU1[];
extern const char RSL_FAR _VARMENU_STR_MENU2[];
extern const char RSL_FAR _VARMENU_STR_MENU3[];
extern const char RSL_FAR _VARMENU_STR_MENU4[];
extern const char RSL_FAR _VARMENU_STR_MENU5[];
extern const char RSL_FAR _VARMENU_STR_MENU6[];
extern const char RSL_FAR _VARMENU_STR_MENU7[];
extern const char RSL_FAR _VARMENU_STR_MENU8[];
extern const char RSL_FAR _VARMENU_STR_MENU8[];
extern const char RSL_FAR _VARMENU_STR_MENU9[];
extern const char RSL_FAR _VARMENU_STR_MENU10[];
extern const char RSL_FAR _VARMENU_STR_MENU11[];
extern const char RSL_FAR _VARMENU_STR_MENU12[];
extern const char RSL_FAR _VARMENU_STR_MENU13[];
extern const char RSL_FAR _VARMENU_STR_MENU14[];

extern const char RSL_FAR _VARMENU_STR_HELP1[];
extern const char RSL_FAR _VARMENU_STR_HELP2[];
extern const char RSL_FAR _VARMENU_STR_HELP3[];
extern const char RSL_FAR _VARMENU_STR_HELP4[];
extern const char RSL_FAR _VARMENU_STR_HELP5[];
extern const char RSL_FAR _VARMENU_STR_HELP6[];
extern const char RSL_FAR _VARMENU_STR_HELP7[];
extern const char RSL_FAR _VARMENU_STR_HELP8[];
extern const char RSL_FAR _VARMENU_STR_HELP8[];
extern const char RSL_FAR _VARMENU_STR_HELP9[];
extern const char RSL_FAR _VARMENU_STR_HELP10[];
extern const char RSL_FAR _VARMENU_STR_HELP11[];
extern const char RSL_FAR _VARMENU_STR_HELP12[];
extern const char RSL_FAR _VARMENU_STR_HELP13[];
extern const char RSL_FAR _VARMENU_STR_HELP14[];

extern const char RSL_FAR _SCROL_STR_NOREC[];

extern const char RSL_FAR *_KEYS_IN_MENU[];

extern const char RSL_FAR _MSG_BOX_TITLE[];
extern const char RSL_FAR _INDICATOR_TITLE[];
extern const char RSL_FAR _INWIN_TITLE[];

extern const char RSL_FAR _SCROL_CH_FILT[];

extern const char RSL_FAR  _NOTIFY_HISTORY_HEAD[];

extern const char RSL_FAR  _FIELD_ERR_BUFLEN[];

#endif // __PANEL_HSS

/* <-- EOF --> */
