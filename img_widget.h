#ifndef IMG_WIDGET_H
#define IMG_WIDGET_H

#include <QWidget>
#include <QDir>
#include <QLabel>
#include <QPushButton>
#include <QLayout>
#include <QKeyEvent>
#include <QTextStream>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QImage>
#include <QMenu>
#include <vector>
#include "img_label.h"
#include "pubheader.h"

using namespace std;

class ImgWidget : public QWidget
{
    Q_OBJECT
public:
    ImgWidget(QWidget *parent = nullptr, QString *dir = new QString("/home/shawn/Documents/dataset/MOT17-01-FRCNN"));
    ~ImgWidget();

    void keyPressEvent(QKeyEvent *event);

    /*Layout*/
    QVBoxLayout *main_vlayout;
    QHBoxLayout *name_hlayout;
    QHBoxLayout *img_menu_hlayout;
    QHBoxLayout *btn_hlayout;

    /*read image and display*/
    QStringList fltr_lst = QStringList("*.jpg");        //* 星号是必需的否则不能读到文件
    QString base_dir;
    QDir qbase_dir;

    QDir qdet_dir;
    QDir qgt_dir;
    QFile *gt_file;
    QStringList file_lst;
    QDir qimg_dir;

    QLabel *display_name_lbl;
    ImgLabel *display_img_lbl;
    QPushButton *save_btn;
    QPushButton *next_img_btn;
    QPushButton *pre_img_btn;
    QImage img;
    int img_idx = 0;
    QMenu *img_menu;

    /*set ID*/
    QLineEdit *id_edit;
    QComboBox *id_box;
    /*draw the det rectangle*/
    QFile *det_file;
    QTextStream *all_det_result;

    /*draw rectangle manually*/
    QPushButton *manual_rect_btn;
    QPushButton *click_btn;

    void strlst_to_objdet(QStringList strlst, ObjDet &det);
signals:
    void next_img();
    void pre_img();

public slots:
    void pre_img_btn_clicked_slot();
    void next_img_btn_clicked_slot();
    void id_box_changed_slot(QString id);
    void set_id_slot(int rect_idx, int id);
    void del_id_slot(int rect_idx);
    void image_display(int &img_idx);
    void save_btn_clicked_slot();
    void manual_rect_btn_clicked_slot();
    void click_btn_clicked_slot();
    //void add_rect_slot(QRect add_rect);
    //void del_rect_slot(int);
	//
};

#endif // IMG_WIDGET_H
