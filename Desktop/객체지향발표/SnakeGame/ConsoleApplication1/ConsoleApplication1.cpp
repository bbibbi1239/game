#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>

using namespace sf;
using namespace std;

enum class Direction { STOP = 0, LEFT = 1, RIGHT = 2, UP = 3, DOWN = 4 };

class GameBase {
protected:
    bool gameOver; // 게임이 종료되었는지 여부를 나타내는 변수
    const int width = 20; // 게임 영역의 너비
    const int height = 20; // 게임 영역의 높이
    RenderWindow window; // 게임 창을 나타내는 SFML의 RenderWindow 객체

public:
    // 생성자: 게임 창을 초기화하고 제목을 "스네이크 게임"으로 설정
    GameBase() : window(VideoMode(800, 800), "스네이크 게임") {}

    // 순수 가상 함수: 자식 클래스에서 반드시 구현해야 하는 함수들
    virtual void initialize() = 0; // 게임 초기화 함수
    virtual void render() = 0;      // 화면 렌더링 함수
    virtual void input() = 0;       // 사용자 입력 처리 함수
    virtual void logic() = 0;       // 게임 논리 처리 함수

    // 게임이 종료되었는지 확인하는 함수
    bool isGameOver() { return gameOver; }

    // 게임 창에 대한 참조를 반환하는 함수
    RenderWindow& getWindow() { return window; }
};


class SnakeGame : public GameBase {
private:
    vector<Vector2i> snake; // 뱀의 몸통을 저장하는 리스트
    Vector2i fruit; // 과일의 위치
    Direction dir; // 뱀의 방향
    int score; // 점수
    bool messageDisplayed; // 메시지 출력 여부를 추적하는 변수

    // 과일을 랜덤하게 생성하는 함수
    void spawnFruit() {
        random_device rd; // 랜덤 숫자를 생성하기 위한 장치
        mt19937 gen(rd()); // 랜덤 숫자를 생성하는 엔진
        uniform_int_distribution<> dis(1, width - 2); // 과일이 나올 위치 범위 설정
        Vector2i newFruit; // 새로운 과일 위치
        bool validPosition = false; // 과일 위치가 유효한지 확인하는 변수

        // 유효한 위치를 찾을 때까지 반복
        while (!validPosition) {
            newFruit.x = dis(gen); // 랜덤한 x 좌표 생성
            newFruit.y = dis(gen); // 랜덤한 y 좌표 생성
            validPosition = true; // 기본적으로 유효한 위치로 설정

            // 뱀의 몸통과 같은 위치인지 확인
            for (const auto& body : snake) {
                if (body == newFruit) { // 뱀의 몸통과 겹치면
                    validPosition = false; // 유효하지 않다고 표시
                    break; // 반복문 중단
                }
            }
        }
        fruit = newFruit; // 유효한 위치에 과일 설정
    }

public:
    SnakeGame() : GameBase() { // 게임 초기화
        gameOver = false; // 게임이 끝나지 않았다고 설정
        dir = Direction::STOP; // 뱀의 방향을 정지로 설정
        score = 0; // 점수를 0으로 초기화
        messageDisplayed = false; // 메시지 출력 여부 초기화
        initialize(); // 게임 초기화 함수 호출
    }

    void initialize() override {
        snake.push_back({ width / 2, height / 2 }); // 뱀의 시작 위치를 중앙으로 설정
        spawnFruit(); // 처음 과일 생성
    }

