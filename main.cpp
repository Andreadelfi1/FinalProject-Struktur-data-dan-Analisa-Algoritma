#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <string>

// struktur partikel
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float radius;
    sf::Color color;
    
    // konstruktor partikel
    Particle(float x, float y, float r) {
        position = sf::Vector2f(x, y);
        radius = r;
        velocity.x = (rand() % 400 - 200) / 10.0f;
        velocity.y = (rand() % 400 - 200) / 10.0f;
        color = sf::Color(rand() % 256, rand() % 256, rand() % 256);
    }
    
    // Cek tabrakan dinding
    void update(float dt, float width, float height) {
        position.x += velocity.x * dt * 100;
        position.y += velocity.y * dt * 100;
        
        if (position.x - radius < 0 || position.x + radius > width) {
            velocity.x = -velocity.x;
            position.x = (position.x < width/2) ? radius : width - radius;
        }
        if (position.y - radius < 0 || position.y + radius > height) {
            velocity.y = -velocity.y;
            position.y = (position.y < height/2) ? radius : height - radius;
        }
    }
    
    void draw(sf::RenderWindow& window) {
        sf::CircleShape shape(radius);
        shape.setPosition(sf::Vector2f(position.x - radius, position.y - radius));
        shape.setFillColor(color);
        shape.setOutlineThickness(1);
        shape.setOutlineColor(sf::Color::White);
        window.draw(shape);
    }
};

// Area Persegi Quadtree
struct Rectangle {
    float x, y, w, h;
    
    Rectangle(float _x, float _y, float _w, float _h) 
        : x(_x), y(_y), w(_w), h(_h) {}
    
    bool contains(const Particle& p) const {
        return (p.position.x >= x && p.position.x <= x + w &&
                p.position.y >= y && p.position.y <= y + h);
    }
    
    bool intersects(const Rectangle& range) const {
        return !(range.x > x + w || range.x + range.w < x ||
                 range.y > y + h || range.y + range.h < y);
    }
};

class Quadtree {
private:
    static const int CAPACITY = 4;
    Rectangle boundary;
    std::vector<Particle*> particles;
    bool divided;
    std::unique_ptr<Quadtree> nw, ne, sw, se;
    
public:
    Quadtree(const Rectangle& rect) : boundary(rect), divided(false) {
        particles.reserve(CAPACITY);
    }
    
    // Bagi Quadtree menjadi 4 bagian
    void subdivide() {
        float x = boundary.x;
        float y = boundary.y;
        float w = boundary.w / 2;
        float h = boundary.h / 2;
        
        // Buat 4 child quadtree
        nw = std::make_unique<Quadtree>(Rectangle(x, y, w, h));
        ne = std::make_unique<Quadtree>(Rectangle(x + w, y, w, h));
        sw = std::make_unique<Quadtree>(Rectangle(x, y + h, w, h));
        se = std::make_unique<Quadtree>(Rectangle(x + w, y + h, w, h));
        
        divided = true;
    }
    
    bool insert(Particle* p) {
        if (!boundary.contains(*p)) return false;
        
        if (particles.size() < CAPACITY) {
            particles.push_back(p);
            return true;
        }
    
        if (!divided) subdivide();
        
        return (nw->insert(p) || ne->insert(p) || sw->insert(p) || se->insert(p));
    }
    
    void query(const Rectangle& range, std::vector<Particle*>& found) {
        if (!boundary.intersects(range)) return;
        
        for (auto p : particles) {
            if (range.contains(*p)) {
                found.push_back(p);
            }
        }
        
        if (divided) {
            nw->query(range, found);
            ne->query(range, found);
            sw->query(range, found);
            se->query(range, found);
        }
    }
    
    void draw(sf::RenderWindow& window) {
        sf::RectangleShape rect(sf::Vector2f(boundary.w, boundary.h));
        rect.setPosition(sf::Vector2f(boundary.x, boundary.y));
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color(0, 255, 0, 80));
        rect.setOutlineThickness(1);
        window.draw(rect);
        
        if (divided) {
            nw->draw(window);
            ne->draw(window);
            sw->draw(window);
            se->draw(window);
        }
    }
};

bool checkCollision(Particle& p1, Particle& p2) {
    float dx = p2.position.x - p1.position.x;
    float dy = p2.position.y - p1.position.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    return distance < (p1.radius + p2.radius);
}

