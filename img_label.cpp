#include "img_label.h"
#include <QDebug>

ImgLabel::ImgLabel(QLabel *parent)
    : QLabel(parent)
{
    setAlignment(Qt::AlignLeft|Qt::AlignTop);

    rect_pen = new QPen(Qt::blue, 2, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);
    id_pen = new QPen(Qt::red, 8, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);
    cursor_pen = new QPen(Qt::cyan, 1, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);
    id_font = new QFont("Helvetica [Cronyx]",20);
    fill_brush = new QBrush(QColor(0,150,0, 100));      //设置填充颜色为浅绿色，透明度为100(0~255)
    erase_brush = new QBrush(Qt::transparent, Qt::SolidPattern);    //橡皮擦，用于擦除已标注的ID，透明色

    rect_painter = new QPainter;
    id_painter = new QPainter;
    cursor_painter = new QPainter;

    error_box = new QMessageBox;
    error_box->setText("Can not set this ID repeatedly!");      //重复标ID报错的消息盒子

    id_mask_widget = new QLabel(this);
    id_mask_widget->setStyleSheet(QString("background-color: rgba(255, 255, 255, 0%)"));   //透明遮罩层，用于标ID号

    add_rect_widget = new QLabel(this);
    add_rect_widget->setStyleSheet(QString("background-color: rgba(255, 255, 255, 0%)"));

    cursor_widget = new QLabel(this);
    cursor_widget->setStyleSheet(QString("background-color: rgba(255, 255, 255, 0%)"));

    right_menu = new QMenu;
    add_rect_right_menu = new QMenu;
    del_id = new QAction(tr("&Delete ID"));
    connect(del_id, SIGNAL(triggered(bool)), this, SLOT(delete_id_slot()));
    right_menu->addAction(del_id);
    add_rect_right_menu->addAction(del_id);
    change_id = new QAction(tr("&Change ID"));
    connect(change_id, SIGNAL(triggered(bool)), this, SLOT(change_id_slot()));
    right_menu->addAction(change_id);
    add_rect_right_menu->addAction(change_id);
    del_rect = new QAction(tr("&Delete Rect"));
    connect(del_rect, SIGNAL(triggered(bool)), this, SLOT(delete_rect_slot()));
    add_rect_right_menu->addAction(del_rect);

    dialog = new QDialog(this);
    line_edit = new QLineEdit;
    line_edit->setValidator(new QIntValidator(0,300, this));
    QDialogButtonBox *button_box = new QDialogButtonBox(dialog);
    button_box->addButton("OK", QDialogButtonBox::YesRole);
    button_box->addButton("Cancel", QDialogButtonBox::NoRole);
    connect(button_box, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(button_box, SIGNAL(rejected()), dialog, SLOT(reject()));
    QVBoxLayout *dialog_layout=new QVBoxLayout;
    dialog->setLayout(dialog_layout);
    dialog_layout->addWidget(line_edit);
    dialog_layout->addWidget(button_box);

    setMouseTracking(true);
    id_mask_widget->setMouseTracking(true);    //鼠标事件
    id_mask_widget->setFocusPolicy(Qt::StrongFocus);       //检测键盘事件
    add_rect_widget->setMouseTracking(true);
    add_rect_widget->setFocusPolicy(Qt::TabFocus);
    cursor_widget->setFocusPolicy(Qt::TabFocus);
    cursor_widget->setMouseTracking(true);
}

ImgLabel::~ImgLabel()
{

}

/*---------------------------------------------------------
 * 显示图片并在图片上画出目标边界框，并在图片上加透明遮罩层用于标ID
 * param
 * QImage *img: 图片指针，由img_widget.cpp中调用时传入
 * ImgDetVec *det: 容器指针，表示该张图片中所有目标的边界
----------------------------------------------------------*/
void ImgLabel::img_rect_diaplay(int idx, QImage *img )
{
    index = idx;    //该张图片在all_det_vec中的编号
    original_img = img;
    setGeometry(0,0,img->width(),img->height());
    rect_vec.clear();   //每显示一张新的图片，都要清空一次，否则新图片的边界框会往后叠加
    //original_img=img;
    img_det=&all_det_vec[index];
    id_mask = new QImage(img->width(), img->height(), QImage::Format_ARGB32);
    id_mask->fill(Qt::transparent);

    add_rect_mask = new QImage(img->width(), img->height(), QImage::Format_ARGB32);
    add_rect_mask->fill(Qt::transparent);

    id_mask_widget->setGeometry(0,0,img->width(),img->height());
    add_rect_widget->setGeometry(0, 0, img->width(), img->height());
    add_rect_widget->setPixmap(QPixmap::fromImage(*add_rect_mask));
    cursor_widget->setGeometry(0, 0, img->width(), img->height());

    rect_painter->begin(img);
    rect_painter->setPen(*rect_pen);
    for(unsigned int j=0; j<img_det->size(); j++)       //在原图上画出所有边界框
    {
        int x=img_det->at(j).ltx;
        int y=img_det->at(j).lty;
        int w=img_det->at(j).width;
        int h=img_det->at(j).height;
        QRect rect(x, y, w, h);
        rect_vec.push_back(rect);
        if(img_det->at(j).added == false)   //判断是否为后加的，不是则在原图上画框，是则在add_rect_mask上画框
            rect_painter->drawRect(rect);
        else
        {
            rect_painter->end();

            rect_painter->begin(add_rect_mask);
            rect_painter->setPen(*rect_pen);            
            rect_painter->drawRect(rect);
            rect_painter->end();

            rect_painter->begin(img);
            rect_painter->setPen(*rect_pen);
        }
    }
    rect_painter->end();
    setPixmap(QPixmap::fromImage(*img));
    add_rect_widget->setPixmap(QPixmap::fromImage(*add_rect_mask));

    id_painter->begin(id_mask);
    id_painter->setPen(*id_pen);
    id_painter->setFont(*id_font);
    for(unsigned int j=0; j<img_det->size(); j++)       //在透明遮罩层上标注目标ID号
        if(img_det->at(j).id!=-1)
           id_painter->drawText(rect_vec[j],Qt::AlignCenter, QString("%1").arg(img_det->at(j).id));
    id_painter->end();
    id_mask_widget->setPixmap(QPixmap::fromImage(*id_mask));

    mouse_position = point_position(point);
    paint_rect(mouse_position);
}
#if 1
void ImgLabel::mousePressEvent(QMouseEvent *event)
{
    point = event->pos();
    mouse_position=point_position(point);
    if(event->button()==Qt::LeftButton)
    {
        if(manual_draw == false)
            draw_id(mouse_position, set_id);
        else
        {
            start_draw = true;      //鼠标按下开始画框
            add_rect_widget->setMouseTracking(false);   //开始画框后，mouseTracking设置为false，当有键按下时才会接受鼠标事件，接下来开始拖动鼠标
            cursor_widget->setMouseTracking(false);
            start_point = event->pos();
        }
    }

    if(mouse_position!=-1 && event->button()==Qt::RightButton)
    {
        //qDebug() << "mouse_position"<<mouse_position;
        //qDebug() << "img_det.size"<<img_det->size();
        if(img_det->at(mouse_position).added == false)
        {
            right_menu->exec(QCursor::pos());
        }
        else
        {
            add_rect_right_menu->exec(QCursor::pos());
        }

    }
}

void ImgLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(manual_draw == true)
        {
            end_point=event->pos();
            if(start_point != end_point)
            {
                QRect add_rect = QRect(start_point, end_point);
                //qDebug() << start_point << end_point;
                rect_vec.push_back(add_rect);
                //emit add_rect_signal(add_rect);
                add_rect_to_vec(add_rect, img_det);
                //qDebug() << "added_rect_vec.size="<<rect_vec.size();
                start_draw = false;
                add_rect_widget->setMouseTracking(true);
                cursor_widget->setMouseTracking(true);
                save_draw_rect(start_point, end_point);
            }
        }
    }
}

