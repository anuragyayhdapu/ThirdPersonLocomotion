#include "Game/Game.hpp"
#include "Game/AttractMode.hpp"
#include "Game/App.hpp"

#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/AABB2.hpp"


App* g_theApp = nullptr;			// Created and owned by Main_Windows.cpp
Renderer* g_theRenderer = nullptr;	// Created and owned by the App
InputSystem* g_theInput = nullptr;	// used by game code for input queries. App class should own (create, manage, destroy) a single instance of the InputSystem for your game
Window* g_theWindow = nullptr;

App::App()
{
}

App::~App()
{
}

void App::Startup()
{
	// create the event system
	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);

	// create input
	InputSystemConfig inputSystemConfig;
	g_theInput = new InputSystem(inputSystemConfig);

	// create window
	WindowConfig windowConfig;
	windowConfig.m_windowTitle = "Third Person Locomotion";
	windowConfig.m_clientAspect = 2.0f;
	g_theWindow = new Window(windowConfig);

	// create renderer
	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	// create dev console
	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_renderer = g_theRenderer;
	g_theDevConsole = new DevConsole(devConsoleConfig);

	g_theEventSystem->Startup();
	g_theDevConsole->Startup();
	AddGameKeyText();
	g_theInput->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();

	// create and startup debug renderer
	DebugRenderConfig debugRendererConfig;
	debugRendererConfig.m_renderer = g_theRenderer;
	DebugRenderSystemStartup(debugRendererConfig);

	m_theAttractMode = new AttractMode();
	m_theAttractMode->Startup();

	LoadFonts();
	LoadTextures();

	// subscribe to quit event
	g_theEventSystem->SubscribeToEvent(QUIT_COMMAND, App::EventHandler_CloseWindow);
}

void App::Run()
{
	// Program main loop; keep running frames until it's time to quit
	while (!IsQuitting())
	{
		RunFrame();
	}
}

void App::Shutdown()
{
	// un-subscribe from quit event
	g_theEventSystem->UnsubscribeFromEvent(QUIT_COMMAND, App::EventHandler_CloseWindow);

	m_isQuitting = false;

	DebugRenderSystemShutdown();
	g_theRenderer->Shutdown();
	g_theWindow->Shutdown();
	g_theInput->Shutdown();
	g_theEventSystem->Shutdown();
	g_theDevConsole->Shutdown();

	if (m_theGame != nullptr)
	{
		m_theGame->Shutdown();
	}
	delete m_theGame;			m_theGame = nullptr;
	delete m_theAttractMode;	m_theAttractMode = nullptr;
	delete g_theRenderer;		g_theRenderer = nullptr;
	delete g_theWindow;			g_theWindow = nullptr;
	delete g_theInput;			g_theInput = nullptr;
	delete g_theEventSystem;	g_theEventSystem = nullptr;
	delete g_theDevConsole;		g_theDevConsole = nullptr;
}

void App::RunFrame()
{
	BeginFrame();
	Update();
	Render();
	EndFrame();
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;

	return m_isQuitting;
}

void App::BeginFrame()
{
	Clock::TickSystemClock();

	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theDevConsole->BeginFrame();
	DebugRenderBeginFrame();

	if (m_gameState == PLAY_MODE && m_theGame != nullptr) 
		m_theGame->BeginFrame();
}

void App::Update()
{
	// update cursor state
	UpdateCursorState();

	XboxController const& controller = g_theInput->GetController(0);

	if (g_theInput->WasKeyJustPressed(KEYCODE_ESCAPE))
	{
		// go to attract mode if in play mode
		if (m_gameState == PLAY_MODE)
		{
			m_theGame->Shutdown();
			delete m_theGame;
			m_theGame = nullptr;

			m_theAttractMode = new AttractMode();
			m_theAttractMode->Startup();
			m_gameState = ATTRACT_MODE;

			return;
		}

		// quit app if in attract mode
		if (m_gameState == ATTRACT_MODE)
		{
			delete m_theAttractMode;
			m_theAttractMode = nullptr;

			HandleQuitRequested();

			return;
		}
	}

	

	// go to play mode
	if ((g_theInput->WasKeyJustPressed(KEYCODE_SPACE) || controller.IsButtonDown(XBOX_BUTTON_A)) && 
		m_gameState == ATTRACT_MODE)
	{
		delete m_theAttractMode;
		m_theAttractMode = nullptr;

		m_theGame = new Game(this);
		m_theGame->Startup();
		m_gameState = PLAY_MODE;

		return;
	}

	// Hard Reset game on (f8)
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{
		if (m_gameState == PLAY_MODE)
		{
			bool isDebugViewOn = m_theGame->IsDubugViewOn();
			m_theGame->Shutdown();
			delete m_theGame;

			m_theGame = new Game(this, isDebugViewOn);
			m_theGame->Startup();
		}
		return;
	}

	// Hard reset game & go back to attract screen (f9)
	if (g_theInput->WasKeyJustPressed(KEYCODE_F9))
	{
		if (m_gameState == ATTRACT_MODE)
		{
			delete m_theAttractMode;
			m_theAttractMode = nullptr;
		}
		else if (m_gameState == PLAY_MODE)
		{
			m_theGame->Shutdown();
			delete m_theGame;
			m_theGame = nullptr;
		}

		m_theAttractMode = new AttractMode();
		m_theAttractMode->Startup();
		m_gameState = ATTRACT_MODE;

		return;
	}

	if (m_gameState == ATTRACT_MODE && m_theAttractMode != nullptr)
		m_theAttractMode->Update();

	if (m_gameState == PLAY_MODE && m_theGame != nullptr)
		m_theGame->Update();
}

