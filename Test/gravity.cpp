
#include <iostream>
#include "SFML/Graphics.hpp"
#include <chrono>
#include <vector>
#include <cmath>
#include <ratio>
#define forn(i, s, n) for(int i=s; i<n; i++)
#define vdebug(v) cout << #v << " = "; for(auto x: v){cout << x << ' ';}; cout << '\n';
#define debug(x) cout << #x << " = " << x << '\n';

using std::cout;
using namespace std::chrono;

class Body
{
public:
	sf::Vector2f pos, vel, accel; // position, velocity, acceleration all 0
	std::vector<sf::Vertex> trail;
	float mass;
	sf::CircleShape shape;
	steady_clock::time_point t;
	Body(float m, sf::Vector2f Pos, float radius) {
		t = steady_clock::now();
		mass = m;
		pos = Pos;	
		shape.setRadius(radius);
		shape.setOrigin(radius, radius);
		trail = std::vector<sf::Vertex>(100, sf::Vertex(Pos, sf::Color::Red));
	}			   
	Body(float m, sf::Vector2f Pos, sf::Vector2f Vel, float radius) {
		t = steady_clock::now();
		mass = m;
		pos = Pos; vel = Vel;
		shape.setRadius(radius);
		shape.setOrigin(radius, radius);
		trail = std::vector<sf::Vertex>(100, sf::Vertex(Pos, sf::Color::Red));
	}
	void updatePos(std::vector<Body> planets) {
		duration<float, std::micro> diff = duration_cast<std::chrono::microseconds>(steady_clock::now() - t); // in microseconds
		float dt = diff.count()/1000.f;
        t = steady_clock::now();
		sf::Vector2f newAccel = gravityAccel(planets);
		pos += vel * dt + accel * (dt * dt) * 0.5f;
		vel += 0.5f * (dt) * (accel + newAccel);
		accel = newAccel;
		shape.setPosition(pos);
		trail.pop_back();
		trail.insert(trail.begin(), sf::Vertex(pos, sf::Color::Red));
	}
	sf::Vector2f gravityAccel(std::vector<Body> planets) {
		sf::Vector2f newAccel = { 0.f, 0.f };
		for (Body planet : planets) {
			if (planet.pos == pos) {
				continue;
			}
			sf::Vector2f diff = planet.pos - pos;
			float rSquared = diff.y * diff.y + diff.x * diff.x;
			sf::Vector2f direction = diff / float(sqrt(rSquared+10));
			newAccel += direction * planet.mass / (rSquared+50000);
		}
		return newAccel;
	}
	sf::Vector2f outsideBounds(float left, float right, float top, float bottom) {
		int isLeft = pos.x < left;
		int isRight = pos.x > right;
		int isAbove = pos.y < top;
		int isBellow = pos.y > bottom;
		return sf::Vector2f(isRight - isLeft, isBellow - isAbove); 
	}
	void setColour(sf::Color colour) {
		shape.setFillColor(colour);
	}
};

void updatePlanets(std::vector<Body>& planets) {
	//time_t t = clock();
	for (Body& planet : planets) {
		planet.updatePos(planets);
	}
}

void keyBindings(sf::View& view, sf::RenderWindow& window, int dt) {
	using sf::Keyboard;
	float transFactor = 0.001f;
	float zoomFactor = 0.005f;
	float xSize = view.getSize().x; float ySize = view.getSize().y;
	float size = sqrt(xSize * xSize + ySize * ySize);
	if (Keyboard::isKeyPressed(Keyboard::Left)) {
		view.move(sf::Vector2f(-1.f*dt*transFactor*size, 0.f));
	}
	if (Keyboard::isKeyPressed(Keyboard::Right)) {
		view.move(sf::Vector2f(1.f*dt*transFactor*size, 0.f));
	}
	if (Keyboard::isKeyPressed(Keyboard::Up)) {
		view.move(sf::Vector2f(0.f, -1.f*dt*transFactor*size));
	}
	if (Keyboard::isKeyPressed(Keyboard::Down)) {
		view.move(sf::Vector2f(0.f, 1.f*dt*transFactor*size));
	}
	if (Keyboard::isKeyPressed(Keyboard::Equal)) {
		view.zoom(1.f-dt*zoomFactor);
	}
	if (Keyboard::isKeyPressed(Keyboard::Hyphen)) {
		view.zoom(1.f+dt*zoomFactor);
	}
}

void fpsCountUpdate(std::vector<int>& times, int dt, int& total) {
	total -= times.back();
	total += dt;
	times.pop_back();
	times.insert(times.begin(), dt);
}



int main() {
	const short int WIDTH = 1000, HEIGHT = 600;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Hi!");
	sf::View worldView;
	sf::View textView;
	worldView.setCenter(0.f, 0.f);
	worldView.setSize(WIDTH, HEIGHT);
	textView = worldView;
	window.setView(worldView);
	window.setFramerateLimit(244);
	sf::Event e;
	steady_clock::time_point time = steady_clock::now();

	sf::Font font;
	if (!font.loadFromFile("FiraCode-Regular.ttf")) {
		cout << "Could not load font" << std::endl;
	}
	sf::Text fpsCount;
	fpsCount.setFont(font);
	fpsCount.setString("hey");
	fpsCount.setPosition(textView.getCenter() - textView.getSize() / 2.f);

	Body planet0 = { 30.f, sf::Vector2f(-100.f, 0.f), sf::Vector2f(0.f, 0.f), 10.f };
	Body planet1 = { 50.f, sf::Vector2f(100.f, 0.f), sf::Vector2f(0.f, -0.f), 10.f };
	Body planet2 = { 100.f, sf::Vector2f(1200.f, 100.f), sf::Vector2f(0.f, -0.1f), 10.f };
	planet0.setColour(sf::Color::Cyan);
	std::vector<Body> planets = {planet0, planet1, planet2};
	/*planets[0] = planet0;
	planets[1] = planet1;*/

	std::vector<int> frameTimes(100, 10);
	int totalT = 100 * 10;
	time = steady_clock::now();

	while (window.isOpen()) {
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}
		//render
		window.clear();
        duration<int, std::milli> diff = duration_cast<std::chrono::milliseconds> (steady_clock::now() - time);
		int dt = diff.count();
		time = steady_clock::now();
		keyBindings(worldView, window, dt);
		window.setView(worldView);
		updatePlanets(planets);
		for (Body planet : planets) {
			window.draw(&planet.trail[0], planet.trail.size(), sf::LineStrip);
			window.draw(planet.shape);
		}
		fpsCountUpdate(frameTimes, dt, totalT);
		fpsCount.setString(std::to_string(100000/float(totalT)));
		window.setView(textView);
		//fpsCount.setPosition(0.f, 0.f);
		window.draw(fpsCount);
		window.display();
	}
}
