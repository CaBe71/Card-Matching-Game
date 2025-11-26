# Card-Matching-Game

一个基于 Cocos2d-x 引擎开发的卡牌匹配游戏，采用经典的 MVC 架构设计，支持关卡配置和回退功能。

✨ 游戏特色
🎯 智能匹配 - 点数差1即可匹配的简单规则

🔄 回退功能 - 支持操作回退，轻松纠错

🎨 精美界面 - 完整的卡牌视觉表现

📊 关卡系统 - 可配置的关卡设计

🎮 流畅体验 - 丝滑的动画效果和响应式交互

🎲 游戏规则
基本规则
匹配条件：两张卡牌点数相差1即可匹配（无花色要求）

示例：A↔2, 2↔3, ..., Q↔K

操作方式
点击桌面卡牌：与底牌匹配成功则替换底牌，并在原位置生成新随机牌

点击手牌区：直接抽取新卡替换底牌

UNDO按钮：回退上一步操作

🏗️ 项目架构
核心架构模式
text
MVC (Model-View-Controller) 模式

├── Model (数据层)

├── View (视图层) 

└── Controller (控制层)

目录结构
text
Classes/

├── controllers/  # 控制器

│   └── GameController.h/cpp

├── models/              # 数据模型

│   ├── GameModel.h/cpp

│   ├── CardModel.h/cpp

│   └── CardTypes.h

├── views/               # 视图组件

│   ├── GameView.h/cpp

│   └── CardView.h/cpp

├── services/            # 服务层

│   ├── CardService.h/cpp

│   ├── CardGeneratorService.h/cpp

│   └── GameModelGenerator.h/cpp

├── managers/            # 管理器

│   └── UndoManager.h/cpp

└── configs/             # 配置管理
    ├── models/
    │   └── LevelConfig.h
    └── loaders/
        └── LevelConfigLoader.h/cpp
