/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"

cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();

/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	score = 0;
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();
	theAreaClicked = { 0, 0 };
	 loop = true;
	// Store the textures
	textureName = { "enemy", "bomb", "asteroid3", "asteroid4", "bullet","tank","theBackground"};
	texturesToUse = { "Images\\enemy.png", "Images\\alienbomb.png", "Images\\asteroid3.png", "Images\\asteroid4.png", "Images\\bullet.png", "Images\\tank.png", "Images\\starscape1024x768.png"};
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	//tempTextTexture = theTextureMgr->getTexture("tile1");
	//aRect = { 0, 0, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	aColour = { 228, 213, 238, 255 };
	btnNameList = { "exit_btn", "instructions_btn", "load_btn", "menu_btn", "play_btn", "save_btn", "settings_btn" };
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_instructions.png", "Images/Buttons/button_load.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png", "Images/Buttons/button_save.png", "Images/Buttons/button_settings.png" };
	btnPos = { { 400, 375 }, { 400, 300 }, { 400, 300 }, { 500, 500 }, { 400, 300 }, { 740, 500 }, { 400, 300 } };
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]);
	}
	for (int bCount = 0; bCount < btnNameList.size(); bCount++)
	{
		cButton * newBtn = new cButton();
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount]));
		newBtn->setSpritePos(btnPos[bCount]);
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight());
		theButtonMgr->add(btnNameList[bCount], newBtn);
	}
	theGameState = MENU;
	theBtnType = EXIT;
	// Create textures for Game Dialogue (text)
	fontList = { "digital", "spaceAge" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/space age.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	//gameTextList = { "spaceshooter" , "score "};

	//theTextureMgr->addTexture("Title", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, gameTextList[0], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));

	// Load game sounds
	soundList = { "theme", "shot", "explosion" };
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/who10Edit.wav", "Audio/shot007.wav", "Audio/explosion2.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	// Create text Textures
	gameTextNames = { "Title", "score", "TitleTxt", "CreateTxt", "DragDropTxt", "ThanksTxt", "SeeYouTxt" };
	gameTextList = { "spaceshooter", "score ", "spaceshooter", "shoot aliens", "move tank and shoot bullets", "Thanks for playing!", "See you again soon!" };
	for (int text = 0; text < gameTextNames.size(); text++)
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 228, 213, 238, 255 }, { 0, 0, 0, 0 }));
	}


	theSoundMgr->getSnd("theme")->play(-1);
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("theBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("theBackground")->getTWidth(), theTextureMgr->getTexture("theBackground")->getTHeight());

	theRocket.setSpritePos({ 500, 650 });
	theRocket.setTexture(theTextureMgr->getTexture("tank"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("tank")->getTWidth(), theTextureMgr->getTexture("tank")->getTHeight());
	theRocket.setRocketVelocity({ 0, 0 });

	// Create vector array of textures
	int spritew = theTextureMgr->getTexture("enemy")->getTWidth();
	int spriteh = theTextureMgr->getTexture("enemy")->getTHeight();
	int row = 0;
	int col = 0;
	for (int astro = 0; astro < 24; astro++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos({ 100 + ((spritew + spritew /2)*col ), 150 +((spriteh + spriteh/2) * row) });
		theAsteroids[astro]->setSpriteTranslation({ (rand() % 8 + 1), (rand() % 8 + 1) });
		//int randAsteroid = rand() % 4;
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[0]));
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[0])->getTWidth(), theTextureMgr->getTexture(textureName[0])->getTHeight());
		theAsteroids[astro]->setAsteroidVelocity({ 3.0f, 3.0f });
		theAsteroids[astro]->setActive(true);
		col++;
		if (astro== 11)
		{
			row++;
			col = 0;
		}
	}

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	SDL_RenderClear(theRenderer);
	switch (theGameState)
	{
	case MENU:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("CreateTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("DragDropTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		// Render Button
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 400, 375 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case PLAYING:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render each asteroid in the vector array
		for (int draw = 0; draw < theAsteroids.size(); draw++)
		{
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
		}
		cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		if (theTextureMgr->getTexture("score") == NULL)
		{
			theTextureMgr->addTexture("score", theFontMgr->getFont("digital")->createTextTexture(theRenderer, gameTextList[1], SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		}
		
		tempTextTexture = theTextureMgr->getTexture("score");
		pos = { 700, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };// Render each bullet in the vector array
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		for (int draw = 0; draw < theBullets.size(); draw++)
		{
			theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
		}
		for (int draw = 0; draw < thebombs.size(); draw++)
		{
			thebombs[draw]->render(theRenderer, &thebombs[draw]->getSpriteDimensions(), &thebombs[draw]->getSpritePos(), thebombs[draw]->getSpriteRotAngle(), &thebombs[draw]->getSpriteCentre(), thebombs[draw]->getSpriteScale());
		}
		// render the rocket
		theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
	}
	break;
	case END:
	{
		// render the buttons
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 500, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
	}
	break;
	case QUIT:
	{
		loop = false;
	}
	break;
	default:
		break;
	}
	
	SDL_RenderPresent(theRenderer);
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{
	// CHeck Button clicked and change state
	if (theGameState == MENU || theGameState == END)
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
		theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);
	}
	else
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);
	}
	if (theGameState == PLAYING)
	{

		// Update the visibility and position of each asteriod
		vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
		while (asteroidIterator != theAsteroids.end())
		{
			if ((*asteroidIterator)->isActive() == false)
			{
				asteroidIterator = theAsteroids.erase(asteroidIterator);
			}
			else
			{
				(*asteroidIterator)->update(deltaTime);
				++asteroidIterator;
			}
		}
		// Update the visibility and position of each bullet
		vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
		while (bulletIterartor != theBullets.end())
		{
			if ((*bulletIterartor)->isActive() == false)
			{
				bulletIterartor = theBullets.erase(bulletIterartor);
			}
			else
			{
				(*bulletIterartor)->update(deltaTime);
				++bulletIterartor;
			}
		} 
		// update the visibillity and position of each bomb
		vector<cBullet*>::iterator bombIterartor = thebombs.begin();
		while (bombIterartor != thebombs.end())
		{
			if ((*bombIterartor)->isActive() == false)
			{
				bombIterartor = thebombs.erase(bombIterartor);
			}
			else
			{
				(*bombIterartor)->update(deltaTime);
				++bombIterartor;
			}
		}
		/*
		==============================================================
		| Check for collisions
		==============================================================
		*/
		for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
		{
			//(*bulletIterartor)->update(deltaTime);
			for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
			{
				if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
				{
					// if a collision set the bullet and asteroid to false
					(*asteroidIterator)->setActive(false);
					(*bulletIterartor)->setActive(false);
					score += 100; // add to the score
					thescore = "score " + to_string(score);
					gameTextList[1] = thescore.c_str();
					theTextureMgr->deleteTexture("score");
					theSoundMgr->getSnd("explosion")->play(0); // play sound
					thebombs.push_back(new cBullet);
					int numbombs = thebombs.size() - 1; // spawning bombs
					thebombs[numbombs]->setSpritePos({ (*asteroidIterator)->getBoundingRect().x + (*asteroidIterator)->getSpriteCentre().x, (*asteroidIterator)->getBoundingRect().y + (*asteroidIterator)->getSpriteCentre().y });
					thebombs[numbombs]->setSpriteTranslation({ 0.0f, 2.0f });
					thebombs[numbombs]->setTexture(theTextureMgr->getTexture("bomb"));
					thebombs[numbombs]->setSpriteDimensions(theTextureMgr->getTexture("bomb")->getTWidth(), theTextureMgr->getTexture("bomb")->getTHeight());
					thebombs[numbombs]->setBulletVelocity({ 0.0f, 75.0f });
					thebombs[numbombs]->setSpriteRotAngle(180);
					thebombs[numbombs]->setActive(true);

				}
			}
		}
		for (vector<cBullet*>::iterator bombIterator = thebombs.begin();bombIterator != thebombs.end(); ++bombIterator)
		{
			if ((*bombIterator)->collidedWith(&(*bombIterator)->getBoundingRect(), &theRocket.getBoundingRect()))
			{
				// if a collision set the bullet and asteroid to false
				(*bombIterator)->setActive(false);
				theGameState = END;
			}
		}
		if (theAsteroids.empty())
		{
			theGameState = END;
			// Create vector array of textures
			int spritew = theTextureMgr->getTexture("enemy")->getTWidth();
			int spriteh = theTextureMgr->getTexture("enemy")->getTHeight();

			int row = 0;
			int col = 0;
			for (int astro = 0; astro < 24; astro++)
			{
				theAsteroids.push_back(new cAsteroid);
				theAsteroids[astro]->setSpritePos({ 100 + ((spritew + spritew / 2)*col), 150 + ((spriteh + spriteh / 2) * row) });
				theAsteroids[astro]->setSpriteTranslation({ (rand() % 8 + 1), (rand() % 8 + 1) });
				//int randAsteroid = rand() % 4;
				theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[0]));
				theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[0])->getTWidth(), theTextureMgr->getTexture(textureName[0])->getTHeight());
				theAsteroids[astro]->setAsteroidVelocity({ 3.0f, 3.0f });
				theAsteroids[astro]->setActive(true);
				col++;
				if (astro == 11)
				{
					row++;
					col = 0;
				}
			}

		}
		// Update the Rockets position
		theRocket.update(deltaTime);
	}
	theAreaClicked = { 0, 0 };

}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y };
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				
				case SDLK_RIGHT:
				{
					// right movement
					if (theRocket.getSpritePos().x < (renderWidth - (theRocket.getSpritePos().w + 40)))
					{
						theRocket.setRocketVelocity({ 160.0, 0.0 });
					}
				}
				break;

				case SDLK_LEFT:
				{ // left movement
					if (theRocket.getSpritePos().x > 0)
					{
						theRocket.setRocketVelocity({ -100.0, 0.0 });
					}
				}
				break;
				case SDLK_SPACE:
				{
					// shoot bullets
					theBullets.push_back(new cBullet);
					int numBullets = theBullets.size() - 1;
					theBullets[numBullets]->setSpritePos({ theRocket.getBoundingRect().x + theRocket.getSpriteCentre().x, theRocket.getBoundingRect().y + theRocket.getSpriteCentre().y });
					theBullets[numBullets]->setSpriteTranslation({ 2.0f, 2.0f });
					theBullets[numBullets]->setTexture(theTextureMgr->getTexture("bullet"));
					theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("bullet")->getTWidth(), theTextureMgr->getTexture("bullet")->getTHeight());
					theBullets[numBullets]->setBulletVelocity({ 2.0f, 2.0f });
					theBullets[numBullets]->setSpriteRotAngle(theRocket.getSpriteRotAngle());
					theBullets[numBullets]->setActive(true);
					cout << "Bullet added to Vector at position - x: " << theRocket.getBoundingRect().x << " y: " << theRocket.getBoundingRect().y << endl;
					theSoundMgr->getSnd("shot")->play(0);
				}
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}

