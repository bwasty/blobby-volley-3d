#ifndef BV3D_GAME
#define BV3D_GAME

#include <vrs/sharedobj.h>
#include <vrs/container/array.h>
#include <fmod.hpp>
#include "Constants.h"

namespace VRS {
	class JumpNavigation;
	class BackgroundGL;
	class GlutCanvas;
	class Camera;
	class SceneThing;
	class AmbientLight;
	class PointLight;
	class DistantLight;
	class BehaviorCallback;
	class TransparencyTechniqueGL;
	class InteractionMode;
	class InteractionConcept;
	class ImageCubeMapTextureGL;
	class LookAt;
	class Perspective;
	class TexGenGL;
	class Sphere;
	class VRSTime;
}

namespace FMOD {
	class System;
}

namespace BV3D
{
	class Arena;
	class Referee;
	class Ball;
	class Blobb;
	class SceneLoader;
	class Menu;
	class HUD;

	class Game : public VRS::SharedObj {
	public:
		Game();
		~Game();
		void update();			// render new frame
		void processInput();	// process and dispatch input
		VRS::SO<Ball> getBall() {return m_Ball;}
		VRS::SO<Referee> getReferee() {return m_Referee;}
		VRS::SO<Blobb> getBlobb(BV3D::TEAM team) {return m_BlobbArray->getElement(team==BV3D::TEAM1 ? 0 : 1);}
		VRS::SO<Arena> getArena() {return m_Arena;}
		void scheduleNewServe();
		void newServe();
		void aiServe(BV3D::TEAM team);


		void playSoundTouch();
		void playSoundWhistle();

		void switchToGame(bool bRestart);
		void toggleFullscreen();

		int getFrameCount() {return m_FrameCount;}
	protected:
		void switchToMenu(bool allowResume);
		void setupSound();
		void applyMenuSettings();
		void switchCameraposition(CAMERAPOSITION position);

	private:
		double			m_DelayedActionStart;
		bool			m_ScheduleNewServe;
		int				mPrevWidth, mPrevHeight, mPrevPosX, mPrevPosY;
		bool			mIsCameraDistant;
		bool			mUseMovieStyleCamera;
		CAMERAPOSITION	mCurrentCameraPosition;
		FMOD::System    *m_fmodSystem;
		FMOD::Sound     *soundTouch, *soundWhistle;

		VRS::SO<HUD>			mHud;

		VRS::SO<VRS::GlutCanvas>			m_Canvas;		// main canvas
		VRS::SO<VRS::SceneThing>			mScene;			// root node where Blobbs etc will be appended to
		VRS::SO<VRS::Array<VRS::SO<VRS::SceneThing> > >	mBlobbScenesArray;	// list of BlobbScenes
		VRS::SO<VRS::TransparencyTechniqueGL>	m_TransparencyTechnique;	// to enable transparency
		VRS::SO<VRS::Perspective>			m_perspective;
		VRS::SO<VRS::LookAt>				m_lookAt;
		VRS::SO<VRS::Camera>				m_Camera;			// scene camera
		VRS::SO<VRS::AmbientLight>			m_AmbientLight;
		VRS::SO<VRS::PointLight>			m_PointLight;
		VRS::SO<VRS::BehaviorCallback>		m_cbInput;		// callback to receive input from canvas
		VRS::SO<VRS::BehaviorCallback>		m_cbUpdate;		// callback to receive timer events for frame stepping
		VRS::SO<VRS::InteractionMode>		m_InteractionMode;	//needed for navigation technique
		VRS::SO<VRS::InteractionConcept>	m_InteractionConcept;	//needed for navigation technique
		VRS::SO<VRS::JumpNavigation>		m_Navigation;	// select and fly between different camera settings
		VRS::SO<VRS::SceneThing>			mBackground;		//SceneNode for background

		int			m_iFramerate;		// frame counter to allow frame rate checking
		int			m_FrameCount;
		double		m_dLastSecond;		// auxiliary variable for frame rate checking
		double		m_dLastUpdateTime;	// auxiliary variable for frame stepping
		double		m_FPS;				// desired frame rate
		
		VRS::SO<Arena>			m_Arena;	// Arena object
		VRS::SO<VRS::Array<VRS::SO<Blobb> > >	m_BlobbArray;	// list of Blobbs
		VRS::SO<Ball>			m_Ball;
		VRS::SO<Referee>		m_Referee;
		VRS::SO<SceneLoader>	m_SceneLoader;
		VRS::SO<Menu>			mMenu;

		
	};
}

#endif	// #ifndef BV3D_GAME
