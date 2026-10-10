#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QMenuBar>
#include <QMenu>
#include <QFrame>
#include <QPainter>
#include <QPainterPath>

// Виджет постера игры из раздела "Недавние игры"
class GamePosterCard : public QFrame {
public:
    GamePosterCard(const QString& title, const QString& subtitle, const QColor& bgColor, 
                   bool hasDemoBadge = false, bool isPlayable = false, QWidget* parent = nullptr)
        : QFrame(parent) {
        setFixedSize(145, 215);
        setObjectName("posterCard");

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(8, 8, 8, 8);

        if (hasDemoBadge) {
            auto* demoBadge = new QLabel("ДЕМО", this);
            demoBadge->setStyleSheet("background-color: #799900; color: #ffffff; font-size: 10px; font-weight: bold; padding: 2px 5px; border-radius: 2px;");
            demoBadge->setFixedWidth(45);
            layout->addWidget(demoBadge);
        }

        layout->addStretch();

        auto* titleLabel = new QLabel(title, this);
        titleLabel->setStyleSheet("color: #ffffff; font-weight: bold; font-size: 13px;");
        titleLabel->setWordWrap(true);
        layout->addWidget(titleLabel);

        if (isPlayable) {
            auto* playRow = new QWidget(this);
            auto* playLayout = new QHBoxLayout(playRow);
            playLayout->setContentsMargins(0, 4, 0, 0);
            playLayout->setSpacing(6);

            auto* playBtn = new QPushButton("▶", playRow);
            playBtn->setFixedSize(32, 32);
            playBtn->setStyleSheet(R"(
                QPushButton {
                    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #799900, stop:1 #8ed000);
                    color: white; border: none; border-radius: 3px; font-size: 13px; font-weight: bold;
                }
                QPushButton:hover { background: #9ee600; }
            )");

            auto* timeLabel = new QLabel(subtitle, playRow);
            timeLabel->setStyleSheet("color: #8f98a0; font-size: 9px;");

            playLayout->addWidget(playBtn);
            playLayout->addWidget(timeLabel);
            layout->addWidget(playRow);
        } else if (!subtitle.isEmpty()) {
            auto* subLabel = new QLabel(subtitle, this);
            subLabel->setStyleSheet("color: #8f98a0; font-size: 11px;");
            layout->addWidget(subLabel);
        }

        setStyleSheet(QString("GamePosterCard { background-color: %1; border-radius: 4px; }").arg(bgColor.name()));
    }
};

// Карточка новости из раздела "Что нового"
class NewsCard : public QFrame {
public:
    NewsCard(const QString& tag, const QString& title, const QString& author, const QColor& accentColor, QWidget* parent = nullptr)
        : QFrame(parent) {
        setFixedSize(210, 150);
        setObjectName("newsCard");

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(4);

        auto* banner = new QFrame(this);
        banner->setFixedHeight(85);
        banner->setStyleSheet(QString("background-color: %1; border-radius: 3px;").arg(accentColor.name()));
        layout->addWidget(banner);

        auto* tagLabel = new QLabel(tag, this);
        tagLabel->setStyleSheet("color: #8f98a0; font-size: 10px;");
        layout->addWidget(tagLabel);

        auto* titleLabel = new QLabel(title, this);
        titleLabel->setStyleSheet("color: #ebebeb; font-weight: bold; font-size: 11px;");
        layout->addWidget(titleLabel);

        auto* authorLabel = new QLabel(author, this);
        authorLabel->setStyleSheet("color: #8091a2; font-size: 10px;");
        layout->addWidget(authorLabel);

        setStyleSheet(R"(
            QFrame#newsCard {
                background-color: #141c24;
                border-radius: 4px;
            }
            QFrame#newsCard:hover {
                background-color: #1e2936;
            }
        )");
    }
};

class SteamMainWindow : public QMainWindow {
public:
    SteamMainWindow() {
        setWindowTitle("Steam");
        resize(1280, 800);
        setupMenus();
        setupUI();
        applyGlobalStyles();
    }

private:
    void setupMenus() {
        auto* mb = menuBar();
        mb->addMenu("Steam");
        mb->addMenu("Вид");
        mb->addMenu("Друзья");
        mb->addMenu("Игры");
        mb->addMenu("Справка");
    }

