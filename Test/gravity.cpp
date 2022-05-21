#include <iostream>
#include "SFML/Graphics.hpp"
#include <time.h>
#include <vector>
#include<chrono>
#include <deque>
#define forn(i, s, n) for(int i=s; i<n; i++)
#define vdebug(v) cout << #v << " = "; for(auto x: v){cout << x << ' ';}; cout << '\n';
#define debug(x) cout << #x << " = " << x << '\n';

using std::cout;

class Body
{
public:
	sf::Vector2f pos, vel, accel; // position, velocity, acceleration all 0
	float mass;
	sf::CircleShape shape;
	clock_t t;
	Body(float m, sf::Vector2f Pos, float radius) {
		t = clock();
		mass = m;
		pos = Pos;	
		shape.setRadius(radius);
		shape.setOrigin(radius, radius);
	}			   
	Body(float m, sf::Vector2f Pos, sf::Vector2f Vel, float radius) {
		t = clock();
		mass = m;
		pos = Pos; vel = Vel;
		shape.setRadius(radius);
		shape.setOrigin(radius, radius);
	}
	void updatePos() {
		clock_t dt = (clock() - t); // in milliseconds
		t = clock();
		pos = pos + vel * float(dt) + accel * float(dt * dt) * 0.5f;
		vel = vel + accel * float(dt);
		shape.setPosition(pos);

	}
	void gravityAccel(std::vector<Body> planets) {
		accel -= accel;
		for (Body planet : planets) {
			if (planet.pos == pos) {
				continue;
			}
			sf::Vector2f diff = planet.pos - pos;
			float rSquared = diff.y * diff.y + diff.x * diff.x;
			sf::Vector2f direction = diff / sqrt(rSquared);
			accel += direction * 6.67e-11f * planet.mass / (rSquared + 10);
		}
	}
	void setColour(sf::Color colour) {
		shape.setFillColor(colour);
	}
};

void gravity(std::vector<Body>& planets) {
	for (Body& planet : planets) {
		planet.gravityAccel(planets);
		//debug(planet.accel.x); debug(planet.accel.y);
	}
}

void updatePlanets(std::vector<Body>& planets) {
	//time_t t = clock();
	for (Body& planet : planets) {
		//debug(planet.pos.x); debug(planet.pos.y);
		planet.updatePos();
		//debug(planet.pos.x); debug(planet.pos.y);
	}
	gravity(planets);
	//debug(clock() - t);
}

void keyBindings(sf::View& view, sf::RenderWindow& window, clock_t dt) {
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
		view.zoom(1.f-float(dt)*zoomFactor);
	}
	if (Keyboard::isKeyPressed(Keyboard::Hyphen)) {
		view.zoom(1.f+float(dt)*zoomFactor);
	}

	window.setView(view);
}

void fpsCountUpdate(std::vector<clock_t>& times, clock_t dt, clock_t& total) {
	total -= times.back();
	total += dt;
	times.pop_back();
	times.insert(times.begin(), dt);
}



int main() {
	const short int WIDTH = 1000, HEIGHT = 600;
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Hi!");
	sf::View worldView;
	worldView.setCenter(500.f, 300.f);
	worldView.setSize(WIDTH, HEIGHT);
	window.setView(worldView);
	//window.setFramerateLimit(60);
	sf::Event e;
	clock_t time = clock();

	sf::Font font;
	if (!font.loadFromFile("FiraCode-Regular.ttf")) {
		cout << "Could not load font" << std::endl;
	}
	sf::Text fpsCount;
	fpsCount.setFont(font);
	fpsCount.setString("hey");

	Body sun(100000000000.f, sf::Vector2f(500.f, 300.f), sf::Vector2f(0.f, 0.0f), 20.f);
	Body earth(7000000000.f, sf::Vector2f(400.f, 300.f), sf::Vector2f(-0.0f, -0.2f), 5.f);
	Body moon(1000.f, sf::Vector2f(300.f, 350.f), sf::Vector2f(0.f, 0.2f), 5.f);
	Body jupiter(1e10f, sf::Vector2f(300.f, 300.f), sf::Vector2f(0.f, 0.2f), 5.f);

	sun.setColour(sf::Color(200, 100, 0));
	earth.setColour(sf::Color(0, 0, 255));
	jupiter.setColour(sf::Color(200, 0, 100));

	std::vector<Body> planets = {sun, earth, moon, jupiter};

	std::vector<clock_t> frameTimes(100, 10);
	clock_t totalT = 100 * 10;

	while (window.isOpen()) {
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}
		}
		//render
		window.clear();
		for (Body planet : planets) {
			window.draw(planet.shape);
			//debug(planet.pos.x); debug(planet.pos.y);
		}
		updatePlanets(planets);
		clock_t dt = clock() - time;
		time = clock();
		keyBindings(worldView, window, dt);
		fpsCountUpdate(frameTimes, dt, totalT);
		fpsCount.setString(std::to_string(100000/(totalT+1)));
		fpsCount.setPosition(window.getView().getCenter() - window.getView().getSize()/2.f);
		window.draw(fpsCount);
		window.display();
	}

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to worldView errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