void ImgLabel::mouseMoveEvent(QMouseEvent *event)
{
    if(manual_draw == false)
    {
        cursor_widget->close();
        point = event->pos();
        mouse_position=point_position(point);
        paint_rect(mouse_position);
    }
    else
    {  
        cursor_widget->show();
        setCursorCross(event->pos());       //鼠标变为十字
        if(start_draw == true)      //该标志位很重要，表示鼠标左键已经按下,此时才会画框
            draw_rect(start_point, event->pos());
    }
}
#endif
/*重写键盘事件虚函数，当按下Del键时，删除鼠标所在框的ID号*/
void ImgLabel::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Delete&&mouse_position!=-1&&img_det->at(mouse_position).id!=-1)
    {
        delete_id_slot();
    }
    else if(event->key()==Qt::Key_D)
    {
        emit next_img_signal();
    }
    else if(event->key()==Qt::Key_A)
        emit pre_img_signal();
#if 1
    else if(event->key()==Qt::Key_Space)
        draw_id(mouse_position, set_id);

#endif
}

/*---------------------------------------------------------------------------
 * 返回鼠标所在位置矩形框的编号，如果鼠标位于两框的重叠位置则返回较小的编号，
 * 矩形框的编号取决于det.txt文件，如果鼠标位于空白部分则返回-1
---------------------------------------------------------------------------*/
int ImgLabel::point_position(QPoint point)
{
    for(int j=0; j<rect_vec.size();j++)
        if(rect_vec.at(j).contains(point, true))
            return j;
    return -1;
}