    void setupUI() {
        auto* central = new QWidget(this);
        setCentralWidget(central);
        auto* rootLayout = new QVBoxLayout(central);
        rootLayout->setContentsMargins(0, 0, 0, 0);
        rootLayout->setSpacing(0);

        // --- 1. Верхняя панель навигации и профиля ---
        auto* navBar = new QWidget(this);
        navBar->setObjectName("navBar");
        navBar->setFixedHeight(48);
        auto* navLayout = new QHBoxLayout(navBar);
        navLayout->setContentsMargins(15, 0, 15, 0);
        navLayout->setSpacing(12);

        auto* backBtn = new QPushButton("←", navBar);
        auto* forwardBtn = new QPushButton("→", navBar);
        backBtn->setObjectName("historyBtn");
        forwardBtn->setObjectName("historyBtn");
        navLayout->addWidget(backBtn);
        navLayout->addWidget(forwardBtn);
        navLayout->addSpacing(10);

        auto* storeBtn = new QPushButton("МАГАЗИН", navBar);
        auto* libBtn = new QPushButton("БИБЛИОТЕКА", navBar);
        auto* commBtn = new QPushButton("СООБЩЕСТВО", navBar);
        auto* profileBtn = new QPushButton("HLCHEEL", navBar);

        storeBtn->setObjectName("navTab");
        libBtn->setObjectName("navTabActive");
        commBtn->setObjectName("navTab");
        profileBtn->setObjectName("navTab");

        navLayout->addWidget(storeBtn);
        navLayout->addWidget(libBtn);
        navLayout->addWidget(commBtn);
        navLayout->addWidget(profileBtn);

        navLayout->addStretch();

        // Правая часть навигации
        auto* newsBell = new QPushButton("📢", navBar);
        auto* notifyBell = new QPushButton("🔔", navBar);
        newsBell->setObjectName("iconToolBtn");
        notifyBell->setObjectName("iconToolBtn");
        navLayout->addWidget(newsBell);
        navLayout->addWidget(notifyBell);

        auto* userBadge = new QWidget(navBar);
        auto* userLayout = new QHBoxLayout(userBadge);
        userLayout->setContentsMargins(0, 0, 0, 0);
        userLayout->setSpacing(6);

        auto* avatar = new QLabel("👤", userBadge);
        avatar->setStyleSheet("background-color: #2a475e; border-radius: 2px; padding: 2px; font-size: 14px;");
        auto* userName = new QLabel("HLCHEEL  <font color='#a4d007'>$0.22</font>", userBadge);
        userName->setStyleSheet("color: #dcdedf; font-size: 12px;");

        userLayout->addWidget(avatar);
        userLayout->addWidget(userName);
        navLayout->addWidget(userBadge);

        rootLayout->addWidget(navBar);

        // --- 2. Основная рабочая область (Сплиттер) ---
        auto* mainArea = new QWidget(this);
        auto* mainLayout = new QHBoxLayout(mainArea);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Левая панель: Список игр
        auto* leftPanel = new QWidget(this);
        leftPanel->setObjectName("leftPanel");
        leftPanel->setFixedWidth(260);
        auto* leftLayout = new QVBoxLayout(leftPanel);
        leftLayout->setContentsMargins(12, 10, 12, 0);
        leftLayout->setSpacing(8);

        // Заголовок и фильтры списка
        auto* libHeader = new QWidget(leftPanel);
        auto* libHeaderLayout = new QHBoxLayout(libHeader);
        libHeaderLayout->setContentsMargins(0, 0, 0, 0);
        auto* libTitle = new QLabel("Библиотека", libHeader);
        libTitle->setStyleSheet("color: #ffffff; font-size: 14px; font-weight: bold;");
        auto* viewToggle = new QLabel("⊞", libHeader);
        viewToggle->setStyleSheet("color: #8f98a0; font-size: 14px;");
        libHeaderLayout->addWidget(libTitle);
        libHeaderLayout->addStretch();
        libHeaderLayout->addWidget(viewToggle);
        leftLayout->addWidget(libHeader);

        auto* filterRow = new QLabel("Игры и Программы  ▾", leftPanel);
        filterRow->setStyleSheet("color: #8f98a0; font-size: 11px;");
        leftLayout->addWidget(filterRow);

        auto* searchEdit = new QLineEdit(leftPanel);
        searchEdit->setPlaceholderText("🔍 Поиск по названию");
        searchEdit->setObjectName("searchField");
        leftLayout->addWidget(searchEdit);

        // Список игр со скриншота
        auto* gameList = new QListWidget(leftPanel);
        gameList->setObjectName("gameList");
        QStringList games = {
            "👾 RetroArch",
            "🏖️ Sandcastle Demo",
            "📦 s&box",
            "⚙️ Scrap Mechanic",
            "🔊 Soundpad Demo",
            "🎬 Source Filmmaker",
            "🚀 Space Station 14 Playtest",
            "🎨 SuchArt: Creative Space",
            "λ  Sven Co-op",
            "⚡ Synergy",
            "🎯 Team Fortress 2 Classified",
            "🔨 Teardown",
            "🌳 Terraria",
            "🔺 THE FINALS",
            "💰 Thief Simulator",
            "🏰 Tiny Glade Demo"
        };
        for (const auto& g : games) {
            gameList->addItem(g);
        }
        leftLayout->addWidget(gameList);
        mainLayout->addWidget(leftPanel);

        // Правая часть: Витрина библиотеки со скроллом
        auto* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);
        scrollArea->setObjectName("contentScroll");

