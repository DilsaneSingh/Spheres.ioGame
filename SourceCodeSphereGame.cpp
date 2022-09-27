// Name - Dilsane Singh

// Assessment1.cpp: A program using the TL-Engine

#include <TL-Engine.h>	// TL-Engine include file and namespace
#include <sstream>

using namespace tle;

// Function that returns the length of the vector i.e. Distance between the two passed models.
float VectorLength(IModel* model1, IModel* model2) {

	float vectorX = model2->GetX() - model1->GetX();
	float vectorY = model2->GetY() - model1->GetY();
	float vectorZ = model2->GetZ() - model1->GetZ();

	float vectorXsqr = vectorX * vectorX;
	float vectorYsqr = vectorY * vectorY;
	float vectorZsqr = vectorZ * vectorZ;

	float vectorLength = sqrt(vectorXsqr + vectorYsqr + vectorZsqr);

	return vectorLength;
}

/* Function that generates random coordinates inside the island, taking care of the radius
of the model so that the model is completely inside the island. */
float RandomCoordinate(float radius) {
	float randomCoordinate = rand() % 200 + -100;
	if (randomCoordinate < 0) {
		randomCoordinate += radius;
	}
	else if (randomCoordinate > 0) {
		randomCoordinate -= radius;
	}

	return randomCoordinate;
}

