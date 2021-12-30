#include "PanicPanel.h"
#include "ui_PanicPanel.h"


PanicPanel::PanicPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PanicPanel)
{
    ui->setupUi(this);
    connect(ui->buttonPanic, SIGNAL(pressed()), MidiManager::get(), SLOT( slotPanic() ) );
}


PanicPanel::~PanicPanel()
{
    delete ui;
}

