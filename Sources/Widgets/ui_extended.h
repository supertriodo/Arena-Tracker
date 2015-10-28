#ifndef UI_EXTENDED
#define UI_EXTENDED

#include "ui_mainwindow.h"
#include "movelistwidget.h"
#include "movetabwidget.h"
#include "movetreewidget.h"
#include "resizebutton.h"

QT_BEGIN_NAMESPACE

class Ui_Extended : public Ui_MainWindow
{
public:
    MoveTabWidget *tabWidget = NULL;
    MoveTabWidget *tabWidgetH2 = NULL;
    MoveTabWidget *tabWidgetH3 = NULL;
    MoveTabWidget *tabWidgetV1 = NULL;
    MoveListWidget *enemyHandListWidget = NULL;
    MoveListWidget *deckListWidget = NULL;
    MoveListWidget *drawListWidget = NULL;
    MoveTreeWidget *secretsTreeWidget = NULL;
    MoveTreeWidget *arenaTreeWidget = NULL;
    ResizeButton *resizeButton = NULL;
};

namespace Ui {
    class Extended: public Ui_Extended {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTENDED

