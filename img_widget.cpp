#include "img_widget.h"
#include <unistd.h>
#include <QDebug>

vector<ImgDetVec> all_det_vec;


ImgWidget::ImgWidget(QWidget *parent, QString *dir) : QWidget(parent), base_dir(*dir)
{
    //设置目录选择按钮和目标显示标签
    main_vlayout = new QVBoxLayout;
    setLayout(main_vlayout);
    setFocusPolicy(Qt::StrongFocus);

    btn_hlayout = new QHBoxLayout;
    btn_hlayout->setAlignment(Qt::AlignCenter);
    next_img_btn = new QPushButton(">>");
    pre_img_btn = new QPushButton("<<");
    next_img_btn->setFixedSize(80, 40);
    pre_img_btn->setFixedSize(80, 40);
    connect(next_img_btn, SIGNAL(clicked(bool)), this, SLOT(next_img_btn_clicked_slot()));
    connect(pre_img_btn, SIGNAL(clicked(bool)), this, SLOT(pre_img_btn_clicked_slot()));
    connect(this, SIGNAL(next_img()), this, SLOT(next_img_btn_clicked_slot()));
    connect(this, SIGNAL(pre_img()), this, SLOT(pre_img_btn_clicked_slot()));
    btn_hlayout->addWidget(pre_img_btn);
    btn_hlayout->addWidget(next_img_btn);

    display_name_lbl = new QLabel;
    qbase_dir = QDir(base_dir);
    qimg_dir = QDir(base_dir+"/img1");
    if(qimg_dir.exists())
        file_lst = qimg_dir.entryList(fltr_lst);
    else
    {
        QMessageBox no_img1;
        no_img1.setText("Can not read images, Dose your directory contains \"img1\" folder?");
        no_img1.exec();
        return;
    }
    qgt_dir = QDir(base_dir+"/gt");
    name_hlayout = new QHBoxLayout;
    name_hlayout->setAlignment(Qt::AlignLeft);

    //id_edit = new QLineEdit("1");
    id_box = new QComboBox;
    id_box->setFixedWidth(150);
    for(int i=0;i<300;i++)
        id_box->addItem(QString("%1").arg(i));
    connect(id_box, SIGNAL(currentTextChanged(const QString)), this, SLOT(id_box_changed_slot(QString)));
    name_hlayout->addWidget(display_name_lbl);

    QLabel *lbl = new QLabel("ID:");
    lbl->setFixedWidth(30);
    name_hlayout->addWidget(lbl);
    name_hlayout->addWidget(id_box);

    save_btn = new QPushButton("Save");
    connect(save_btn, SIGNAL(clicked(bool)), this, SLOT(save_btn_clicked_slot()));
    name_hlayout->addWidget(save_btn);

    manual_rect_btn = new QPushButton("Draw");
    connect(manual_rect_btn, SIGNAL(clicked(bool)), this, SLOT(manual_rect_btn_clicked_slot()));
    click_btn = new QPushButton("Click");
    click_btn->setEnabled(false);
    connect(click_btn, SIGNAL(clicked(bool)), this, SLOT(click_btn_clicked_slot()));
    name_hlayout->addWidget(manual_rect_btn);
    name_hlayout->addWidget(click_btn);

    img_menu_hlayout = new QHBoxLayout;
    display_img_lbl = new ImgLabel;
    connect(display_img_lbl, SIGNAL(set_id_signal(int, int)), this, SLOT(set_id_slot(int, int)));
    connect(display_img_lbl, SIGNAL(del_id_signal(int)), this, SLOT(del_id_slot(int)));
    connect(display_img_lbl, SIGNAL(pre_img_signal()), this, SLOT(pre_img_btn_clicked_slot()));
    connect(display_img_lbl, SIGNAL(next_img_signal()), this, SLOT(next_img_btn_clicked_slot()));
    //connect(display_img_lbl, SIGNAL(add_rect_signal(QRect)), this, SLOT(add_rect_slot(QRect)));
    //connect(display_img_lbl, SIGNAL(del_rect_signal(int)), this, SLOT(del_rect_slot(int)));
    img_menu_hlayout->addWidget(display_img_lbl);

    /*开始读取边界信息，并存储到容器中*/
    qdet_dir = QDir(base_dir+"/det");
    QString det_path;
    if(qdet_dir.exists())   //检测det文件夹是否存在
        det_path= qdet_dir.absoluteFilePath("det.txt");
    else
    {
        QMessageBox no_det;
        no_det.setText("Can not read detection result, Dose your directory contains \"det/det.txt\" file?");
        no_det.exec();
        return;
    }
    det_file = new QFile(det_path);
    det_file->open(QIODevice::ReadOnly);
    all_det_result = new QTextStream(det_file);
    all_det_vec.clear();    //将存储容器清空，否则切换文件夹后读到的边界信息会叠加到后面
    QString each_line;
    QStringList present_list;
    ImgDetVec img_det;

    bool first_line = true;
    int i;      // 用于存放第一张图片的序号，因为第一张图片的序号可以为0或1或其他数字
    each_line = all_det_result->readLine();
    do{
        present_list = each_line.split(",");
        ObjDet obj_det;
        strlst_to_objdet(present_list, obj_det);
        if(first_line==true)    //获取文件中第一行的帧号，该if语句只在开始时执行一次
        {
            i=obj_det.idx;
            first_line=false;
        }
        if(obj_det.idx==i)  //是同一帧中的目标
        {
            img_det.push_back(obj_det);
        }
        else        //一帧中所有目标的信息读取完毕
        {
            all_det_vec.push_back(img_det);
            img_det.clear();
            img_det.push_back(obj_det);
        }
        i=obj_det.idx;
    }while(!(each_line = all_det_result->readLine()).isNull());  
    if(each_line.isNull())      //最后一帧的信息
        all_det_vec.push_back(img_det);
    det_file->close();      //关闭打开的文件
    image_display(img_idx);     //img_idx初始值为0

    main_vlayout->addLayout(name_hlayout);
    main_vlayout->addLayout(img_menu_hlayout);
    main_vlayout->addLayout(btn_hlayout);
}
ImgWidget::~ImgWidget()
{

}

