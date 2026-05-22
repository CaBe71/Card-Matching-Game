
#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

 /**
 @brief    The cocos2d Application.

 Private inheritance here hides part of interface from Director.
 */
class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    virtual void initGLContextAttrs();

    /**
    @brief    在此处实现导演和场景的初始化代码。
    @return true    初始化成功，应用程序继续运行。
    @return false   初始化失败，应用程序已终止。
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  当应用程序进入后台时触发此事件
    @param  该应用程序的指针
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  当应用程序重新进入前台时触发此事件
    @param  该应用程序的指针
    */
    virtual void applicationWillEnterForeground();
};

#endif // _APP_DELEGATE_H_