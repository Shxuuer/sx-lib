/**
  ******************************************************************************
  * @file           : component_structs.h
  * @author         : BUPT_ShenXu
  * @date           : 2023/4/27
  * @detail         : 自制组件库
  ******************************************************************************
*/

#ifndef VISUAL_VERSION_SX_LIB_H
#define VISUAL_VERSION_SX_LIB_H

#include <iostream>
#include <unistd.h>
#include "string"
#include "easyx.h"
#include <conio.h>
#include <cstdlib>
#include "sys/time.h"

using namespace std;

///窗口结构体
struct SX_window;
///样式结构体
struct Style;
///所有组件的父类
struct ComponentVisual;

/// 非手动调用，将组件添加到组件流当中
/// \param component
void addComponent(ComponentVisual *component);
void fpsUserRender();
long long getCurrentTime();

/// 根据id获取组件指针
/// \param id 所需要的组件的id
/// \return 查找组件的指针
ComponentVisual *getComponentById(const string &id);

/// 从组件流中删除指定组件
/// \param component 指定组件的指针
void del_component(ComponentVisual *component);

///清空组件流
void clearAll();

///子组件
struct SX_button;
struct SX_text;
struct SX_inputBox;
struct SX_image;

///组件流单位
typedef struct ComponentsVisual {
    ComponentsVisual *after;
    ComponentsVisual *before;
    ComponentVisual *that;
} Components;

void *renderAll(void *);

///创建窗口
typedef struct SX_window {
    SX_window(int width, int height, COLORREF background_color = WHITE) {
        initgraph(width, height);
        setbkcolor(background_color);
        cleardevice();
        setbkmode(TRANSPARENT);
    }
} SX_window;
typedef struct Component {

} Component;
typedef struct ComponentNonVisual : Component {

} ComponentNonVisual;

///可视组件样式
struct Style {
    //组件位置
    int top, left;
    //组件大小
    int width, height;
    //背景颜色
    COLORREF background_color;
    //边框样式
    int border_width, border_style;
    COLORREF border_color;
    //字体（样式&内容）
    int font_size;
    LPCTSTR font_style;
    COLORREF font_color;
};

///可视组件
struct ComponentVisual : Component {
    //是否已经被渲染
    bool is_render;
    //是否被点击
    bool clicked;

    //构造方法
    ComponentVisual() {
        show = true;
        leftClickDown = rightClickDown = midClickDown = leftClickUp = rightClickUp = midClickUp = leftDoubleClick =
        rightDoubleClick = midDoubleClick = leftClick = rightClick = rightAndLeftDown = nullptr;
        mouseWheel = nullptr;
        keyDown = nullptr;
        blur = nullptr;

        is_render = false;
        content = id = "";
        clicked = false;
    }

    //是否显示
    bool show;
    //组件样式
    struct Style style;
    //内容
    string content;
    //组件ID
    string id;

    ///渲染事件
    virtual void render_this() {}

    void (*mouseWheel)(int pointX, int pointY, int wheelSize, ComponentVisual *component);

    /// 左键按下触发事件
    /// \param pointX 点击x坐标
    /// \param pointY 点击y坐标
    /// \param component 触发事件的组件指针
    void (*leftClickDown)(int pointX, int pointY, ComponentVisual *component);

    void (*leftClickUp)(int pointX, int pointY, ComponentVisual *component);

    void (*leftDoubleClick)(int pointX, int pointY, ComponentVisual *component);

    void (*leftClick)(int pointX, int pointY, ComponentVisual *component);

    /// 右键触发事件
    /// \param pointX 点击x坐标
    /// \param pointY 点击y坐标
    /// \param component 触发事件的组件指针
    void (*rightClickDown)(int pointX, int pointY, ComponentVisual *component);

    void (*rightClickUp)(int pointX, int pointY, ComponentVisual *component);

    void (*rightDoubleClick)(int pointX, int pointY, ComponentVisual *component);

    void (*rightClick)(int pointX, int pointY, ComponentVisual *component);

    void (*rightAndLeftDown)(int pointX, int pointY, ComponentVisual *component);