void resolveCollision(Particle& p1, Particle& p2) {
    float dx = p2.position.x - p1.position.x;
    float dy = p2.position.y - p1.position.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance == 0) return;

    float nx = dx / distance;
    float ny = dy / distance;
    
    float dvx = p2.velocity.x - p1.velocity.x;
    float dvy = p2.velocity.y - p1.velocity.y;
    float dvn = dvx * nx + dvy * ny;
    
    if (dvn > 0) return;
    
    float impulse = dvn;
    p1.velocity.x += impulse * nx;
    p1.velocity.y += impulse * ny;
    p2.velocity.x -= impulse * nx;
    p2.velocity.y -= impulse * ny;
    
    float overlap = (p1.radius + p2.radius) - distance;
    if (overlap > 0) {
        p1.position.x -= nx * overlap * 0.5f;
        p1.position.y -= ny * overlap * 0.5f;
        p2.position.x += nx * overlap * 0.5f;
        p2.position.y += ny * overlap * 0.5f;
    }
}

int bruteForceCollision(std::vector<Particle>& particles, int& checks) {
    checks = 0;
    int collisions = 0;
    
    for (size_t i = 0; i < particles.size(); i++) {
        for (size_t j = i + 1; j < particles.size(); j++) {
            checks++;
            if (checkCollision(particles[i], particles[j])) {
                resolveCollision(particles[i], particles[j]);
                collisions++;
            }
        }
    }
    return collisions;
}

int quadtreeCollision(std::vector<Particle>& particles, Quadtree& qtree, int& checks) {
    checks = 0;
    int collisions = 0;
    
    for (size_t i = 0; i < particles.size(); i++) {
        Particle& p = particles[i];
        
        float searchRadius = p.radius * 3;
        Rectangle searchArea(
            p.position.x - searchRadius,
            p.position.y - searchRadius,
            searchRadius * 2,
            searchRadius * 2
        );
        
        std::vector<Particle*> nearby;
        qtree.query(searchArea, nearby);
        
        for (auto other : nearby) {
            if (&p < other) {
                checks++;
                if (checkCollision(p, *other)) {
                    resolveCollision(p, *other);
                    collisions++;
                }
            }
        }
    }
    return collisions;
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    
    const int WIDTH = 1200;
    const int HEIGHT = 800;
    
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "FP Strukdat");
    window.setFramerateLimit(60);
    
    std::vector<Particle> particles;
    sf::Clock clock;
    
    bool useQuadtree = true;
    bool showGrid = false;
    int checks = 0;
    
    sf::Font font;
    font.openFromFile("arial.ttf");
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    for (int i = 0; i < 5; i++) {
                        float r = 10 + rand() % 20;
                        float x = 100 + rand() % (WIDTH - 200);
                        float y = 100 + rand() % (HEIGHT - 200);
                        particles.push_back(Particle(x, y, r));
                    }
                }
                if (keyPressed->code == sf::Keyboard::Key::R) {
                    particles.clear();
                }
                if (keyPressed->code == sf::Keyboard::Key::Q) {
                    useQuadtree = !useQuadtree;
                }
                if (keyPressed->code == sf::Keyboard::Key::V) {
                    showGrid = !showGrid;
                }
            }
        }
        
        float dt = clock.restart().asSeconds();
        
        for (auto& p : particles) {
            p.update(dt, WIDTH, HEIGHT);
        }
        
        int collisions = 0;
        
        if (useQuadtree) {
            Quadtree qtree(Rectangle(0, 0, WIDTH, HEIGHT));
            for (auto& p : particles) {
                qtree.insert(&p);
            }
            collisions = quadtreeCollision(particles, qtree, checks);
            
            window.clear(sf::Color(20, 20, 30));
            if (showGrid) {
                qtree.draw(window);
            }
        } else {
            collisions = bruteForceCollision(particles, checks);
            window.clear(sf::Color(20, 20, 30));
        }
        
        for (auto& p : particles) {
            p.draw(window);
        }
        
        std::string info = "=== PARTICLE COLLISION SIMULATION ===\n\n";
        info += "Algorithm: " + std::string(useQuadtree ? "QUADTREE" : "BRUTE FORCE") + "\n";
        info += "Particles: " + std::to_string(particles.size()) + "\n";
        info += "FPS: " + std::to_string((int)(1.0f / dt)) + "\n";
        info += "Checks/Frame: " + std::to_string(checks) + "\n";
        info += "Collisions/Frame: " + std::to_string(collisions) + "\n\n";
        info += "[SPACE] Add 5 Particles\n";
        info += "[Q] Toggle Algorithm\n";
        info += "[V] Toggle Grid\n";
        info += "[R] Reset";
        
        sf::Text text(font, info, 16);
        text.setFillColor(sf::Color::White);
        text.setPosition({10.f, 10.f});
        window.draw(text);
        
        window.display();
    }
    
    return 0;
}