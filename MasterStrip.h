#ifndef MASTERSTRIP_H
#define MASTERSTRIP_H

#include <QWidget>
#include <QProgressDialog>

#include "config.h"
#include "MasterStrip.h"
#include "ElementStrip.h"
#include "Led.h"
#include "VertLabel.h"

namespace Ui {
class MasterStrip;
}

class MasterStrip : public QWidget
{
    Q_OBJECT

public:
    explicit MasterStrip(QWidget *parent = 0, QList<ElementStrip *> * strips = 0);
    ~MasterStrip();

private:

    Ui::MasterStrip *ui;

    QList<ElementStrip *> * strips;
    int stripsCount;
    int volumeReference;
    void buildConnections();

private slots:

    void slotRevert();
    void slotSave();
    void slotVolumePercentChanged(int value);
    void slotEnabledInvert();

 signals:

    void analizeConflicts();

};

#endif // MASTERSTRIP_H
