/**
  ******************************************************************************
  * @file           : component_structs.cpp
  * @author         : BUPT_ShenXu
  * @date           : 2023/4/27
  * @detail         :
  ******************************************************************************
*/

#include "sx_lib.h"

pthread_mutex_t eventMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t renderMutex = PTHREAD_MUTEX_INITIALIZER;

ComponentsVisual *get_components_list();

ComponentVisual *getComponentById(const string &id) {
    ComponentsVisual *list = get_components_list()->after;
    while (list->that != nullptr) {
        if (id == list->that->id) {
            return list->that;
        }
        list = list->after;
    }
    return nullptr;
}

void *listenKey(void *) {
    while (true) {
        int input_char = getch();
        pthread_mutex_lock(&eventMutex);
        ComponentsVisual *list = get_components_list()->after;
        while (list->that != nullptr) {
            if (list->that->keyDown != nullptr) {
                (*list->that->keyDown)((char) input_char, list->that);
            }
            list = list->after;
        }
        pthread_mutex_unlock(&eventMutex);
    }
}

void *listenClick(void *) {
    bool leftDown = false, rightDown = false, singleClick = true;
    while (true) {
        ExMessage m = getmessage(EX_MOUSE);
        if (m.message == WM_MOUSEMOVE) {
            continue;
        }
        ComponentsVisual *components_list = get_components_list();
        pthread_mutex_lock(&eventMutex);
        ComponentsVisual *list = components_list->after;
        while (list->that != nullptr) {
            list->that->is_click_me(m.x, m.y);
            list = list->after;
        }
        list = list->after;
        while (list->that != nullptr) {
            if (list->that->is_click_me(m.x, m.y)) {
                //左键按下时
                if (m.message == WM_LBUTTONDOWN) {
                    leftDown = singleClick = true;
                    if (list->that->leftClickDown != nullptr)(*list->that->leftClickDown)(m.x, m.y, list->that);
                }//左键释放时
                else if (m.message == WM_LBUTTONUP) {
                    leftDown = false;
                    if (list->that->leftClickUp != nullptr) (*list->that->leftClickUp)(m.x, m.y, list->that);
                    if (rightDown == true) {
                        singleClick = false;
                        break;
                    }
                    if (singleClick && list->that->leftClick != nullptr) {
                        (*list->that->leftClick)(m.x, m.y, list->that);
                        singleClick = true;
                    }
                }//右键释放时
                else if (m.message == WM_RBUTTONUP) {
                    rightDown = false;
                    if (list->that->rightClickUp != nullptr)(*list->that->rightClickUp)(m.x, m.y, list->that);
                    if (leftDown == true) {
                        singleClick = false;
                        break;
                    }
                    if (singleClick && list->that->rightClick != nullptr) {
                        (*list->that->rightClick)(m.x, m.y, list->that);
                        singleClick = true;
                    }
                }//中键释放时
                else if (m.message == WM_MBUTTONUP && list->that->midClickUp != nullptr) {
                    (*list->that->midClickUp)(m.x, m.y, list->that);
                }//鼠标滚轮滚动时
                else if (m.message == WM_MOUSEWHEEL && list->that->mouseWheel != nullptr) {
                    (*list->that->mouseWheel)(m.x, m.y, m.wheel, list->that);
                }//左键双击时
                else if (m.message == WM_LBUTTONDBLCLK && list->that->leftDoubleClick != nullptr) {
                    (*list->that->leftDoubleClick)(m.x, m.y, list->that);
                }//右键双击时
                else if (m.message == WM_RBUTTONDBLCLK && list->that->rightDoubleClick != nullptr) {
                    (*list->that->rightDoubleClick)(m.x, m.y, list->that);
                }//中键双击时
                else if (m.message == WM_MBUTTONDBLCLK && list->that->midDoubleClick != nullptr) {
                    (*list->that->midDoubleClick)(m.x, m.y, list->that);
                }//右键按下时
                else if (m.message == WM_RBUTTONDOWN) {
                    rightDown = singleClick = true;
                    if (list->that->rightClickDown != nullptr)(*list->that->rightClickDown)(m.x, m.y, list->that);
                }//中键按下时
                else if (m.message == WM_MBUTTONDOWN && list->that->midClickDown != nullptr) {
                    (*list->that->midClickDown)(m.x, m.y, list->that);
                }
                if (rightDown && leftDown) {
                    if (list->that->rightAndLeftDown != nullptr)(*list->that->rightAndLeftDown)(m.x, m.y, list->that);
                    singleClick = false;
                }
                break;
            }
            list = list->after;
        }
        pthread_mutex_unlock(&eventMutex);
    }
}

ComponentsVisual *get_components_list() {
    static ComponentsVisual *components_list;
    if (components_list == nullptr) {
        components_list = (ComponentsVisual *) malloc(sizeof(ComponentsVisual));
        components_list->that = nullptr;
        components_list->after = components_list;
        components_list->before = components_list;
        //事件处理线程
        pthread_t click_thread;
        pthread_create(&click_thread, nullptr, listenClick, nullptr);
        pthread_t key_thread;
        pthread_create(&key_thread, nullptr, listenKey, nullptr);
        pthread_t renderThread;
        pthread_create(&renderThread, nullptr, renderAll, nullptr);
    }
    return components_list;
}

void addComponent(ComponentVisual *component) {
    ComponentsVisual *components_list = get_components_list();
    auto *new_component = (ComponentsVisual *) malloc(sizeof(ComponentsVisual));
    new_component->that = component;
    new_component->before = components_list;
    new_component->after = components_list->after;
    components_list->after->before = new_component;
    components_list->after = new_component;
}

void del_component(ComponentVisual *component) {
    pthread_mutex_lock(&renderMutex);
    ComponentsVisual *components_list = get_components_list();
    ComponentsVisual *list = components_list->after;
    while (list->that == component) {
        list = list->after;
        if (list->that == nullptr)return;
    }
    list->before->after = list->after;
    list->after->before = list->before;
    delete list->that;
    free(list);
    pthread_mutex_unlock(&renderMutex);
}

void clearAll() {
    pthread_mutex_lock(&renderMutex);
    ComponentsVisual *components_list = get_components_list();
    ComponentsVisual *list = components_list->after;
    while (true) {
        list = list->after;
        delete list->before->that;
        free(list->before);
        if (list->that == nullptr)break;
    }
    components_list->after = components_list;
    components_list->before = components_list;
    pthread_mutex_unlock(&renderMutex);
}

long long getCurrentTime() {
    struct timeval tv{};
    gettimeofday(&tv, nullptr);
    return time(nullptr) * 1000 + tv.tv_usec / 1000;
}

void *renderAll(void *) {
    ComponentsVisual *components_list = get_components_list();
    int framePerSecond = 6;
    long long lastTime = 0L;
    //loop
    while (true) {
        //render
        pthread_mutex_lock(&eventMutex);
        pthread_mutex_lock(&renderMutex);
        ComponentsVisual *list = components_list->before;
        BeginBatchDraw();
        cleardevice();
        while (list->that != nullptr) {
            if (list->that->show) {
                list->that->render_this();
                list->that->is_render = true;
            }
            list = list->before;
        }
        fpsUserRender();
        EndBatchDraw();
        pthread_mutex_unlock(&eventMutex);
        pthread_mutex_unlock(&renderMutex);
        //fps controller
        while (getCurrentTime() - lastTime < 1000 / framePerSecond) Sleep(1);
        lastTime = getCurrentTime();
    }
}