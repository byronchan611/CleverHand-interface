#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>
#include <vector>

class graph;
class plot;

int winH = 500;
int winW = 1000;
int maxY = 255, maxX = 3000;
float incX = (winW - 100) / float(maxX);

void
GUI_thread(sf::Image &graph)
{
    sf::RenderWindow window(sf::VideoMode(winW, winH), "SFML works!");
    //window.setVerticalSyncEnabled(true);
    //window.setFramerateLimit(60);

    sf::RectangleShape axis[2];
    for(int i = 0; i < 2; i++)
    {
        float px = 50 - (1 - i) * 20;
        float py = winH - 50 + i * 20;
        float l = (1 - i) * (winW - 2 * px) + i * (py - 50);
        float a = -90.f * i;
        axis[i].setSize(sf::Vector2f(l, 1.f));
        axis[i].setFillColor(sf::Color::Black);
        axis[i].setPosition(px, py);
        axis[1].rotate(a);
    }

    sf::RectangleShape incs[2][4];
    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            float px = 50 - i * 5 + 100 * j * (1 - i);
            float py = winH - 50 + (1 - i) * 5 - 100 * j * i;
            float l = 10;
            float a = -90.f * (1-i);
            incs[i][j].setSize(sf::Vector2f(l, 1.f));
            incs[i][j].setFillColor(sf::Color::Black);
            incs[i][j].setPosition(px, py);
            incs[i][j].rotate(a);
        }
    }

    window.clear(sf::Color(255, 255, 255));
    for(int i = 0; i < 2; i++) window.draw(axis[i]);
    for(int i = 0; i < 2; i++)
        for(int j = 0; j < 4; j++) window.draw(incs[i][j]);

    sf::Vector2u size(maxX, maxY);

    sf::Texture tex;
    tex.loadFromImage(graph);
    sf::Sprite sprite(tex);
    sprite.setPosition(53, 48);

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        tex.update(graph);

        window.clear(sf::Color(255, 255, 255));

        window.draw(sprite);
        for(int i = 0; i < 2; i++) window.draw(axis[i]);
        for(int i = 0; i < 2; i++)
            for(int j = 0; j < 4; j++) window.draw(incs[i][j]);

        window.display();
    }
}

class graph
{
    public:
    graph();

    void
    setPix(int x, int y, sf::Color &col)
    {
        _img.setPixel(x, y, col);
    };

    void
    clrPix(int x, int y)
    {
        _img.setPixel(x, y, _color);
    };

    int _w;
    int _h;
    sf::Color _color;
    sf::Image _img;
    std::vector<plot> _plots;
};

class plot
{
    public:
    plot();

    void
    addPoint(int x, int y)
    {
        _values[x] = y;
        _graph.setPix(x, y, _color);
    };

    void
    clear()
    {
        for(int i = 0; i < _values.size(); i++) _graph.clrPix(i, _values[i]);
    };

    private:
    graph &_graph;
    sf::Image &_graphImg;
    std::vector<int32_t> _values;
    sf::Color _color;
};

int
main()
{
    sf::Image graph;

    graph.create(winW - 100, winH - 100, sf::Color(255, 255, 0));
    std::thread t1(GUI_thread, std::ref(graph));

    sf::Clock clock;
    std::vector<int32_t> plot(maxX);
    int32_t time = 0, prev_time = 0;
    while(1)
    {
        time = (clock.getElapsedTime().asMilliseconds()) % maxX;
        if(time < prev_time)
            for(int i = 0; i < maxX; i++)
                graph.setPixel(i, plot[i], sf::Color(255, 255, 255));
        prev_time = time;

        plot[time * incX] = int(time * incX) % maxY;

        graph.setPixel(time * incX, plot[time * incX], sf::Color(0, 0, 0));
        std::cout << incX * time << std::endl;
    }
    t1.join();
    return 0;
}