        auto* scrollContent = new QWidget();
        scrollContent->setObjectName("scrollContent");
        auto* contentLayout = new QVBoxLayout(scrollContent);
        contentLayout->setContentsMargins(24, 18, 24, 20);
        contentLayout->setSpacing(20);

        // Секция 1: Что нового
        auto* newsHeader = new QLabel("Что нового  ⚙", scrollContent);
        newsHeader->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold;");
        contentLayout->addWidget(newsHeader);

        auto* newsRow = new QWidget(scrollContent);
        auto* newsLayout = new QHBoxLayout(newsRow);
        newsLayout->setContentsMargins(0, 0, 0, 0);
        newsLayout->setSpacing(12);
        newsLayout->addWidget(new NewsCard("на этой неделе", "Triple-I Initiative Sale!", "Guilty as Sock!", QColor("#d38200")));
        newsLayout->addWidget(new NewsCard("на этой неделе", "Crash and Learn | Story", "Trailmakers", QColor("#1e5799")));
        newsLayout->addWidget(new NewsCard("Сегодня", "Make Your Move.", "Deadlock", QColor("#49583b")));
        newsLayout->addWidget(new NewsCard("на этой неделе", "World Spotlight: City", "VRChat", QColor("#3c2b5c")));
        newsLayout->addStretch();
        contentLayout->addWidget(newsRow);

        // Кнопка разделителя
        auto* addSectionBtn = new QPushButton("✚ Добавить новый раздел", scrollContent);
        addSectionBtn->setObjectName("addSectionBtn");
        contentLayout->addWidget(addSectionBtn, 0, Qt::AlignCenter);

        // Секция 2: Недавние игры
        auto* recentHeader = new QLabel("Недавние игры  ▾", scrollContent);
        recentHeader->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold;");
        contentLayout->addWidget(recentHeader);

        auto* postersRow = new QWidget(scrollContent);
        auto* postersLayout = new QHBoxLayout(postersRow);
        postersLayout->setContentsMargins(0, 0, 0, 0);
        postersLayout->setSpacing(12);

        postersLayout->addWidget(new GamePosterCard("GRANULE", "60 мин. за две недели\n3 ч. всего", QColor("#1b2838"), false, true));
        postersLayout->addWidget(new GamePosterCard("garry's mod", "", QColor("#0e5f99")));
        postersLayout->addWidget(new GamePosterCard("GODOT\nEngine", "", QColor("#22394f")));
        postersLayout->addWidget(new GamePosterCard("Soundpad", "", QColor("#661111"), true));
        postersLayout->addWidget(new GamePosterCard("HELLO\nNEIGHBOR 2", "", QColor("#36251b")));
        postersLayout->addWidget(new GamePosterCard("HELLO\nNEIGHBOR", "", QColor("#4d4323")));
        postersLayout->addStretch();
        contentLayout->addWidget(postersRow);

        contentLayout->addStretch();
        scrollArea->setWidget(scrollContent);
        mainLayout->addWidget(scrollArea);

        rootLayout->addWidget(mainArea);