    /// 中键触发事件
    /// \param pointX 点击x坐标
    /// \param pointY 点击y坐标
    /// \param component 触发事件的组件指针
    void (*midClickDown)(int pointX, int pointY, ComponentVisual *component);

    void (*midClickUp)(int pointX, int pointY, ComponentVisual *component);

    void (*midDoubleClick)(int pointX, int pointY, ComponentVisual *component);

    /// 组件失去焦点时
    /// \param component 实现该事件的组件
    void (*blur)(ComponentVisual *component);

    /// 键盘按下触发事件
    /// \param key 键盘按下的字符
    /// \param component 实现该事件的组件
    void (*keyDown)(char key, ComponentVisual *component);

    /// 判断点击是否为当前组件
    /// \param pointX 点击的x坐标
    /// \param pointY 点击的y坐标
    /// \return 是否点击的当前组件
    virtual bool is_click_me(int pointX, int pointY) {
        bool is_me = pointX > this->style.left
                     && pointX < this->style.left + this->style.width
                     && pointY > this->style.top
                     && pointY < this->style.top + this->style.height
                     && this->show
                     && this->is_render;
        if (!is_me && this->clicked) {
            if (this->blur != nullptr) {
                this->blur(this);
            }
        }
        this->clicked = is_me;
        return is_me;
    }
};


typedef struct SX_button : ComponentVisual {
    SX_button() : ComponentVisual() {
        addComponent(this);
    }

    explicit SX_button(int left, int top, int width, int height) : SX_button() {
        this->style.left = left;
        this->style.top = top;
        this->style.width = width;
        this->style.height = height;
        this->style.font_size = 10;
        this->style.font_style = _T("Yahei");
        this->style.border_width = 0;
        this->style.background_color = WHITE;
        this->style.font_color = BLACK;
        this->style.border_style = BS_SOLID;
        this->style.border_color = BLACK;
    }

    void render_this() override {
        setlinestyle(this->style.border_style, this->style.border_width);
        setlinecolor(this->style.border_color);
        setlinestyle(this->style.border_style);
        settextstyle(this->style.font_size, this->style.font_size / 1.8, this->style.font_style);
        settextcolor(this->style.font_color);
        setfillcolor(this->style.background_color);
        setfillstyle(BS_SOLID);
        auto *content = new TCHAR[this->content.size() + 1];
        content[this->content.size()] = 0;
        copy(this->content.begin(), this->content.end(), content);
        fillrectangle(this->style.left, this->style.top, this->style.left + this->style.width,
                      this->style.top + this->style.height);
        outtextxy(this->style.left + 2, this->style.top + (this->style.height - this->style.font_size) / 2, content);
    }
} SX_button;


typedef struct SX_text : ComponentVisual {
    SX_text() : ComponentVisual() {
        addComponent(this);
    }

    explicit SX_text(string content, int left, int top) : SX_text() {
        this->content = std::move(content);
        this->style.left = left;
        this->style.top = top;
        this->style.font_size = 10;
        this->style.font_style = _T("Yahei");
        this->style.width = this->style.height = this->style.border_width = 0;
        this->style.background_color = WHITE;
        this->style.font_color = BLACK;
        this->style.border_style = BS_SOLID;
        this->style.border_color = BLACK;
    }

    void render_this() override {
        settextstyle(this->style.font_size, this->style.font_size / (1.8), this->style.font_style);
        settextcolor(this->style.font_color);
        auto *content = new TCHAR[this->content.size() + 1];
        content[this->content.size()] = 0;
        copy(this->content.begin(), this->content.end(), content);
        outtextxy(this->style.left, this->style.top, content);
    }

    bool is_click_me(int pointX, int pointY) override {
        return false;
    }
} SX_text;


