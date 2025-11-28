#include "AppDelegate.h"
#include "Classes/controllers/GameController.h"
#include "Classes/views/GameView.h"

USING_NS_CC;

// 构造函数
AppDelegate::AppDelegate()
{
}

// 析构函数
AppDelegate::~AppDelegate()
{
}

// 初始化OpenGL上下文属性
void AppDelegate::initGLContextAttrs()
{
    
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8, 0 };
    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // 初始化导演实例
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = GLViewImpl::createWithRect("Card Game", cocos2d::Rect(0, 0, 540, 1040), 1.0f);
        director->setOpenGLView(glview);
    }

    // 设置设计分辨率
    glview->setDesignResolutionSize(1080, 2080, ResolutionPolicy::SHOW_ALL);

    // 设置帧率
    director->setAnimationInterval(1.0f / 60);

    // 开启调试信息
    director->setDisplayStats(false);

    // 添加资源搜索路径
    auto fileUtils = FileUtils::getInstance();
    std::vector<std::string> searchPaths;
    searchPaths.push_back("res");
    searchPaths.push_back(".");
    fileUtils->setSearchPaths(searchPaths);

    CCLOG("Application initialized successfully");

    // 创建并运行第一个场景
    auto scene = Scene::create();

    // 创建游戏控制器 - 确保它被场景持有
    auto gameController = GameController::create();
    if (gameController) {
        // 将控制器添加到场景，确保生命周期被管理
        scene->addChild(gameController);

        // 开始游戏
        gameController->startGame(1);

        // 将游戏视图添加到场景
        auto gameView = gameController->getGameView();
        if (gameView) {
            scene->addChild(gameView);
        }
        else {
            CCLOG("ERROR: GameView is null!");
        }

        // 运行场景
        director->runWithScene(scene);

        CCLOG("Game started successfully!");
        return true;
    }

    CCLOG("Failed to create game controller!");
    return false;
}

void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();
}

void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();
}