/*-------------------------------------------------------------------
 * 判断ID是否唯一，如果不唯一返回重复编号
 * @param
 * rect_idx: 当前鼠标所在位置的矩形框编号，由det文件中的顺序而定，程序不可干预
 * current_id: 当前ComboBox中的编号，即需要设置的ID
 * img_det: 当前图片中所有矩形框信息
 * @return
 * -1: 无重复
 * -2: 当前矩形框已经有ID，且与要设置的ID相同
 * -3: 当前矩形框已经有ID，与要设置的ID不同
--------------------------------------------------------------------*/
int ImgLabel::unique_id_judge(int rect_idx, int current_id, ImgDetVec *img_det)
{
    for(int i=0;i<img_det->size();i++)
    {
        if(i!=rect_idx)     //遍历到的矩形框编号与鼠标所在不同
        {
            if(img_det->at(i).id!=current_id)
                continue;
            else
            {
                error_box->exec();
                return i;
            }
        }
        else
        {
            if(img_det->at(i).id==current_id)
                return -2;
            else if(img_det->at(i).id!=-1&&img_det->at(i).id!=current_id)
                return -3;
            else
                continue;
        }
    }
    return -1;
}
/*------------------------------------------------------------------
 * 标注目标的ID号
 * @param
 * int _mouse_position: 表示鼠标所在位置矩形框的编号
 * int _set_id: 需要设置的ID号
-------------------------------------------------------------------*/
void ImgLabel::draw_id(int _mouse_position, int _set_id)
{
    id_painter->begin(id_mask);
    id_painter->setPen(*id_pen);
    id_painter->setFont(*id_font);

    for(int j=0; j<rect_vec.length();j++)
    {
        if(_mouse_position==j)
        {
            int uni = unique_id_judge(j, _set_id, img_det);
            if(uni == -1)   //无重复
            {
                emit set_id_signal(j, _set_id);
                id_painter->drawText(rect_vec.at(j), Qt::AlignCenter, QString("%1").arg(img_det->at(j).id));
                id_mask_widget->setPixmap(QPixmap::fromImage(*id_mask));
            }
            else if(uni == -2)  //当前框已设置为该ID
                ;
            else if(uni == -3)  //当前框已设置为其他ID，需要重新设置
            {
                emit set_id_signal(j, _set_id);
                //qDebug()<<"eraseRect";
                id_painter->setCompositionMode(QPainter::CompositionMode_Clear);
                int x=rect_vec.at(j).x()+rect_vec.at(j).width()/2-20;
                int y=rect_vec.at(j).y()+rect_vec.at(j).height()/2-20;
                id_painter->setBrush(*erase_brush);
                id_painter->drawPie(x, y, 40, 40, 0, 5760);
                id_painter->setCompositionMode(QPainter::CompositionMode_DestinationOver);
                id_painter->drawText(rect_vec.at(j), Qt::AlignCenter, QString("%1").arg(img_det->at(j).id));
                id_mask_widget->setPixmap(QPixmap::fromImage(*id_mask));
            }
            break;  //标注完成，退出循环
        }
    }
    id_painter->end();
}
/*--------------------------------------------------
 * 擦除鼠标所在矩形框内的ID号
---------------------------------------------------*/
void ImgLabel::erase_id(int _mouse_position)
{
    id_painter->begin(id_mask);
    id_painter->setBrush(*erase_brush);
    id_painter->setCompositionMode(QPainter::CompositionMode_Clear);
    int x=rect_vec.at(_mouse_position).x()+rect_vec.at(_mouse_position).width()/2-20;
    int y=rect_vec.at(_mouse_position).y()+rect_vec.at(_mouse_position).height()/2-20;
    id_painter->drawPie(x, y, 40, 40, 0, 5760);
    id_painter->end();
}

/*----------------------------------------------------
 * slot function
 * 删除ID号
-----------------------------------------------------*/
void ImgLabel::delete_id_slot()
{
    emit del_id_signal(mouse_position);
    erase_id(mouse_position);
    id_mask_widget->setPixmap(QPixmap::fromImage(*id_mask));
}

