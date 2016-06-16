#include "globals.h"

#include <iostream>

// GLUT keyboard callback handler
void myKeyboardCallback(unsigned char keyCode, int x, int y)
{
	// Get # seconds, then reset
	//float deltaTime = g_AniTimer.GetElapsedSeconds( true );

	//std::cout << "delta Time = " << deltaTime << std::endl;

	//const float speedPerSecond = 1.0f;
	//const float rotSpeedPerSecond = 2.0f;

	//float speed = speedPerSecond * deltaTime;
	//float rotSpeed = rotSpeedPerSecond * deltaTime;


	switch (keyCode)
	{	// WASD  QE    vs arrows

		// Change selected light
	case '+':
		::g_selectedLightIndex++;
		if ( ::g_selectedLightIndex >= NUMBEROFLIGHTS ) 
		{ ::g_selectedLightIndex = NUMBEROFLIGHTS; }
		break;
	case '-':
		::g_selectedLightIndex--;
		if ( ::g_selectedLightIndex <= 0 ) 
		{ ::g_selectedLightIndex = 0; }
		break;

	case '(':
		::g_bDebugLights = true;
		break;
	case ')':
		::g_bDebugLights = false;
		break;



	case 'l': case 'L':
		//glUniform4f( lightPosition, x, y, z);

		//::g_vecLights[0].position.x += 1.0f;

//		::g_vec_pGOs[0]->colour = glm::vec4( 1.0f, 0.0f, 0.0f, 1.0f );
		break;

	case 'o': case 'O':		// Note radians
//		::g_vec_pGOs[0]->preRotation.y += glm::radians(rotSpeed);
//		::g_vec_pGOs[0]->postRotation.y += glm::radians(rotSpeed);
		break;

	case 'w': case 'W':		// Up
//		::g_vec_pGOs[0]->position.y += speed;
		break;
	case 's': case 'S':
//		::g_vec_pGOs[0]->position.y -= speed;
		break;

	case 'a': case 'A':		// "Left"
//		::g_vec_pGOs[0]->position.x -= speed;
		break;
	
	case 'd': case 'D':		// "Right"
//		::g_vec_pGOs[0]->position.x += speed;
		break;

	case 'q': case 'Q':		// "Away???"
//		::g_vec_pGOs[0]->position.z -= speed;
		break;
	
	case 'e': case 'E':		// "Towards???"
//		::g_vec_pGOs[0]->position.z += speed;
		break;

	default:
		// Because it's just the right thing to do...
		break;
	}

	return;
}

void mySpecialKeyboardCallback(int key, int x, int y)
{
	float camSpeed = 0.1f;

	switch (key)
	{
	case GLUT_KEY_UP:		// "Away??"
		::g_cam_eye.z += camSpeed;  
		break;
	case GLUT_KEY_DOWN:
		::g_cam_eye.z -= camSpeed;	
		break;

	case GLUT_KEY_LEFT:		// "Away??"
		::g_cam_eye.x -= camSpeed;  
		break;
	case GLUT_KEY_RIGHT:
		::g_cam_eye.x += camSpeed;	
		break;

	case GLUT_KEY_PAGE_UP:
		::g_cam_eye.y += camSpeed;
		break;
	case GLUT_KEY_PAGE_DOWN:
		::g_cam_eye.y -= camSpeed;
		break;

	case GLUT_KEY_F1:
		std::cout << "You pressed F1" << std::endl;
		break;

	}

	return;
}

