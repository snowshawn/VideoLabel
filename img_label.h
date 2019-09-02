#ifndef IMG_LABEL_H
#define IMG_LABEL_H
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QBrush>
#include <QAction>
#include <QMenu>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QIntValidator>
#include <vector>
#include <QMessageBox>
#include "pubheader.h"

class ImgLabel : public QLabel
{
    Q_OBJECT

public:
    ImgLabel(QLabel *parent=0);
    ~ImgLabel();
    int index;        //该张图片在all_det_vec中的编号
    /*draw rectangle*/
    QImage *img;
    QImage *original_img;
    QPainter *rect_painter; 
    QPen *rect_pen;     //用于画边界框

    /*draw id*/
    QPainter *id_painter;
    QPen *id_pen;       //用于标ID
    QBrush *fill_brush;
    QBrush *erase_brush;
    QFont *id_font;

    QLabel *id_mask_widget;    //用于显示ID号的控件
    QImage *id_mask;           //显示ID号的遮罩层
    QImage filled_mask;

    ImgDetVec *img_det;
    QVector<QRect> rect_vec;    //该张图片所有的边界框

    int set_id = 0;
    QMessageBox *error_box;
    QPoint point=QPoint(0,0);
    int mouse_position=0;

    /*right button menu*/
    QAction *del_id;
    QAction *change_id;
    QAction *del_rect;
    QMenu *right_menu;
    QMenu *add_rect_right_menu;
    QDialog *dialog;
    QLineEdit *line_edit;

    /*add rectangle*/
    bool manual_draw = false;
    bool start_draw = false;
    QLabel *add_rect_widget;
    QLabel *cursor_widget;
    QImage *add_rect_mask;
    QPoint start_point;
    QPoint end_point;
    QPainter *cursor_painter;
    QPen *cursor_pen;

    //void img_rect_diaplay(int idx, QImage *img, ImgDetVec *img_det);    //显示图片及目标边界框
    //void img_rect_diaplay(int idx, ImgDetVec *img_det);    //显示图片及目标边界框
    void img_rect_diaplay(int idx, QImage *img);    //显示图片及目标边界框
    int point_position(QPoint point);
    int unique_id_judge(int rect_idx, int current_id, ImgDetVec *img_det);
    void draw_id(int _mouse_position, int _set_id);
    void erase_id(int _mouse_position);
    void paint_rect(int _mouse_position);
    void draw_rect(QPoint _start, QPoint _end);
    void setCursorCross(QPoint p);
    void save_draw_rect(QPoint _start, QPoint _end);
    void add_rect_to_vec(QRect add_rect, ImgDetVec *_img_det);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void set_id_signal(int rect_idx, int id);
    void del_id_signal(int rect_idx);    
    void next_img_signal();
    void pre_img_signal();
    //void del_rect_signal(int rect_idx);
    //void add_rect_signal(QRect add_rect);

public slots:
    void delete_id_slot();
    void change_id_slot();
    void delete_rect_slot();

};

#endif // IMG_LABEL_H