typedef struct SX_inputBox : ComponentVisual {
    int maxCharNum;
private:
    COLORREF last_border_color;

    static void inputBoxClicked(int pointX, int pointY, ComponentVisual *component) {
        auto this_input_box = (SX_inputBox *) component;
        if (this_input_box->style.border_color != BLACK) {
            this_input_box->last_border_color = this_input_box->style.border_color;
        }
        this_input_box->style.border_color = BLACK;
    }

    static void inputBoxInput(char key, ComponentVisual *component) {
        auto this_input_box = (SX_inputBox *) component;
        if (this_input_box->clicked) {
            if (key == '\b' && component->content.length() != 0)
                this_input_box->content.erase(this_input_box->content.end() - 1);
            if (this_input_box->maxCharNum != -1 &&
                this_input_box->content.length() == this_input_box->maxCharNum)
                return;
            if (key != '\b') this_input_box->content += (char) key;
        }
    }

    static void inputBoxBlur(ComponentVisual *component) {
        auto this_input_box = (SX_inputBox *) component;
        this_input_box->style.border_color = this_input_box->last_border_color;
    }

public:
    SX_inputBox() : ComponentVisual() {
        this->maxCharNum = -1;
        addComponent(this);
        this->leftClick = inputBoxClicked;
        this->keyDown = inputBoxInput;
        this->blur = inputBoxBlur;
    }

    SX_inputBox(int left, int top, int width, int height) : SX_inputBox() {
        this->style.left = left;
        this->style.top = top;
        this->style.width = width;
        this->style.height = height;
        this->style.font_size = height - 1;
        this->style.border_width = 1;
        this->style.border_color = RGB(200, 200, 200);
        this->style.background_color = WHITE;
    }

    void render_this() override {
        setlinestyle(this->style.border_style, this->style.border_width);
        setlinecolor(this->style.border_color);
        setlinestyle(this->style.border_style);
        settextstyle(this->style.font_size, this->style.font_size / (1.8), this->style.font_style);
        settextcolor(this->style.font_color);
        setfillcolor(this->style.background_color);
        setfillstyle(BS_SOLID);
        fillrectangle(this->style.left, this->style.top, this->style.left + this->style.width,
                      this->style.top + this->style.height);
        auto *context = new TCHAR[this->content.size() + 1];
        context[this->content.size()] = 0;
        copy(this->content.begin(), this->content.end(), context);
        outtextxy(this->style.left + this->style.border_width,
                  this->style.top + (this->style.height - this->style.font_size) / 2, context);
    }
} SX_inputBox;


typedef struct SX_image : ComponentVisual {
    //图片路径
    string src;

    SX_image() : ComponentVisual() {
        addComponent(this);
    }

    explicit SX_image(string src, int top, int left, int width, int height) : SX_image() {
        this->src = std::move(src);
        this->style.top = top;
        this->style.left = left;
        this->style.width = width;
        this->style.height = height;
    }

    void render_this() override {
        auto *src_t = new TCHAR[this->src.size() + 1];
        src_t[this->src.size()] = 0;
        copy(this->src.begin(), this->src.end(), src_t);
        IMAGE img;
        loadimage(&img, src_t, this->style.width - this->style.border_width * 2,
                  this->style.height - this->style.border_width * 2);
        setfillcolor(this->style.border_color);
        setfillstyle(BS_SOLID);
        solidrectangle(this->style.left, this->style.top, this->style.left + this->style.width,
                       this->style.top + this->style.height);
        putimage(this->style.left + this->style.border_width, this->style.top + this->style.border_width, &img);
    }
} SX_image;

///计时器组件
typedef struct SX_timer : ComponentNonVisual {
private:
    static void *timing_finish_inner(void *timer) {
        auto clock = (SX_timer *) timer;
        while (true) {
            Sleep(clock->sleep_time);
            if (!clock->started || clock->sleep_time == -1)break;
            if (clock->ifFree)return nullptr;
            if (clock->timing_finish != nullptr)clock->timing_finish(clock);
        }
    }

    //是否要被销毁
    bool ifFree;
    //计时线程
    pthread_t pthread;
public:
    //每隔多长时间执行一次（单位：秒）
    int sleep_time;
    //是否倒计时
    bool started;

    //计时结束后执行的函数
    void (*timing_finish)(SX_timer *);

    SX_timer() {
        ifFree = false;
        timing_finish = nullptr;
        started = false;
        sleep_time = -1;
        pthread_create(&pthread, nullptr, timing_finish_inner, (void *) this);
    }

    explicit SX_timer(int sleep_time) : SX_timer() {
        this->sleep_time = sleep_time;
    }

    void free_timer() {
        this->ifFree = true;
        delete this;
    }
} SX_timer;
#endif //VISUAL_VERSION_SX_LIB_H