void App::Render() const
{
	if (m_isQuitting)
		return;

	if (g_theRenderer == nullptr)
		return;

	if (m_gameState == PLAY_MODE && m_theGame != nullptr)
	{
		m_theGame->Render();
	}
	else if (m_gameState == ATTRACT_MODE && m_theAttractMode != nullptr)
	{
		m_theAttractMode->Render();
	}

	// render dev console
	if (g_theDevConsole)
	{
		DevConsoleRenderConfig renderConfig;
		if (m_gameState == PLAY_MODE && m_theGame != nullptr)
		{
			renderConfig.m_camera = &m_theGame->m_screenCamera;
		}
		else if (m_gameState == ATTRACT_MODE && m_theAttractMode != nullptr)
		{
			renderConfig.m_camera = &m_theAttractMode->m_screenCamera;
		}

		g_theDevConsole->Render(renderConfig);
	}

	RenderTestMouse();
}


void App::RenderTestMouse() const
{
	/*Vec2 cursorClientPos = g_theInput->GetCursorClientPosition();
	IntVec2 clientDimensions = g_theWindow->GetClientDimensions();
	

	float cursorScreenX = RangeMap(cursorClientPos.x, m_theGame->m_screenCamera.GetOrthographicBottomLeft().x, )*/
}


void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theWindow->EndFrame();
	g_theRenderer->EndFrame();
	g_theDevConsole->EndFrame();
	DebugRenderEndFrame();

	if (m_gameState == PLAY_MODE && m_theGame != nullptr) m_theGame->EndFrame();
}


void App::LoadFonts()
{
	// test font
	g_simpleBitmapFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");

	// dev console fonts
	if (g_theDevConsole)
	{
		g_theDevConsole->LoadFonts();
	}
}

void App::LoadTextures()
{
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");
}


bool App::EventHandler_CloseWindow(EventArgs& eventArgs)
{
	UNUSED(eventArgs);

	if (g_theApp != nullptr)
	{
		g_theApp->HandleQuitRequested();
		return true;
	}

	return false;
}

void App::AddGameKeyText()
{
	if (g_theDevConsole)
	{
		g_theDevConsole->AddLine(DevConsole::INFO_MAJOR_COLOR, "Major Controls");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "---------------");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- E/S/D/F		: XY Movement");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- Z / C			: Z Movement");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- Shift			: Sprint");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- 1				: Spawn wire frame sphere");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- 2				: spawn Line");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- 3				: Spawn Basis");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- 4				: spawn Billboard Text");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- 5				: Spawn Wire frame Cylinder");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- 6				: Spawn point");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- 7				: Add Message");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- ~				: Open Dev console");
		g_theDevConsole->AddLine(DevConsole::INFO_MAJOR_COLOR, "Other Controls");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "---------------");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- Space	: Start game from Attract mode. ");
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR_COLOR, "- Esc	: Quit if in Attract Mode. / Go back to Attract mode if in Game mode.");
		g_theDevConsole->AddLine(DevConsole::INFO_MAJOR_COLOR, "Type help for a list of commands");
	}
}


void App::UpdateCursorState()
{
	bool cursorHidden = false;
	bool cursorRelative = false;
	bool isCurrentWindowFocused = g_theWindow->DoesCurrentWindowHaveFocus();

	if (isCurrentWindowFocused && 
		m_gameState == PLAY_MODE &&
		!g_theDevConsole->IsOpen())
	{
		cursorHidden = true;
		cursorRelative = true;
	}

	g_theInput->SetCursorMode(cursorHidden, cursorRelative);
}