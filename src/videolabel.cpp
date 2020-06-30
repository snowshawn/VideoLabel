#include "videolabel.h"
#include <QDebug>

VideoLabel::VideoLabel(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(720, 480);
    main_vlayout = new QVBoxLayout;
    setLayout(main_vlayout);

    QHBoxLayout *dir_hlayout = new QHBoxLayout;

    base_dir = new QString("/home/shawn/Documents/datasets/MOT_PIG/test/MOT17-06-FRCNN");

    open_dir_btn = new QPushButton("Chose New Dir");
    open_dir_btn->setMaximumSize(150,30);
    connect(open_dir_btn, SIGNAL(clicked(bool)), this, SLOT(open_dir_btn_clicked_slot()));

    dir_lbl = new QLabel;
    dir_lbl->setText(*base_dir);
    dir_hlayout->addWidget(open_dir_btn);
    dir_hlayout->addWidget(dir_lbl);

    img_widget = new ImgWidget(this, base_dir);
    main_vlayout->addLayout(dir_hlayout);
    main_vlayout->addWidget(img_widget);

}

VideoLabel::~VideoLabel()
{

}

void VideoLabel::open_dir_btn_clicked_slot()
{
    QString  path= QFileDialog::getExistingDirectory(this,"Open Dir", "/home/shawn/Documents/dataset");
    if(!path.isEmpty())
    {
        *base_dir =path;
        qDebug() << *base_dir;
        dir_lbl->setText(*base_dir);
        delete img_widget;
        img_widget = new ImgWidget(this, base_dir);
        main_vlayout->addWidget(img_widget);
    }

}
