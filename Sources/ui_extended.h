#ifndef UI_EXTENDED
#define UI_EXTENDED

#include "ui_mainwindow.h"
#include "movelistwidget.h"

QT_BEGIN_NAMESPACE

class Ui_Extended : public Ui_MainWindow
{
public:
    MoveListWidget *enemyHandListWidget, *deckListWidget;
};

namespace Ui {
    class Extended: public Ui_Extended {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTENDED

