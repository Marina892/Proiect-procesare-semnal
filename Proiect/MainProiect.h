/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1       /* callback function: OnPanel */
#define  PANEL_GRAPH                      2       /* control type: graph, callback function: (none) */
#define  PANEL_LOAD                       3       /* control type: command, callback function: OnLoad */
#define  PANEL_NUMERIC                    4       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_2                  5       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_3                  6       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_4                  7       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_5                  8       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_6                  9       /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_7                  10      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_8                  11      /* control type: numeric, callback function: (none) */
#define  PANEL_GRAPH_2                    12      /* control type: graph, callback function: (none) */
#define  PANEL_GRAPH_3                    13      /* control type: graph, callback function: (none) */
#define  PANEL_COMMANDBUTTON              14      /* control type: command, callback function: On_Prev */
#define  PANEL_COMMANDBUTTON_2            15      /* control type: command, callback function: On_Next */
#define  PANEL_NUMERIC_9                  16      /* control type: numeric, callback function: (none) */
#define  PANEL_NUMERIC_10                 17      /* control type: numeric, callback function: (none) */
#define  PANEL_RING                       18      /* control type: ring, callback function: OnFilter */
#define  PANEL_NUMERIC_11                 19      /* control type: numeric, callback function: (none) */
#define  PANEL_COMMANDBUTTON_3            20      /* control type: command, callback function: OnApply */
#define  PANEL_COMMANDBUTTON_4            21      /* control type: command, callback function: OnDerivative */
#define  PANEL_COMMANDBUTTON_5            22      /* control type: command, callback function: OnEnvelope */
#define  PANEL_BINARYSWITCH               23      /* control type: binary, callback function: OnSwitchPanel */
#define  PANEL_SAVE                       24      /* control type: command, callback function: OnSave */

#define  PANEL_2                          2       /* callback function: OnPanel_2 */
#define  PANEL_2_BINARYSWITCH             2       /* control type: binary, callback function: OnSwitchPanel */
#define  PANEL_2_BINARYSWITCH_2           3       /* control type: binary, callback function: OnWindowType */
#define  PANEL_2_GRAPH                    4       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_5                  5       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_6                  6       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_4                  7       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_2                  8       /* control type: graph, callback function: (none) */
#define  PANEL_2_GRAPH_3                  9       /* control type: graph, callback function: (none) */
#define  PANEL_2_COMMANDBUTTON            10      /* control type: command, callback function: OnNextButton */
#define  PANEL_2_COMMANDBUTTON_2          11      /* control type: command, callback function: OnPrevButton */
#define  PANEL_2_RING_3                   12      /* control type: ring, callback function: (none) */
#define  PANEL_2_RING_2                   13      /* control type: ring, callback function: OnSelectGraph */
#define  PANEL_2_NUMERIC                  14      /* control type: numeric, callback function: (none) */
#define  PANEL_2_NUMERIC_2                15      /* control type: numeric, callback function: (none) */
#define  PANEL_2_CURENT_WINDOW            16      /* control type: numeric, callback function: (none) */
#define  PANEL_2_NUMBER_OF_WINDOWS        17      /* control type: numeric, callback function: (none) */
#define  PANEL_2_FREQUENCY_PEAK           18      /* control type: numeric, callback function: (none) */
#define  PANEL_2_POWER_PEAK               19      /* control type: numeric, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK On_Next(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK On_Prev(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnApply(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnDerivative(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnEnvelope(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnFilter(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoad(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnNextButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanel(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPanel_2(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnPrevButton(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSave(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSelectGraph(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnSwitchPanel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnWindowType(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif