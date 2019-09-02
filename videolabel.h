#ifndef VIDEOLABEL_H
#define VIDEOLABEL_H

#include <QWidget>
#include <QFileDialog>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <img_widget.h>

class VideoLabel : public QWidget
{
    Q_OBJECT

public:
    VideoLabel(QWidget *parent = 0);
    ~VideoLabel();

    QPushButton *open_dir_btn;

    QString *base_dir;
    QStringList file_lst;

    QVBoxLayout *main_vlayout;
    QLabel *dir_lbl;

    ImgWidget *img_widget;


public slots:
    void open_dir_btn_clicked_slot();

};

#endif // VIDEOLABEL_H
