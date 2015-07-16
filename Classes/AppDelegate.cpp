#include "AppDelegate.h"
#include "TitleScene.h"
#include "Constants.h"

USING_NS_CC;

AppDelegate::AppDelegate() {
}

AppDelegate::~AppDelegate() {
}

// if you want a different context,just modify the value of glContextAttrs
// it will takes effect on all platforms
void AppDelegate::initGLContextAttrs() {
    // set OpenGL context attributions,now can only set six attributions:
    // red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages() {
    return 0; // flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview) {
        glview = GLViewImpl::create(GAME_NAME.c_str());
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    glview->setDesignResolutionSize(1136, 640, ResolutionPolicy::FIXED_WIDTH);

    cocos2d::Size frameSize = glview->getFrameSize();
    float targetSize = frameSize.height > frameSize.width ? frameSize.height : frameSize.width;

    FileUtils::getInstance()->addSearchPath("res");
    FileUtils::getInstance()->addSearchPath("fonts");

    std::vector<std::string> searchResolutionOrder(1);

    searchResolutionOrder[0] = "resources-2x";
    /*
    if (targetSize < 481.0f) {
        searchResolutionOrder[0] = "resources-1x";
    } else if (targetSize < 1335.0f) {
        searchResolutionOrder[0] = "resources-2x";
    } else if (targetSize < 1921.0f) {
        searchResolutionOrder[0] = "resources-3x";
    } else {
        searchResolutionOrder[0] = "resources-4x";
    }
     */
    FileUtils::getInstance()->setSearchResolutionsOrder(searchResolutionOrder);

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = TitleScene::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}