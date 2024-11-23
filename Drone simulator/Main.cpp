#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <stdexcept>

class Drone {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;
    float batteryLife;
    sf::Vector2f position;

    // ����������� ���� � ������������� ��������
    Drone(float init_battery) : batteryLife(init_battery) {
        if (!texture.loadFromFile("drone.png")) {
            throw std::runtime_error("�� ������� ����������� drone.png");
        }
        sprite.setTexture(texture);
        sprite.setScale(0.1f, 0.1f);  // ������� ������� ���������� ��� ������
        speed = 100.0f;  // ϳ���� �� �������
    }

    // ��� �� �����
    void moveTo(const sf::Vector2f& target, float deltaTime) {
        float distance = std::sqrt(std::pow(target.x - position.x, 2) + std::pow(target.y - position.y, 2));
        if (distance > 0.1f) {
            sf::Vector2f direction = (target - position) / distance;
            position += direction * speed * deltaTime;
            sprite.setPosition(position);
            batteryLife -= deltaTime;
        }
    }

    bool isBatteryLow() const {
        return batteryLife <= 0;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Drone simulator");

    // ������������ �����
    sf::Texture mapTexture;
    if (!mapTexture.loadFromFile("map.png")) {
        return -1;
    }
    sf::Sprite mapSprite(mapTexture);

    // ����������� ����
    Drone drone(20.0f);  // ������� �� 20 ������
    drone.position = { 100, 100 };
    drone.sprite.setPosition(drone.position);

    // ������������ ����� ��������
    std::vector<sf::Vector2f> route = { {200, 150}, {400, 300}, {600, 400}, {300, 500}, {100, 100} };

    // ��������� ���� ��� ����������� ����� ��������
    std::vector<sf::CircleShape> markers;
    for (const auto& point : route) {
        sf::CircleShape marker(5);
        marker.setFillColor(sf::Color::Green);  // ������� ���� ��� �����������
        marker.setPosition(point);
        markers.push_back(marker);
    }

    // ����� ��� ����������� ������
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return -1;  // ������� �������
    }

    sf::Text batteryText;
    batteryText.setFont(font);
    batteryText.setCharacterSize(24);  // ����� ������
    batteryText.setFillColor(sf::Color::Red);  // �������� ���� ��� ������
    batteryText.setPosition(650, 10);  // ��������� ������ � ������� ��������� ���

    size_t currentPoint = 0;
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();

        // ��� �� �������� �����
        if (currentPoint < route.size() && !drone.isBatteryLow()) {
            drone.moveTo(route[currentPoint], deltaTime);
            if (std::abs(drone.position.x - route[currentPoint].x) < 1.0f &&
                std::abs(drone.position.y - route[currentPoint].y) < 1.0f) {
                currentPoint++;
            }
        }
        else {
            // ���������� �� ���� (����� �����), ���� ����� �������
            drone.moveTo(route[0], deltaTime);
        }

        // ��������� ����
        window.clear();
        window.draw(mapSprite);

        // ������� ����� ��������
        for (const auto& marker : markers) {
            window.draw(marker);
        }

        // ������� ����
        window.draw(drone.sprite);

        // ��������� ������ ������
        batteryText.setString("Battery: " + std::to_string(static_cast<int>(drone.batteryLife)) + "s");
        window.draw(batteryText);  // ������� ����� ������

        window.display();
    }

    return 0;
}