void main()
{
	// Create a 3D engine (using TLX engine here) and open a window for it
	I3DEngine* myEngine = New3DEngine(kTLX);
	myEngine->StartWindowed();

	// Add default folder for meshes and other media
	myEngine->AddMediaFolder("C:\\ProgramData\\TL-Engine\\Media");
	myEngine->AddMediaFolder("C:\\Users\\dilsa\\Desktop\\CO1301 Assessment\\Assessment1Resources");

	/**** Set up your scene here ****/

	// Time as seed for random function to not behave in a pseudo random manner.
	srand(time(NULL));

	// Camera
	const float kCameraSpeed = 0.05f;
	ICamera* myCamera = myEngine->CreateCamera(kManual, 0.0f, 200.0f, 0.0f);
	myCamera->RotateX(90);

	// Camera States
	enum ECameraStates { TopDown, Isometric };
	ECameraStates CameraState = TopDown;

	// Water
	IMesh* waterMesh = myEngine->LoadMesh("water.x");
	IModel* water = waterMesh->CreateModel(0.0f, -5.0f, 0.0f);

	// Island
	IMesh* islandMesh = myEngine->LoadMesh("island.x");
	IModel* island = islandMesh->CreateModel(0.0f, -5.0f, 0.0f);

	// Skybox
	IMesh* skyMesh = myEngine->LoadMesh("sky.x");
	IModel* sky = skyMesh->CreateModel(0.0f, -960.0f, 0.0f);

	// Sphere
	const float kSphereSpeed = 0.05f;
	const float kRotationSpeed = 0.05f;
	IMesh* sphereMesh = myEngine->LoadMesh("sphereMesh.x");

	// Player
	float playerRadius = 10.0f;
	IModel* player = sphereMesh->CreateModel(0.0f, playerRadius, 0.0f);

	// Enemy
	float enemyRadius = 10.0f;
	float randomXEnemy = RandomCoordinate(enemyRadius);
	float randomZEnemy = RandomCoordinate(enemyRadius);
	IModel* enemy = sphereMesh->CreateModel(randomXEnemy, enemyRadius, randomZEnemy);
	enemy->SetSkin("enemysphere.jpg");

	while (VectorLength(enemy, player) < 50.0f) {
		randomXEnemy = RandomCoordinate(enemyRadius);
		randomZEnemy = RandomCoordinate(enemyRadius);
		enemy->SetPosition(randomXEnemy, enemyRadius, randomZEnemy);
	}

	// Cubes
	const int numCubes = 12;
	float cubeRadius = 2.5f;
	const int minDistanceBwCubes = 10.0f;
	IMesh* cubeMesh = myEngine->LoadMesh("miniCube.x");

	// HyperCube
	float randomXHyperCube = RandomCoordinate(cubeRadius);
	float randomZHyperCube = RandomCoordinate(cubeRadius);
	IModel* hyperCube = cubeMesh->CreateModel(randomXHyperCube, cubeRadius, randomZHyperCube);
	hyperCube->SetSkin("hypercube.jpg");
	while (VectorLength(player, hyperCube) <= playerRadius + cubeRadius || VectorLength(enemy, hyperCube) <= enemyRadius + cubeRadius || VectorLength(enemy, hyperCube) <= enemyRadius + cubeRadius || VectorLength(player, hyperCube) <= playerRadius + cubeRadius) {
		cubeMesh->RemoveModel(hyperCube);
		float randomXHyperCube = RandomCoordinate(cubeRadius);
		float randomZHyperCube = RandomCoordinate(cubeRadius);
		IModel* hyperCube = cubeMesh->CreateModel(randomXHyperCube, cubeRadius, randomZHyperCube);
		hyperCube->SetSkin("hypercube.jpg");
	}

	// Regular Cubes
	IModel* cube[numCubes];
	for (int i = 0; i < numCubes; i++) {
		float randomXCube = RandomCoordinate(cubeRadius);
		float randomZCube = RandomCoordinate(cubeRadius);
		cube[i] = cubeMesh->CreateModel(randomXCube, cubeRadius, randomZCube);
		while (VectorLength(player, cube[i]) <= playerRadius + cubeRadius || VectorLength(enemy, cube[i]) <= enemyRadius + cubeRadius || VectorLength(enemy, hyperCube) <= enemyRadius + cubeRadius || VectorLength(player, hyperCube) <= playerRadius + cubeRadius) {
			cubeMesh->RemoveModel(cube[i]);
			randomXCube = RandomCoordinate(cubeRadius);
			randomZCube = RandomCoordinate(cubeRadius);
			cube[i] = cubeMesh->CreateModel(randomXCube, cubeRadius, randomZCube);
		}
	}

	// Game States
	enum EGameStates { Playing, Paused, GameOver, GameWon };
	EGameStates GameState = Playing;

	// Text
	IFont* myFont = myEngine->LoadFont("Comic Sans MS", 36);
	stringstream gameStateText, playerPointsText, enemyPointsText;
	float windowWidth = myEngine->GetWidth();
	float windowHeight = myEngine->GetHeight();
	float textWidth = windowWidth - 40.0f;

	// Points
	const float scaleRate = 1.2f;
	int pointsPerCube = 10.0f;
	int scalePoints = 40;
	int gamePoints = 120;
	int playerPoints = 0.0f;
	int enemyPoints = 0.0f;
	bool playerFlag = false;
	bool enemyFlag = false;
	int playerPointPos = 10.0f;
	int enemyPointsPos = 35.0f;

	// Existance
	bool hyperCubeExists = true;
	bool playerExists = true;
	bool enemyExists = true;

	// Player States
	enum EPlayerStates { playerRegular, playerHyperState };
	EPlayerStates PlayerState = playerRegular;

	// Enemy States
	enum EEnemyStates { enemyRegular, enemyHyperState };
	EEnemyStates EnemyState = enemyRegular;

	// HyperState Variabbles
	float time, timepassed = 0;
	const float hyperStateTime = 5.0f;
	const float magneticRadius = 50.0f;
	bool changeSkin = true;

	// Collision State
	enum ECollisionStates { collision, noCollision };
	ECollisionStates CollisionState = noCollision;
	int nearestCube = 0;

	// The main game loop, repeat until engine is stopped
	while (myEngine->IsRunning())
	{
		// Draw the scene
		myEngine->DrawScene();

		/**** Update your scene each frame here ****/

		// Updating Points of Player Cube
		for (int i = 0; i < numCubes; i++) {
			if (VectorLength(cube[i], player) <= (playerRadius + cubeRadius) && GameState == Playing) {
				playerPoints += pointsPerCube;
				if (playerPoints % scalePoints == 0) {
					playerFlag = true;
				}
			}
		}

		// Updating Points of Enemy Cube
		for (int i = 0; i < numCubes; i++) {
			if (VectorLength(cube[i], enemy) <= (enemyRadius + cubeRadius) && GameState != GameOver) {
				enemyPoints += pointsPerCube;
				if (enemyPoints % scalePoints == 0) {
					enemyFlag = true;
				}
			}
		}

		// Scaling Player Cube
		if (playerFlag) {
			playerRadius *= scaleRate;
			player->Scale(scaleRate);
			player->SetY(playerRadius);
			playerFlag = false;
		}

		// Scaling Enemy Cube
		//if (enemyFlag) {
		//	enemyRadius *= scaleRate;
		//	enemy->Scale(scaleRate);
		//	enemy->SetY(enemyRadius);
		//	enemyFlag = false;
		//}

		// Player Won Condition
		if (playerPoints >= gamePoints) {
			GameState = GameWon;
		}

		// Player Loose Condition
		if (enemyPoints >= gamePoints) {
			GameState = GameOver;
		}

		// Score Board Points Positioning
		if (enemyPoints > playerPoints) {
			playerPointPos = 35.0f;
			enemyPointsPos = 10.0f;
		}
		else if (enemyPoints < playerPoints) {
			playerPointPos = 10.0f;
			enemyPointsPos = 35.0f;
		}

		// Score Board Text 
		playerPointsText << "Player Points : " << playerPoints;
		myFont->Draw(playerPointsText.str(), textWidth, playerPointPos, kBlack, kRight);
		playerPointsText.str("");

		enemyPointsText << "Enemy Points : " << enemyPoints;
		myFont->Draw(enemyPointsText.str(), textWidth, enemyPointsPos, kBlack, kRight);
		enemyPointsText.str("");

		// Cubes Respawn Checks
		for (int i = 0; i < numCubes; i++) {
			for (int j = 0; j < numCubes; j++) {
				if (i == j) {

				}
				else {
					if (VectorLength(cube[i], cube[j]) <= minDistanceBwCubes || VectorLength(cube[i], player) <= (playerRadius + cubeRadius) || VectorLength(cube[i], enemy) <= (enemyRadius + cubeRadius)) {
						cubeMesh->RemoveModel(cube[i]);
						float randomXCube = RandomCoordinate(cubeRadius);
						float randomZCube = RandomCoordinate(cubeRadius);
						cube[i] = cubeMesh->CreateModel(randomXCube, cubeRadius, randomZCube);
					}
					if (hyperCubeExists && VectorLength(cube[i], hyperCube) <= minDistanceBwCubes) {
						cubeMesh->RemoveModel(cube[i]);
						float randomXCube = RandomCoordinate(cubeRadius);
						float randomZCube = RandomCoordinate(cubeRadius);
						cube[i] = cubeMesh->CreateModel(randomXCube, cubeRadius, randomZCube);
					}
				}
			}
		}

		// Camera Key Hit
		if (myEngine->KeyHit(Key_1)) {
			CameraState = TopDown;
			myCamera->ResetOrientation();
			myCamera->SetPosition(0.0f, 200.0f, 0.0f);
			myCamera->RotateX(90);
		}

		else if (myEngine->KeyHit(Key_2)) {
			CameraState = Isometric;
			myCamera->ResetOrientation();
			myCamera->SetPosition(150, 150, -150);
			myCamera->RotateLocalY(-45);
			myCamera->RotateLocalX(45);
		}

		// Camera States

		if (CameraState == TopDown) {

			// Camera Movement

			if (myEngine->KeyHeld(Key_Up)) {
				myCamera->MoveZ(kCameraSpeed);
			}
			else if (myEngine->KeyHeld(Key_Down)) {
				myCamera->MoveZ(-kCameraSpeed);
			}

			if (myEngine->KeyHeld(Key_Right)) {
				myCamera->MoveX(kCameraSpeed);
			}
			else if (myEngine->KeyHeld(Key_Left)) {
				myCamera->MoveX(-kCameraSpeed);
			}
		}

		else if (CameraState == Isometric) {

		}

		// Quit Gamme
		if (myEngine->KeyHit(Key_Escape)) {
			myEngine->Stop();
		}

		// Pause Key Hit
		if (myEngine->KeyHit(Key_P)) {

			if (GameState == Playing) {
				GameState = Paused;
			}

			else if (GameState == Paused) {
				GameState = Playing;
			}

		}


		// Game States

		if (GameState == Playing) {

			// Sphere Movements
			if (myEngine->KeyHeld(Key_W)) {
				player->MoveLocalZ(kSphereSpeed);
			}
			else if (myEngine->KeyHeld(Key_S)) {
				player->MoveLocalZ(-kSphereSpeed);
			}

			if (myEngine->KeyHeld(Key_A)) {
				player->RotateY(-kRotationSpeed);
			}
			else if (myEngine->KeyHeld(Key_D)) {
				player->RotateY(kRotationSpeed);
			}

			// GameOver
			if (player->GetX() >= 100 || player->GetX() <= -100) {
				GameState = GameOver;
			}
			if (player->GetZ() >= 100 || player->GetZ() <= -100) {
				GameState = GameOver;
			}

		}
		else if (GameState == Paused) {
			gameStateText << "Paused" << endl << "Press P to Resume" << endl << "Press Esc to Quit Game";
			myFont->Draw(gameStateText.str(), windowWidth / 2, windowHeight / 2, kBlue, kCentre);
			gameStateText.str("");
		}
		else if (GameState == GameOver) {
			gameStateText << "You Loose, Game Over" << endl << "Press Esc to Quit Game";
			myFont->Draw(gameStateText.str(), windowWidth / 2, windowHeight / 2, kRed, kCentre);
			gameStateText.str("");
		}
		else if (GameState == GameWon) {
			gameStateText << "Congrats, You Win" << endl << "Press Esc to Quit Game";
			myFont->Draw(gameStateText.str(), windowWidth / 2, windowHeight / 2, kGreen, kCentre);
			gameStateText.str("");
		}


		// HyperCube Pick up
		if (hyperCubeExists) {
			if (VectorLength(player, hyperCube) <= (playerRadius + cubeRadius)) {
				cubeMesh->RemoveModel(hyperCube);
				hyperCubeExists = false;
				PlayerState = playerHyperState;
				myEngine->Timer();
			}
			else if (VectorLength(enemy, hyperCube) <= (enemyRadius + cubeRadius)) {
				cubeMesh->RemoveModel(hyperCube);
				hyperCubeExists = false;
				EnemyState = enemyHyperState;
				myEngine->Timer();
			}
		}

		// Player States
		if (PlayerState == playerHyperState) {
			time = myEngine->Timer();
			timepassed += time;
			if (timepassed <= hyperStateTime) {
				if (changeSkin) {
					player->SetSkin("hypersphere.jpg");
					changeSkin = false;
				}
				for (int i = 0; i < numCubes; i++) {
					if (VectorLength(cube[i], player) <= magneticRadius) {
						cube[i]->LookAt(player);
						cube[i]->MoveLocalZ(kSphereSpeed);
					}
				}
			}
			if (timepassed > hyperStateTime) {
				player->SetSkin("regularsphere.jpg");
				PlayerState = playerRegular;
			}
		}
		else if (PlayerState == playerRegular) {
		}

		// Enemy States
		if (EnemyState == enemyHyperState) {
			time = myEngine->Timer();
			timepassed += time;
			if (timepassed <= hyperStateTime) {
				if (changeSkin) {
					enemy->SetSkin("hypersphere.jpg");
					changeSkin = false;
				}
				for (int i = 0; i < numCubes; i++) {
					if (VectorLength(cube[i], enemy) <= magneticRadius) {
						cube[i]->LookAt(enemy);
						cube[i]->MoveLocalZ(kSphereSpeed);
					}
				}
			}
			if (timepassed > hyperStateTime) {
				enemy->SetSkin("enemysphere.jpg");
				EnemyState = enemyRegular;
			}
		}
		else if (EnemyState == enemyRegular) {
		}

		// Enemy Movements
		if (playerExists && enemyPoints - playerPoints > scalePoints) {
			enemy->LookAt(player);
		}
		else {
			for (int i = 0; i < numCubes; i++) {
				if (VectorLength(enemy, cube[i]) < VectorLength(enemy, cube[nearestCube])) {
					nearestCube = i;
				}
				enemy->LookAt(cube[nearestCube]);
				if (hyperCubeExists && (VectorLength(enemy, hyperCube) < VectorLength(enemy, cube[nearestCube]))) {
					enemy->LookAt(hyperCube);
				}
			}
		}

		if (VectorLength(player, enemy) < playerRadius + enemyRadius) {
			CollisionState = collision;
		}

		if (CollisionState == collision) {
			if (playerPoints - enemyPoints >= scalePoints) {
				enemyExists = false;
				player->Scale(scaleRate);
				enemy->SetY(-1000);
				playerPoints += scalePoints;
				CollisionState = noCollision;
			}
			else if (enemyPoints - playerPoints >= scalePoints) {
				enemyPoints += scalePoints;
				playerExists = false;
				enemy->Scale(scaleRate);
				GameState = GameOver;
				player->SetY(-2000);
				CollisionState = noCollision;
			}

			else {
				float bounceVectorX = (-(player->GetX() - enemy->GetX())) * 0.5;
				float bounceVectorZ = (-(player->GetZ() - enemy->GetZ())) * 0.5;
				player->Move(-bounceVectorX, 0, -bounceVectorZ);
				enemy->Move(bounceVectorX, 0, bounceVectorZ);
				CollisionState = noCollision;
			}

		}
		if (CollisionState == noCollision && GameState != Paused) {
			if (enemyExists == true) {
				enemy->MoveLocalZ(kSphereSpeed);
				enemy->SetY(enemyRadius);
			}
		}

	}

	// Delete the 3D engine now we are finished with it
	myEngine->Delete();
}