    void render() override {
        getWindow().clear(Color::Black); // 화면을 검은색으로 지우기

        // 게임 영역 그리기 (윗벽)
        for (int i = 0; i <= width; ++i) {
            RectangleShape wall(Vector2f(40, 40)); // 벽을 나타내는 사각형 생성
            wall.setFillColor(Color::White); // 벽 색상을 흰색으로 설정
            wall.setPosition(i * 40, 0); // 벽의 위치 설정
            getWindow().draw(wall); // 벽 그리기
        }

        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                if (j == 0 || i == 0 || j == width - 1 || i == height - 1) {
                    RectangleShape wall(Vector2f(40, 40)); // 벽을 나타내는 사각형 생성
                    wall.setFillColor(Color::White);
                    wall.setPosition(j * 40, (i) * 40);
                    getWindow().draw(wall); // 벽 그리기
                }
                else {
                    for (const auto& body : snake) {
                        if (body.x == j && body.y == i) {
                            RectangleShape bodyPart(Vector2f(40, 40)); // 뱀 몸통 부분 그리기
                            bodyPart.setFillColor(Color::Green);
                            bodyPart.setPosition(j * 40, (i) * 40);
                            getWindow().draw(bodyPart); // 뱀 몸통 그리기
                        }
                    }
                    if (fruit.x == j && fruit.y == i) {
                        CircleShape fruitShape(10); // 과일을 나타내는 원 생성
                        fruitShape.setFillColor(Color::Red);
                        fruitShape.setPosition(j * 40 + 10, (i) * 40 + 10);
                        getWindow().draw(fruitShape); // 과일 그리기
                    }
                }
            }
        }

        getWindow().display(); // 화면에 모든 것을 표시하기
    }

    void input() override {
        if (Keyboard::isKeyPressed(Keyboard::Left) && dir != Direction::RIGHT) {
            dir = Direction::LEFT; // 왼쪽 키를 누르면 왼쪽으로 방향 전환
        }
        else if (Keyboard::isKeyPressed(Keyboard::Right) && dir != Direction::LEFT) {
            dir = Direction::RIGHT; // 오른쪽 키를 누르면 오른쪽으로 방향 전환
        }
        else if (Keyboard::isKeyPressed(Keyboard::Up) && dir != Direction::DOWN) {
            dir = Direction::UP; // 위쪽 키를 누르면 위로 방향 전환
        }
        else if (Keyboard::isKeyPressed(Keyboard::Down) && dir != Direction::UP) {
            dir = Direction::DOWN; // 아래쪽 키를 누르면 아래로 방향 전환
        }
        else if (Keyboard::isKeyPressed(Keyboard::X)) {
            gameOver = true; // X 키를 누르면 게임 종료 설정
        }
    }

    void logic() override {
        Vector2i prev = snake[0];

        switch (dir) {
        case Direction::LEFT: snake[0].x--; break;
        case Direction::RIGHT: snake[0].x++; break;
        case Direction::UP: snake[0].y--; break;
        case Direction::DOWN: snake[0].y++; break;
        default: break;
        }

        for (size_t i = 1; i < snake.size(); ++i) {
            Vector2i prev2 = snake[i];
            snake[i] = prev;
            prev = prev2;
        }

        if (snake[0] == fruit) {
            score += 10; // 과일을 먹으면 점수 증가
            spawnFruit(); // 새로운 과일 생성
            snake.push_back(prev); // 뱀 길이 늘리기 위해 마지막 부분 추가
        }

        // 벽과 충돌 체크 및 자기 몸통과 충돌 체크
        if (snake[0].x < 0 || snake[0].x >= width || snake[0].y < 0 || snake[0].y >= height ||
            std::find(snake.begin() + 1, snake.end(), snake[0]) != snake.end()) {
            if (!messageDisplayed) {
                cout << score << endl;
                cout << "OVER : 살아남기 힘들다,,,," << endl;
                messageDisplayed = true;
            }
            gameOver = true;
        }

        if (snake.size() >= 10) {
            cout << score << endl;
            cout << "CLEAR : 성체가 되었다!!" << endl;
            gameOver = true;
        }
    }
};

int main() {
    SnakeGame game; // SnakeGame 클래스의 인스턴스를 생성하여 게임을 초기화
    Clock clock;    // 게임의 프레임 간 시간을 측정하기 위한 Clock 객체 생성

    // 게임 창이 열려 있고 게임이 종료되지 않은 동안 반복
    while (game.getWindow().isOpen() && !game.isGameOver()) {
        Event event; // 이벤트 처리를 위한 Event 객체 생성

        // 이벤트 큐에서 이벤트를 가져와 처리
        while (game.getWindow().pollEvent(event)) {
            if (event.type == Event::Closed) // 창 닫기 이벤트가 발생하면
                game.getWindow().close();    // 게임 창을 닫음
        }

        game.input();  // 사용자 입력을 처리하는 메서드 호출
        game.logic();  // 게임의 논리를 업데이트하는 메서드 호출
        game.render(); // 게임 화면을 렌더링하는 메서드 호출

        Time elapsed = clock.restart(); // 경과 시간을 측정하고 Clock을 재시작
        sleep(milliseconds(200) - elapsed); // 다음 프레임까지 대기, 200ms에서 경과 시간을 뺀 만큼 대기
    }
    
    return 0; // 프로그램 종료
}

