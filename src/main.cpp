#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

int
main()
{
    int winH = 500;
    int winW = 1000;
    sf::RenderWindow window(sf::VideoMode(winW, winH), "SFML works!");
    //window.setVerticalSyncEnabled(true);
    //window.setFramerateLimit(60);

    int  maxY = 255, maxX = 3000;
    std::vector<int32_t> plot(maxX);
    float incX = (winW - 100) / float(maxX);

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

    window.clear(sf::Color(255, 255, 255));
    for(int i = 0; i < 2; i++) window.draw(axis[i]);

    sf::Vector2u size(maxX, maxY);
    sf::Image graph;
    graph.create(winW-100, winH-100, sf::Color(255, 255, 255));
    sf::Texture tex;
    tex.loadFromImage(graph);
    sf::Sprite sprite(tex);
    sprite.setPosition(53, 48);

    sf::Clock clock;
    int32_t time = 0, prev_time=0;
    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }
	
        time = (clock.getElapsedTime().asMilliseconds())%maxX;
	if(time<prev_time)
	  {
	    for(int i=0; i<plot.size(); i++)
	      graph.setPixel(i, plot[i], sf::Color(255, 255, 255));
	  }
	prev_time = time;
	
	plot[time*incX] = 20;
	
        graph.setPixel(time*incX, plot[time*incX], sf::Color(0, 0, 0));
	std::cout << incX*time << std::endl;
	tex.update(graph);

	window.clear(sf::Color(255, 255, 255));
	for(int i = 0; i < 2; i++) window.draw(axis[i]);
	
        window.draw(sprite);
	window.display();
    }

    return 0;
}
