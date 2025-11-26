#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "cocos2d.h"

/**
 * @brief 应用程序委托类，必须继承自cocos2d::Application
 */
class AppDelegate : private cocos2d::Application  // 正确继承
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    // 重写Application的虚函数
    virtual void initGLContextAttrs() override;
    virtual bool applicationDidFinishLaunching() override;
    virtual void applicationDidEnterBackground() override;
    virtual void applicationWillEnterForeground() override;
};

#endif // _APP_DELEGATE_H_