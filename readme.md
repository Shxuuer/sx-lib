```yaml
title: readme.md
author: 巴普特_ShenXu
date: 2023/4/26
detail: 基于easyx图形库的组件库
```

# 导包
```c
#include "component_structs.h"
```

# 新建窗口
```c
new SX_window(int width,int height);
```
# 可视组件
## 组件的属性
    style：用于保存组件的样式。
    is_render：理论上不允许使用者修改。
    component_type：用于保存组件的类型，一般情况下，如非自定义组件，不需修改此值。
    content：组件的文本内容。
    show：组件是否显示。
    id：组件的标识码，请不要重复，否则选出的仅为第一个匹配id的组件。
    
    (for SX_image)
    src：显示图片的路径
    (for SX_inputBox)
    maxCharNum；输入框最大输入字符数，默认为-1表示不限制。
## 创建组件
    我们一共提供了四个组件可供使用，分别为文本框(SX_text)、图片(SX_image)、输入框(SX_inputBox)、按钮(SX_button)。
    对于每一个组件均可使用new创建并加载到渲染流中，创建的方法如下方代码所示；
    我们建议每一个组件都编写id属性，便于在事件中获取其他组件。
```c
auto text = new SX_text();
text.id = "myText";
auto image = new SX_image();
auto button = new SX_button();
auto inputBox = new SX_inputBox();
```
    当然，你也可以使用下面的有参数的构造方法使得组件的创建更加简洁，并且提供了相应的初始样式。
    但注意，我们并不推荐你使用此方法来构造组件，不能够做到每一个含有参数的构造方法能够全面地提供你想要的初始样式。
    我们推荐你使用无参数构造方法后，对组件的每一个样式进行手动修改，虽然这样很麻烦，但是却能有效满足需求。
```c
auto text = new SX_text(string content, int left, int top);
auto image = new SX_image(string src, int top, int left, int width, int height);
auto button = new SX_button(int left, int top, int width, int height);
auto inputBox = new SX_inputBox(int left, int top, int width, int height);
```

## 删除组件
    当你想要从渲染流当中删除某个组件，可以调用del_component(ComponentVisual *component)函数从组件流中删除相应的组件。
    或者你可以使用clearAll函数将组件流中的所有组件删除。
    但如果你仅仅是想暂时隐藏某个组件，可以试试将组件的show属性改为false。

## 渲染组件
    当你创建组件或者修改组件的样式后，组件并不能及时地展示到窗口上，而是存储在一个渲染流当中。
    你需要手动调用render()方法来将你的全部修改展示在屏幕当中。
    render方法已经写好了缓冲，因此你不必为避免屏幕闪烁手动写双缓冲。

## 修改样式
    对于每一个子组件，都有Style结构体属性来修改组件的样式，你只需要按照下面的方式来修改对应组件的样式。
    但注意，每次修改样式后都需要调用render方法来使样式的修改生效。
```c
//生成一个坐标为（100，100），大小为200、10，字体大小为9像素，背景颜色为9的输入框
auto input = new SX_inputBox(100,100,200,10);
input->style.font_size = 9;
input->style.background_color = GREEN;
//生成一个坐标为（100，200），内容为test，而后修改为123的文本框
auto text = new SX_text("test",100,200);
text->content = "123";
//将添加的组件渲染到屏幕上
render();
```
### style的所有属性
#### 颜色
    可以使用WHITE、BLACK等类似的颜色常量，但并不推荐。我们推荐您使用RGB(int r,int g,int b)来表示颜色
#### top
    决定了组件距离上边界的距离（单位：像素）。
#### left
    决定了组件距离左边界的距离（单位：像素）。
#### width
    决定了组件的宽度（单位：像素）。
#### height
    决定了组件的高度（单位：像素）。
#### background_color
    决定了组件的背景颜色。
#### border_width
    决定了组件的边框宽度（单位：像素）。
#### border_style
    决定了组件的边框样式。
#### border_color
    决定了组件的边框颜色。
#### font_size
    决定了组件的字体大小（单位：像素）。
#### font_color
    决定了组件的字体颜色。
#### font_style
    决定了字体。由于easyx字符集的原因，我们并不推荐使用汉字表示字体，而是使用对应字体的英文，例如：Yahei、Verdana。

## 事件处理
    对于每一个组件，我们都提供了部分事件处理。
    你只需要将相应的函数指针存放到相应组件的指针函数变量当中即可。
    
    对于每一个需要处理的点击事件，其参数列表必须为
    (int pointX, int pointY, ComponentVisual *component)。
    pointX、pointY为点击的位置坐标（相对于窗口左边与上边），component为点击的组件指针。
    
    对于每一个需要处理的焦点类事件，其参数列表必须为
    (ComponentVisual*component)。
    component为失去焦点的组件指针

    对于每一个需要处理的键盘输入事件，其参数列表必须为
    (char key,ComponentVisual*component)。
    key为键盘输入的值，component为需要处理事件的组件的指针。
    
    注意：如非必要，请不要在方法中写render函数渲染，在对组件流的事件处理中，会在所有组件的事件处理完毕后统一渲染。
    注意：SX_inputBox组件已经默认重写左键单击、键盘输入、失去焦点事件，当你试图重写其函数时，其原有的输入功能可能会丧失。

### 获取组件
    对于每一个事件的处理中，参数列表中都包含有触发事件的组件的指针。
    当然这是不够的，你可以通过getComponentById(ComponentVisual * component)函数来获取其他组件的指针
```c
auto button = new SX_button(0,0,100,100);
button->id = "test";
getComponentById("test")->style.font_size = 20;
render();
```
    示例程序实现一个左键方块，方块下移的程序。
```c
#include "component_structs.h"
void test(int x, int y, ComponentVisual *component){
    //组件的y坐标加10像素
    component->style.top+=10;
}
int main(){
    //创建窗口
    new SX_window(1000,600);
    //创建一个按钮，并且绑定点击事件
    auto button = new SX_button(0,0,100,100);
    button->leftClickDown = test;
    render();
    sleep(-1);
}
```
    示例程序实现点击按钮后，文本框的内容改变。
```c
#include "component_structs.h"
void test(int x, int y, ComponentVisual *component) {
    auto myText = getComponentById("myText");
    if(myText!= nullptr){
        myText->content = "newContext";
    }
}
int main() {
    new SX_window(1000, 600);
    auto text = new SX_text("123",0,0);
    text->id = "myText";
    auto button = new SX_button(0, 30, 50, 50);
    button->leftClickDown = test;
    render();
    sleep(-1);
}
```
# 不可视组件
## SX_clock
### 创建组件
```c
auto demo = new SX_click();
```
### 销毁组件
```c
demo.free_timer();
```
### 组件属性
    int sleep_time;多久执行一次，默认-1，单位：秒
    bool started;是否开始计时，默认false
### 绑定事件
    对于每一个需要绑定的超时事件，其参数列表必须为
    void timing_finish(SX_timer *)
### 示例程序
    示例程序展示了每一秒钟屏幕上的text加一
```c
#include "component_structs.h"
void add(SX_timer * timer){
    auto text = getComponentById("text");
    text->content = to_string(stoi(text->content)+1);
    render();
}
int main(){
    new SX_window(600,400);
    auto text = new SX_text("0",0,0);
    text->id = "text";
    render();
    auto timer = new SX_timer(1);
    timer->timing_finish = add;
    timer->started = true;
    sleep(-1);
}
```