void ImgWidget::strlst_to_objdet(QStringList strlst, ObjDet &det)
{
    det.idx=strlst.at(0).toInt();
    det.id = strlst.at(1).toInt();
    det.ltx = strlst.at(2).toFloat();
    det.lty = strlst.at(3).toFloat();
    det.width = strlst.at(4).toFloat();
    det.height = strlst.at(5).toFloat();
    det.confidence = strlst.at(6).toFloat();
}

/*--------------------------------------------
 * 显示图片名称和图片
 * param: img_idx表示显示图片的编号
---------------------------------------------*/
void ImgWidget::image_display(int &img_idx)
{
    display_name_lbl->setText(file_lst.at(img_idx));
    img = QImage(qimg_dir.absoluteFilePath(file_lst.at(img_idx)));
    //img = &tmp_img;
    display_img_lbl->setMinimumSize(img.size());

    //ImgDetVec det=all_det_vec[img_idx];
    /*如果在下一句中传入&det，则程序在display_img_lbl控件的除img_rect_diaplay以外的函数中会崩溃，因为该函数结束后det后被清除，而&det变为野指针*/
    display_img_lbl->img_rect_diaplay(img_idx, &img);
}

/*slot function*/
void ImgWidget::pre_img_btn_clicked_slot()
{
    if(img_idx > 0)
        img_idx--;
    delete display_img_lbl->img;
    delete display_img_lbl->id_mask;
    delete display_img_lbl->add_rect_mask;      //img, id_mask, add_rect_mask都是在img_rect_diaplay函数中初始化的指针，如果这个函数反复被调用，那么这些指针就要及时删除，否则会占用太多内存
    image_display(img_idx);
}

/*slot function*/
void ImgWidget::next_img_btn_clicked_slot()
{
    if(img_idx < file_lst.length()-1)
        img_idx++;
    delete display_img_lbl->img;
    delete display_img_lbl->id_mask;
    delete display_img_lbl->add_rect_mask;
    image_display(img_idx);
}

/*----------------------------------------------------
 * ID changed slot
 * @param
 * QString id: Current ID that will be set to object
-----------------------------------------------------*/
void ImgWidget::id_box_changed_slot(QString id)
{
    display_img_lbl->set_id=id.toInt();
}

/*----------------------------------------------------------------
 * set id slot
 * @param
 * int rect_idx: The index of rectangle where the mouse is located
-----------------------------------------------------------------*/
void ImgWidget::set_id_slot(int rect_idx, int id)
{
    all_det_vec[img_idx][rect_idx].id=id;
}

