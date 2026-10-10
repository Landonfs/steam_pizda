#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QProcess>
#include <QMessageBox>
#include <vector>

struct Game {
    QString title;
    QString developer;
    QString hoursPlayed;
    QString executablePath;
};

class SteamWindow : public QMainWindow {
public:
    SteamWindow() {
        setWindowTitle("Steam");
        resize(1100, 700);

        initData();
        setupUI();
        applySteamStyles();

        if (!games.empty()) {
            gameList->setCurrentRow(0);
        }
    }

private:
    std::vector<Game> games;
    QListWidget* gameList;
    QLabel* gameTitleLabel;
    QLabel* gameInfoLabel;
    QPushButton* playButton;
    int selectedIndex = 0;

    void initData() {
        games = {
            {"Counter-Strike 2", "Valve", "240 ч.", "notepad.exe"},
            {"Half-Life: Alyx", "Valve", "18 ч.", "calc.exe"},
            {"Portal 2", "Valve", "45 ч.", "mspaint.exe"},
            {"Dota 2", "Valve", "1,200 ч.", "cmd.exe"}
        };
    }

    void setupUI() {
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* rootLayout = new QVBoxLayout(centralWidget);
        rootLayout->setContentsMargins(0, 0, 0, 0);
        rootLayout->setSpacing(0);

        // Верхняя навигационная панель Steam
        auto* topBar = new QWidget();
        topBar->setObjectName("topBar");
        auto* topLayout = new QHBoxLayout(topBar);
        topLayout->setContentsMargins(15, 8, 15, 8);
        topLayout->setSpacing(25);

        QStringList navItems = {"МАГАЗИН", "БИБЛИОТЕКА", "СООБЩЕСТВО", "ПРОФИЛЬ"};
        for (const auto& item : navItems) {
            auto* navBtn = new QPushButton(item);
            navBtn->setObjectName("navButton");
            if (item == "БИБЛИОТЕКА") {
                navBtn->setObjectName("navButtonActive");
            }
            topLayout->addWidget(navBtn);
        }
        topLayout->addStretch();
        rootLayout->addWidget(topBar);

        // Основная рабочая область (Сплит: список слева, детали справа)
        auto* mainArea = new QWidget();
        auto* mainLayout = new QHBoxLayout(mainArea);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);

        // Боковая панель библиотеки
        auto* sidePanel = new QWidget();
        sidePanel->setObjectName("sidePanel");
        sidePanel->setFixedWidth(280);
        auto* sideLayout = new QVBoxLayout(sidePanel);
        sideLayout->setContentsMargins(10, 10, 10, 10);

        auto* listHeader = new QLabel("ИГРЫ");
        listHeader->setObjectName("subHeader");
        sideLayout->addWidget(listHeader);

        gameList = new QListWidget();
        gameList->setObjectName("gameList");
        for (const auto& g : games) {
            gameList->addItem(g.title);
        }
        sideLayout->addWidget(gameList);
        mainLayout->addWidget(sidePanel);

        // Правая часть — баннер и детали игры
        auto* detailsPanel = new QWidget();
        detailsPanel->setObjectName("detailsPanel");
        auto* detailsLayout = new QVBoxLayout(detailsPanel);
        detailsLayout->setContentsMargins(30, 30, 30, 30);

        gameTitleLabel = new QLabel();
        gameTitleLabel->setObjectName("gameTitle");
        detailsLayout->addWidget(gameTitleLabel);

        gameInfoLabel = new QLabel();
        gameInfoLabel->setObjectName("gameInfo");
        detailsLayout->addWidget(gameInfoLabel);

        detailsLayout->addSpacing(20);

        playButton = new QPushButton("В ИГРУ");
        playButton->setObjectName("playButton");
        playButton->setFixedSize(160, 48);
        detailsLayout->addWidget(playButton);

        detailsLayout->addStretch();
        mainLayout->addWidget(detailsPanel);

        rootLayout->addWidget(mainArea);

        // Подключение сигналов
        connect(gameList, &QListWidget::currentRowChanged, this, [this](int row) {
            if (row >= 0 && row < static_cast<int>(games.size())) {
                selectedIndex = row;
                updateDetails(games[row]);
            }
        });

        connect(playButton, &QPushButton::clicked, this, [this]() {
            launchGame(games[selectedIndex]);
        });
    }

    void updateDetails(const Game& game) {
        gameTitleLabel->setText(game.title);
        gameInfoLabel->setText(QString("Разработчик: %1  |  Время в игре: %2")
                                  .arg(game.developer, game.hoursPlayed));
    }

    void launchGame(const Game& game) {
        bool started = QProcess::startDetached(game.executablePath);
        if (!started) {
            QMessageBox::warning(this, "Ошибка запуска",
                                 "Не удалось запустить исполняемый файл: " + game.executablePath);
        }
    }

    void applySteamStyles() {
        setStyleSheet(R"(
            QMainWindow {
                background-color: #171a21;
            }
            #topBar {
                background-color: #171d25;
                border-bottom: 1px solid #101214;
            }
            QPushButton#navButton {
                background: transparent;
                border: none;
                color: #b8b6b4;
                font-size: 13px;
                font-weight: bold;
                padding: 4px;
            }
            QPushButton#navButton:hover {
                color: #ffffff;
            }
            QPushButton#navButtonActive {
                background: transparent;
                border: none;
                border-bottom: 2px solid #1a9fff;
                color: #ffffff;
                font-size: 13px;
                font-weight: bold;
                padding: 4px;
            }
            #sidePanel {
                background-color: #1b2838;
                border-right: 1px solid #141f2c;
            }
            #detailsPanel {
                background-color: #1b2838;
            }
            #subHeader {
                color: #626366;
                font-size: 11px;
                font-weight: bold;
                margin-bottom: 5px;
            }
            QListWidget#gameList {
                background-color: transparent;
                border: none;
                color: #c7d5e0;
                font-size: 13px;
                outline: none;
            }
            QListWidget#gameList::item {
                padding: 7px 5px;
                border-radius: 3px;
            }
            QListWidget#gameList::item:hover {
                background-color: #213247;
                color: #ffffff;
            }
            QListWidget#gameList::item:selected {
                background-color: #2a475e;
                color: #ffffff;
                font-weight: bold;
            }
            #gameTitle {
                color: #ffffff;
                font-size: 32px;
                font-weight: bold;
            }
            #gameInfo {
                color: #8f98a0;
                font-size: 14px;
                margin-top: 5px;
            }
            QPushButton#playButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #47bfff, stop:1 #1a9fff);
                border: none;
                border-radius: 2px;
                color: #ffffff;
                font-size: 16px;
                font-weight: bold;
            }
            QPushButton#playButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #67caff, stop:1 #33a9ff);
            }
            QPushButton#playButton:pressed {
                background-color: #1580cc;
            }
        )");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SteamWindow window;
    window.show();
    return app.exec();
}
