// Include important C++ libraries here
#include <SFML/Graphics.hpp>

#include <sstream>

#include <SFML/Audio.hpp>

// Make code easier to type with "using namespace"
using namespace sf;
// Function declaration

int i = 5;
bool acceptInput = true;

// Track whether the game is running
bool paused = true;

// Defineixo les posicions que pot tenir un objecte a la pantalla
enum class side {
	LEFT,
	RIGHT,
	NONE
};

const int MaxWidth = 1920;// Amplada maxima de la pantalla
const int MaxHeight = 1080;// Altura maxima de la pantalla
const int PosTimeBar = 980;// Posicio de la barra de temps a la pantalla
const int NUM_BRANCHES = 6;// Nombre de branques de l'arbre

// Variables per a controlar el log.
bool logActive = false;
float logSpeedX = 1000;
float logSpeedY = -1500;
int logSpeed = (rand() % 100000) + 2500;

// Variable per el comptador de frames
int frameCounter = 0;

// Cloud data
struct Cloud {
	bool active;
	sf::Sprite sprite;
	int speed;
};

sf::SoundBuffer buffer;
sf::Sound sound;

void playSound(const std::string& filename) {// Funcio per reproduir musica i effectes del joc.
	if (!buffer.loadFromFile(filename)) {// Intenta carregar el fitxer de so al buffer per fer-lo servir en el joc.
	}
	sound.setBuffer(buffer);
	sound.play();// Reprodueix l'arxiu de musica o l'effecte de so.
}