/*----------------------------------------------------------------
 * delete id slot, reset the ID to -1
 * @param
 * int rect_idx: The index of rectangle where the mouse is located
----------------------------------------------------------------*/
void ImgWidget::del_id_slot(int rect_idx)
{
    all_det_vec[img_idx][rect_idx].id=-1;
}

/*--------------------------------------------------------------
 * virtual function
 * 键盘事件，当按下D时显示下一张图片，按下A时显示上一张
 * 需要先设置setFocusPolicy(Qt::StrongFocus)或Qt::TabFocus
---------------------------------------------------------------*/
void ImgWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_D)
        emit next_img();
    if(event->key()==Qt::Key_A)
        emit pre_img();
}
/*-------------------------------------------------------------
 * save slot
 * 保存标注结果到gt.txt文件，格式为MOT Chanllenge官方格式
 * 例：1, 2, 164.1, 19.6, 66.5, 163.2, 1, 1, 0.5
 * 1st: frame index
 * 2nd: id index
 * 3rd: left top x
 * 4th: left top y
 * 5th: width
 * 6th: height
 * 7th: be ignored or considered
 * 8th: class
 * 9th: visible ratio
--------------------------------------------------------------*/
void ImgWidget::save_btn_clicked_slot()
{
    qbase_dir.mkpath("gt");     //当前目录下是否有gt文件夹，如果没有则创建
    gt_file = new QFile(qgt_dir.absoluteFilePath("gt.txt"));

    gt_file->open(QIODevice::WriteOnly);        //打开gt/gt.txt文件，如果文件不存在则创建
    for(unsigned int i=0;i<all_det_vec.size();i++)
        for(unsigned int j=0;j<all_det_vec.at(i).size();j++)
        {
            if(all_det_vec[i][j].id!=-1)
            {
                QString tmp=QString("%1, %2, %3, %4, %5, %6, 1, 1, %7\n").arg(all_det_vec[i][j].idx).arg(all_det_vec[i][j].id).arg(all_det_vec[i][j].ltx).arg(all_det_vec[i][j].lty).arg(all_det_vec[i][j].width).arg(all_det_vec[i][j].height).arg(all_det_vec[i][j].confidence);
                QByteArray byte_tmp = tmp.toLatin1();
                gt_file->write(byte_tmp);
                usleep(10);     //防止写入信息不完整
            }
        }
    gt_file->close();       //不要忘记关闭已打开的文件
    QMessageBox *write_successful_box = new QMessageBox;
    write_successful_box->setText("Write to gt.txt successfully!");
    write_successful_box->exec();
}

void ImgWidget::manual_rect_btn_clicked_slot()
{
    display_img_lbl->manual_draw = true;
    manual_rect_btn->setEnabled(false);
    click_btn->setEnabled(true);
    //display_img_lbl->add_rect_widget->setMouseTracking(false);
}

void ImgWidget::click_btn_clicked_slot()
{
    display_img_lbl->manual_draw = false;
    manual_rect_btn->setEnabled(true);
    click_btn->setEnabled(false);
    display_img_lbl->add_rect_widget->setMouseTracking(true);
    display_img_lbl->setCursor(Qt::ArrowCursor);
}
#if 0
/*以下两个槽函数不再需要，因为img_det本身就是一个指针，可以直接操作all_det_vec中的元素,不需要用信号和槽*/
void ImgWidget::add_rect_slot(QRect add_rect)
{
    //qDebug() << "add_rect_slot";
    ObjDet tmp_obj;
    tmp_obj={img_idx, -1, add_rect.x(), add_rect.y(), add_rect.width(), add_rect.height(), 1, true};
    all_det_vec[img_idx].push_back(tmp_obj);
    //display_img_lbl->img_det->push_back(&tmp_obj);
    /*这个语句是错误的，因为img_det是指针，直接指向all_det_vec[img_idx]，执行这个语句相当于重复添加一个个tmp_obj*/
    display_img_lbl->img_det = &all_det_vec[img_idx];
}

void ImgWidget::del_rect_slot(int rect_idx)
{ 
    qDebug() << "del_rect_slot";
    ImgDetVec::iterator k = all_det_vec[img_idx].begin()+rect_idx;
    all_det_vec[img_idx].erase(k);
    //display_img_lbl->img_det->erase(k);
    /*这是一条多余的语句，因为img_det本身就指向了all_det_vec[img_idx]，执行该条语句相当于又删除了一次，从而可能导致程序崩溃*/
}
#endif
