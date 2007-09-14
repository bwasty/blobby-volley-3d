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
	class AI;

	/**
	* \brief The Game class represents an instance of the BlobbyVolley3D game.

	* It is responsible for:
	* - creating and managing:
	*   - the obvious game objects: the Arena, the Blobbs, the Ball and the Net
	*   - the canvas(window) to which the scene is rendered
	*   - the root scene node
	*   - the scene camera
	* - hooking in/out the sub-scenes of the game objects
	* - processing and dispatching of input events from the canvas
	* - periodically issue rendering
	*/
	class Game : public VRS::SharedObj {
	public:
		Game();
		~Game();
		void update();			// render new frame
		void processInput();	// process and dispatch input
		VRS::SO<BV3D::Ball> getBall()			{return mBall;}
		VRS::SO<BV3D::Referee> getReferee()		{return mReferee;}
		VRS::SO<BV3D::Blobb> getBlobb(BV3D::TEAM team) {return mBlobbArray->getElement(team==BV3D::TEAM1 ? 0 : 1);}
		VRS::SO<BV3D::Arena> getArena()			{return mArena;}
		void scheduleNewServe();
		void newServe();
		void playSoundTouch();
		void playSoundWhistle();
		void switchToGame(bool bRestart);
		void toggleFullscreen();
		int getFrameCount() {return mFrameCount;}

	protected:
		void switchToMenu(bool allowResume);
		void setupSound();
		void applyMenuSettings();
		void switchCameraposition(BV3D::CAMERAPOSITION position);
		VRS::Vector getPositionVector(BV3D::CAMERAPOSITION position);
		VRS::Vector getDirectionVector(BV3D::CAMERAPOSITION position);
		bool isCameraMovieStyle()				{return mUseMovieStyleCamera;}
		bool isCameraDistant()					{return mIsCameraDistant;}
		bool isCameraHigher()					{return mIsCameraHigher;}
		void setMovieStyleCamera(bool useMovieStyle) {mUseMovieStyleCamera = useMovieStyle;}
		void setCameraDistant(bool distant)		{mIsCameraDistant = distant;}
		void setCameraHigher(bool higher)		{mIsCameraHigher = higher;}
		bool isPaused()							{return mIsPaused;}
		void setPaused(bool pauseGame)			{mIsPaused = pauseGame;}

	private:
		double								mDelayedActionStart;
		bool								mScheduleNewServe;
		int									mPrevWidth, mPrevHeight, mPrevPosX, mPrevPosY;
		bool								mIsCameraDistant;
		bool								mIsCameraHigher;
		bool								mUseMovieStyleCamera;
		bool								mIsPaused;
		bool								mIsCameraAnimating;
		int									mFrameCount;		// total frame count
		double								mLastUpdateTime;	// auxiliary variable for frame stepping
		BV3D::CAMERAPOSITION				mCurrentCameraPosition;
		FMOD::System						*mFmodSystem;
		FMOD::Sound							*soundTouch, *soundWhistle;

		VRS::SO<BV3D::HUD>					mHud;
		VRS::SO<BV3D::Referee>				mReferee;
		VRS::SO<BV3D::Arena>				mArena;					// Arena object
		VRS::SO<VRS::Array<VRS::SO<BV3D::Blobb> > >	mBlobbArray;	// list of Blobbs
		VRS::SO<BV3D::Ball>					mBall;
		VRS::SO<BV3D::SceneLoader>			mSceneLoader;
		VRS::SO<BV3D::Menu>					mMenu;
		VRS::SO<BV3D::AI>					mAI;

		VRS::SO<VRS::GlutCanvas>			mCanvas;		// main canvas
		VRS::SO<VRS::SceneThing>			mScene;			// root node where Blobbs etc will be appended to
		VRS::SO<VRS::TransparencyTechniqueGL>	mTransparencyTechnique;	// to enable transparency
		VRS::SO<VRS::Perspective>			mPerspective;
		VRS::SO<VRS::LookAt>				mLookAt;
		VRS::SO<VRS::Camera>				mCamera;			// scene camera
		VRS::SO<VRS::AmbientLight>			mAmbientLight;
		VRS::SO<VRS::PointLight>			mPointLight;
		VRS::SO<VRS::BehaviorCallback>		mCbInput;		// callback to receive input from canvas
		VRS::SO<VRS::BehaviorCallback>		mCbUpdate;		// callback to receive timer events for frame stepping
		VRS::SO<VRS::JumpNavigation>		mNavigation;	// select and fly between different camera settings
		VRS::SO<VRS::SceneThing>			mBackground;		//SceneNode for background


	};
}

#endif	// #ifndef BV3D_GAME