int main() {
	Sprite branches[NUM_BRANCHES];
	side branchPositions[NUM_BRANCHES];

	srand((int)time(0) * 10);

	// Create a video mode object
	VideoMode vm(1920, 1080);

	// Create and open a window for the game
	RenderWindow window(vm, "GOH!!!"); // , Style::Fullscreen);

	// Create a texture to hold a graphic on the GPU
	Texture textureBackground;

	// Load a graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");

	// Create a sprite
	Sprite spriteBackground;

	// Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	// Set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	// Make a tree sprite
	Texture textureTree;
	//Li assigna a l'abre una textura utilitzant una imatge.
	textureTree.loadFromFile("./graphics/tree.png");
	// Crea els tres sprites per els arbres.
	Sprite spriteTree1;
	Sprite spriteTree2;
	Sprite spriteTree3;
	// Assigna la textura als tres sprites.
	spriteTree1.setTexture(textureTree);
	spriteTree2.setTexture(textureTree);
	spriteTree3.setTexture(textureTree);
	// Posa els sprites en varies posicions de la pantalla.
	spriteTree1.setPosition(810, 0);
	spriteTree2.setPosition(50, -100);
	spriteTree3.setPosition(1730, 0);

	// Prepare the bee
	Texture textureBee;
	//Li assigna a l'abella una textura utilitzant una imatge.
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	// Posa al abella a la part baixa de la pantalla.
	spriteBee.setPosition(0, 800);
	// Is the bee currently moving?
	bool beeActive = false;
	// How fast can the bee fly
	float beeSpeed = 0.0f;

	// make 3 cloud sprites from 1 texture
	Texture textureCloud;
	// Load 1 new texture
	textureCloud.loadFromFile("graphics/cloud.png");

	// Cloud data array
	const int numClouds = 3;// Aqui indiquem a el nombre de nuvols.
	const int distance = 150;// Triem la distancia entre nuvols.
	Cloud spriteCloud[numClouds];
	for (int i = 0; i < numClouds; ++i) {
		spriteCloud[i].active = false;
		spriteCloud[i].sprite.setTexture(textureCloud);
		spriteCloud[i].sprite.setPosition(0, 0 * distance);// Assignacio de la posicio del nuvol la multiplica per la distancia per a separar els nuvols.
		spriteCloud[i].speed = 0.0f;// Posem la velocitat del nuvol.
	}


	// Variables to control time itself
	Clock clock;

	// Draw some text
	int score = 0;
	Text messageText;
	Text scoreText;
	// We need to choose a font
	Font font;
	font.loadFromFile("fonts/Halo.ttf");

	// Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);

	// Assign the actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score = 0");

	// Make it really big
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	// Choose a color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color::White);


	// Position the text
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left +
		textRect.width / 2.0f,
		textRect.top +
		textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);


	// Time bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((MaxWidth / 2) - timeBarStartWidth / 2, PosTimeBar);
	Time gameTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;
	// Track whether the game is running
	bool paused = true;

	sf::RectangleShape rectangleBackground;// Creo un rectangle.

	rectangleBackground.setSize(sf::Vector2f(850, 110)); // Estableix la midadel rectangle.
	rectangleBackground.setPosition(20, 20); // Estableixo la posicio del rectangle a la pantalla.
	rectangleBackground.setFillColor(sf::Color(5, 5, 5, 200)); // Estableixo el color a un negre transparent. 

	// Prepare 5 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	// Set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);
		// Set the sprite's origin to dead centre
		// We can then spin it round without changing its position
		branches[i].setOrigin(220, 20);
	}


	// Move all the branches down one place
	for (int seed = 1; seed <= 5; seed++) {
		// Spawn a new branch at position 0
		// LEFT, RIGHT or NONE
		srand((int)time(0) + seed);
		int r = (rand() % 5);
		switch (r) {
		case 0:
			branchPositions[0] = side::LEFT;
			break;
		case 1:
			branchPositions[0] = side::RIGHT;
			break;
		default:
			branchPositions[0] = side::NONE;
			break;
		}
	}

	// Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(1200, 720);

	// The player starts on the left
	side playerSide = side::LEFT;

	// Prepare the gravestone
	Texture textureRip;
	textureRip.loadFromFile("graphics/rip.png");
	Sprite spriteRip;
	spriteRip.setTexture(textureRip);
	spriteRip.setPosition(7000, 830);

	// Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(7000, 830);

	// Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700;
	const float AXE_POSITION_RIGHT = 1075;

	// Prepare the flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(7000, 720);

	// Prepare the sound
	playSound("sound/unawakening_float.wav");

	//game loop
	while (window.isOpen()) {

		/*
		****************************************
		Handle the players input
		****************************************
		*/

		sf::Event event;// Creo un esdeveniment.
		while (window.pollEvent(event)) {// Comprovar si hi han esdeveniments.
			if (event.type == sf::Event::Closed)// Comprovar el tipus de esdeveniment que es.
				window.close();// Si l'esdeveniment es de tancar tenquem el joc.
			if (event.type == Event::KeyReleased && !paused) {// Si el joc no esta pausat.
				// Listen for key presses again
				acceptInput = true;
				// hide the axe
				spriteAxe.setPosition(2000,
					spriteAxe.getPosition().y);
			}
		}

		// Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;
			// Reset the time and the score
			score = 0;
			timeRemaining = 6;
		}

		if (Keyboard::isKeyPressed(Keyboard::Escape)) {// Comprova si la tecla esc s'ha premut.
			window.close();// Si es el cas tencar el joc.
		}

		// First handle pressing the right cursor key
		if (acceptInput) {
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				playSound("sound/chop.wav");
				// Make sure the player is on the right
				playerSide = side::RIGHT;

				// Add to the amount of time remaining
				timeRemaining += (0.1) + .10;
				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 720);

				// update the branches
				for (int j = NUM_BRANCHES - 1; j > 0; j--) {
					branchPositions[j] = branchPositions[j - 1];
				}

				// Spawn a new branch at position 0
				// LEFT, RIGHT or NONE
				int r = (rand() % 5);
				switch (r) {
				case 0:
					branchPositions[0] = side::LEFT;
					break;
				case 1:
					branchPositions[0] = side::RIGHT;
					break;
				default:
					branchPositions[0] = side::NONE;
					break;
				}

				// Update the score text
				score++;
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());
				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;

			}

			// First handle pressing the left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				playSound("sound/chop.wav");
				// Make sure the player is on the Left
				playerSide = side::LEFT;

				// Add to the amount of time remaining
				timeRemaining += (0.1) + .10;
				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(580, 720);
				// Update the branches
				for (int j = NUM_BRANCHES - 1; j > 0; j--) {
					branchPositions[j] = branchPositions[j - 1];
				} // Spawn a new branch at position 0
				// LEFT, RIGHT or NONE
				int r = (rand() % 5);
				switch (r) {
				case 0:
					branchPositions[0] = side::RIGHT;
					break;
				case 1:
					branchPositions[0] = side::LEFT;
					break;
				default:
					branchPositions[0] = side::NONE;
					break;
				}
				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;
				acceptInput = false;

				// Update the score text
				score++;
				std::stringstream ss;
				ss << "Score = " << score;
				scoreText.setString(ss.str());
			}

		}

		/*
		****************************************
		Update the scene
		****************************************
		*
		*
		*
		*
		*
		*/
		if (!paused) {
			// Measure time
			Time dt = clock.restart();
			frameCounter++;

			// Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// size up the time bar
			timeBar.setSize(Vector2f(timeBarWidthPerSecond *
				timeRemaining, timeBarHeight));

			if (timeRemaining <= 0.0f) {
				// Pause the game
				paused = true;
				// Change the message shown to the player
				messageText.setString("Out of time!!");
				//Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);
				messageText.setPosition(MaxWidth / 2.0f, MaxHeight / 2.0f);
				window.draw(spriteRip);
				spriteRip.setPosition(1200, 720);
				playSound("sound/out_of_time.ogg");
			}

			// Setup the bee

			if (!beeActive) {// Comprova si l'abella no esta activa.
				beeSpeed = (rand() % 200) + 200;// Genera una velocitat aleatoria per a l'abella.

				float height = (rand() % 500) + 500;// Genera una altura aleatoria per a l'abella.
				spriteBee.setPosition(2000, height);
				beeActive = true;// Activa l'abella.

			}
			else // Move the bee
			{
				spriteBee.setPosition(// Preparem el moviment de la abella.
					spriteBee.getPosition().x -// Posem la abella a el costat esquerra.
					(beeSpeed * dt.asSeconds()),// La velocitat de l'abella es multiplica pel temps.
					spriteBee.getPosition().y);// La posicio "y" es mante igual.
				// Has the bee reached the right hand edge of the screen?
				if (spriteBee.getPosition().x < -100) {// Comprovem si l'abella ha sortit de la pantalla per la dreta.
					beeActive = false;// Desactivem l'abella.
				}
			}
			for (int i = 0; i < numClouds; ++i) {// Posem un bucle que recorre cada nuvol en l'array.
				if (!spriteCloud[i].active) {// Comprova si el nuvol actual esta actiu.
					spriteCloud[i].speed = rand() % 200;// Si el nuvol no esta actiu assigna una velocitat aleatoria al nuvol.
					float height = rand() % (distance * (i + 1));// Genera una altura aleatoria per al nuvol.
					spriteCloud[i].sprite.setPosition(-200, height);// Posiciona el nuvol en una posicio inicial.
					spriteCloud[i].active = true;// Activa el nuvol.

				}
				else {
					spriteCloud[i].sprite.setPosition(// Aquesta part serveix per calcular la nova posicio del nuvol basant-se en la seva velocitat i el temps que ha pasat
						spriteCloud[i].sprite.getPosition().x + (spriteCloud[i].speed * dt.asSeconds()),
						spriteCloud[i].sprite.getPosition().y);
					if (spriteCloud[i].sprite.getPosition().x > MaxWidth) {// Si el nuvol ha sortit de la pantalla el desactivem.
						spriteCloud[i].active = false;

					}

				}
			}

			if (logActive) {// Si el log esta actiu.
				spriteLog.setPosition(spriteLog.getPosition().x - (logSpeed * dt.asSeconds()), spriteLog.getPosition().y);// Actualitzem la posicio del log.

				// Has the bee reached the right hand edge of the screen?
				if (spriteLog.getPosition().x < -100) {// Comprovem si el log ha arribat a l'extrem dret de la pantalla.
					logActive = false;// Si esta actiu el desactivem.
				}
			}

			// update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++) {
				float height = i * 150;
				if (branchPositions[i] == side::LEFT) {
					// Move the sprite to the left side
					branches[i].setPosition(610, height);
					// Flip the sprite round the other way
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT) {
					// Move the sprite to the right side
					branches[i].setPosition(1330, height);
					// Set the sprite rotation to normal
					branches[i].setRotation(0);
				}
				else {
					// Hide the branch
					branches[i].setPosition(3000, height);
				}
			}


			// has the player been squished by a branch?

			if (branchPositions[5] == playerSide)// Aqui comprovem si el jugador ha estat esclafat.
			{
				paused = true;// Pausem el joc.
				messageText.setString("SQUISHED!!"); // Mostrem el missatge "SQUISHED!!".
				FloatRect textRect = messageText.getLocalBounds();// Especifiquem les dimensions del missatge.
				messageText.setOrigin(textRect.left + textRect.width / 2.0f,// Posicionem el missatge al mig de la pantalla.
					textRect.top + textRect.height / 2.0f);
				messageText.setPosition(MaxWidth / 2.0f, MaxHeight / 2.0f);

				spriteRip.setPosition(spritePlayer.getPosition().x, spritePlayer.getPosition().y);// Posicionem la tomba a la posicio actual del jugador.
				spritePlayer.setPosition(2000, spritePlayer.getPosition().y);// Moguem l'sprite del jugador fora de la pantalla.
				spriteRip.setPosition(spritePlayer.getPosition().x, spritePlayer.getPosition().y);
				acceptInput = false;// Desactivem l'input del jugador.
				playSound("sound/death.ogg");// Reprodueim la musica del Game Over.
				if (playerSide == side::LEFT)// Si el jugador esta a l'esquerra.
				{
					spriteRip.setPosition(590, 780);// Posicionem la tomba a les coordenades del jugador, el qual esta a la dreta.
				}
				else
				{
					spriteRip.setPosition(1200, 780);// Posicionem la tomba a les coordenades del jugador, el qual esta a la esquerra.

				}


			}


		}

		// End if(!paused)

		/*
		****************************************
		Draw the scene
		****************************************
		*/

		// Clear everything from the last frame
		window.clear();

		// Draw our game scene here
		window.draw(spriteBackground);

		// Draw the timebar
		window.draw(timeBar);

		// Draw the branches

		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		// Draw the tree
		window.draw(spriteTree1);
		window.draw(spriteTree2);
		window.draw(spriteTree3);

		// Draw the player
		window.draw(spritePlayer);

		// Draw the axe
		window.draw(spriteAxe);

		// Draw the flying log
		window.draw(spriteLog);

		// Draw the gravestone
		window.draw(spriteRip);

		// Now draw the insect
		window.draw(spriteBee);

		// Draw clouds
		for (int i = 0; i < numClouds; ++i) {
			window.draw(spriteCloud[i].sprite);

		}
		// Draw the score
		window.draw(rectangleBackground);
		window.draw(scoreText);

		if (paused) {
			// Draw our message
			window.draw(messageText);
		}

		// Show everything we just drew
		window.display();

	}

	return 0;
}