        // --- 3. Нижний статус-бар ---
        auto* bottomBar = new QWidget(this);
        bottomBar->setObjectName("bottomBar");
        bottomBar->setFixedHeight(36);
        auto* bottomLayout = new QHBoxLayout(bottomBar);
        bottomLayout->setContentsMargins(15, 0, 15, 0);

        auto* addGameBtn = new QPushButton("＋ Добавить игру", bottomBar);
        addGameBtn->setObjectName("bottomLinkBtn");
        bottomLayout->addWidget(addGameBtn);

        bottomLayout->addStretch();

        auto* downloadsBtn = new QPushButton("⤓ Управление загрузками", bottomBar);
        downloadsBtn->setObjectName("bottomLinkBtn");
        bottomLayout->addWidget(downloadsBtn);

        bottomLayout->addStretch();

        auto* friendsBtn = new QPushButton("Друзья и чат", bottomBar);
        friendsBtn->setObjectName("friendsBtn");
        bottomLayout->addWidget(friendsBtn);

        rootLayout->addWidget(bottomBar);
    }

    void applyGlobalStyles() {
        setStyleSheet(R"(
            QMainWindow {
                background-color: #171d25;
            }
            QMenuBar {
                background-color: #171a21;
                color: #b8b6b4;
                font-size: 11px;
                border-bottom: 1px solid #111417;
            }
            QMenuBar::item:selected {
                background-color: #212c3d;
                color: #ffffff;
            }
            #navBar {
                background-color: #171d25;
                border-bottom: 1px solid #0e141b;
            }
            QPushButton#historyBtn {
                background: transparent;
                color: #67707b;
                border: none;
                font-size: 16px;
                font-weight: bold;
            }
            QPushButton#historyBtn:hover {
                color: #c6d4df;
            }
            QPushButton#navTab {
                background: transparent;
                border: none;
                color: #b8b6b4;
                font-size: 13px;
                font-weight: bold;
                padding: 10px 4px;
            }
            QPushButton#navTab:hover {
                color: #ffffff;
            }
            QPushButton#navTabActive {
                background: transparent;
                border: none;
                border-bottom: 3px solid #1a9fff;
                color: #ffffff;
                font-size: 13px;
                font-weight: bold;
                padding: 10px 4px;
            }
            QPushButton#iconToolBtn {
                background: #212c3d;
                color: #67c1f5;
                border: none;
                border-radius: 2px;
                padding: 4px 8px;
            }
            #leftPanel {
                background-color: #18212c;
                border-right: 1px solid #121820;
            }
            QLineEdit#searchField {
                background-color: #12171e;
                border: 1px solid #000000;
                border-radius: 3px;
                color: #c6d4df;
                padding: 5px 8px;
                font-size: 11px;
            }
            QListWidget#gameList {
                background-color: transparent;
                border: none;
                color: #8f98a0;
                font-size: 12px;
                outline: none;
            }
            QListWidget#gameList::item {
                padding: 5px 4px;
                border-radius: 3px;
            }
            QListWidget#gameList::item:hover {
                background-color: #212f3f;
                color: #c7d5e0;
            }
            QListWidget#gameList::item:selected {
                background-color: #2a3f5a;
                color: #ffffff;
                font-weight: bold;
            }
            #contentScroll {
                border: none;
                background-color: #1a222d;
            }
            #scrollContent {
                background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #1b2634, stop:1 #141c26);
            }
            QPushButton#addSectionBtn {
                background: transparent;
                border: 1px dashed #344458;
                border-radius: 4px;
                color: #67707b;
                font-size: 12px;
                padding: 8px 16px;
            }
            QPushButton#addSectionBtn:hover {
                border-color: #67c1f5;
                color: #ffffff;
            }
            #bottomBar {
                background-color: #171a21;
                border-top: 1px solid #0e141b;
            }
            QPushButton#bottomLinkBtn {
                background: transparent;
                border: none;
                color: #8f98a0;
                font-size: 11px;
            }
            QPushButton#bottomLinkBtn:hover {
                color: #ffffff;
            }
            QPushButton#friendsBtn {
                background-color: #223042;
                border: none;
                border-radius: 2px;
                color: #92b8d8;
                font-size: 11px;
                font-weight: bold;
                padding: 4px 12px;
            }
            QPushButton#friendsBtn:hover {
                background-color: #2b3d54;
                color: #ffffff;
            }
        )");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SteamMainWindow win;
    win.show();
    return app.exec();
}