/*----------------------------------------------------
 * slot function
 * 更换ID号
-----------------------------------------------------*/
void ImgLabel::change_id_slot()
{
    if(img_det->at(mouse_position).id!=-1)
        delete_id_slot();
    line_edit->setEnabled(true);
    line_edit->clear();
    if(dialog->exec()==QDialog::Accepted)
    {
        int id=line_edit->text().toInt();
        draw_id(mouse_position, id);
    }
}

/*----------------------------------------------------
 * 给鼠标所在的矩形框内涂上颜色，表示当前需要设置ID的框
 * @param
 * int _mouse_position: 表示矩形框的编号
-----------------------------------------------------*/
void ImgLabel::paint_rect(int _mouse_position)
{
    filled_mask=*id_mask;
    rect_painter->begin(&filled_mask);
    if(_mouse_position!=-1)
    {
        //qDebug() << "_mouse_position"<<_mouse_position;
        rect_painter->fillRect(rect_vec.at(_mouse_position), *fill_brush);
    }
    id_mask_widget->setPixmap(QPixmap::fromImage(filled_mask));
    id_mask_widget->show();
    rect_painter->end();
    filled_mask=*id_mask;
}

/*----------------------------------------------------
 * 当出现漏检目标时需要手动画边界框，调用该函数
 * @param
 * QPoint _start, QPoint _end: 分别表示矩形的两个顶点
-----------------------------------------------------*/
void ImgLabel::draw_rect(QPoint _start, QPoint _end)
{
    QImage tmp_mask = *add_rect_mask;
    rect_painter->begin(&tmp_mask);
    rect_painter->setPen(*rect_pen);
    rect_painter->drawRect(QRect(_start, _end));
    add_rect_widget->setPixmap(QPixmap::fromImage(tmp_mask));
    rect_painter->end();
}

/*--------------------------------------------------------
 * 删除用户自己画的框，txt文件中读到的框不能删除
 * 删除框之前，若已经标注ID，则要先删除ID，因为两者不在一个图层上
--------------------------------------------------------*/
void ImgLabel::delete_rect_slot()
{
    erase_id(mouse_position);
    id_painter->begin(add_rect_mask);
    id_painter->setBrush(*erase_brush);
    id_painter->setCompositionMode(QPainter::CompositionMode_Clear);
    QRect original_rect = rect_vec[mouse_position];
    //id_painter->eraseRect(original_rect);
    QRect bigger_rect = QRect(original_rect.x()-2, original_rect.y()-2, original_rect.width()+4, original_rect.height()+4);
    id_painter->drawRect(bigger_rect);
    id_painter->end();
    add_rect_widget->setPixmap(QPixmap::fromImage(*add_rect_mask));
    //emit del_rect_signal(mouse_position);
    //ImgDetVec::iterator k = img_det->begin()+mouse_position;
    img_det->erase(img_det->begin()+mouse_position);
    rect_vec.erase(rect_vec.begin()+mouse_position);
    //qDebug() << "delete_rect_vec.size="<<rect_vec.size();
    mouse_position = -1;
}

/*--------------------------------------------------
 * 设置鼠标为十字形，便于画框
 * @param
 * QPoint p: 鼠标所在的位置
--------------------------------------------------*/
void ImgLabel::setCursorCross(QPoint p)
{
    setCursor(Qt::CrossCursor);
    QImage cursor_mask=*add_rect_mask;
    cursor_painter->begin(&cursor_mask);
    cursor_painter->setPen(*cursor_pen);
    cursor_painter->drawLine(QPoint(0, p.y()), QPoint(original_img->width(), p.y()));
    cursor_painter->drawLine(QPoint(p.x(), 0), QPoint(p.x(), original_img->height()));
    cursor_painter->end();
    cursor_widget->setPixmap(QPixmap::fromImage(cursor_mask));
}

/*--------------------------------------------------
 * 保存已经画好的边界框,鼠标左键释放时执行该函数
 * @param
 * QPoint _start, QPoint _end: 边界框的顶点
--------------------------------------------------*/
void ImgLabel::save_draw_rect(QPoint _start, QPoint _end)
{
    rect_painter->begin(add_rect_mask);
    rect_painter->setPen(*rect_pen);
    rect_painter->drawRect(QRect(_start, _end));
    rect_painter->end();
}

void ImgLabel::add_rect_to_vec(QRect add_rect, ImgDetVec *_img_det)
{
    ObjDet tmp_obj;
    tmp_obj={index, -1, add_rect.x(), add_rect.y(), add_rect.width(), add_rect.height(), 1, true};
    _img_det->push_back(tmp_obj);
}
