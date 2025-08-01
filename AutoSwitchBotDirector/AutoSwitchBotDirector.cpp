#include "pch.h"
#include "AutoSwitchBotDirector.h"


BAKKESMOD_PLUGIN(AutoSwitchBotDirector, "AutoSwitchBotDirector", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void AutoSwitchBotDirector::onLoad()
{
	_globalCvarManager = cvarManager;

	//Debug command to score a goal
	/*cvarManager->registerNotifier("tpball", [&](std::vector<std::string> args) {
		ServerWrapper server = gameWrapper->GetCurrentGameState();
		if (!server)
		{
			LOG("[ERROR]server NULL!");
			return;
		}

		BallWrapper ball = server.GetBall();
		if (!ball)
		{
			LOG("[ERROR]ball NULL!");
			return;
		}

		ball.SetLocation(Vector(0, 5400, 100));
		}, "", 0);*/

	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.EndState",
		std::bind(&AutoSwitchBotDirector::OnReplayEnds, this, std::placeholders::_1));

	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Countdown.BeginState",
		std::bind(&AutoSwitchBotDirector::OnOvertimeKickOffBegins, this, std::placeholders::_1));
}

void AutoSwitchBotDirector::SwitchToBotDirector()
{
	PlayerControllerWrapper pc = gameWrapper->GetPlayerController();
	if (!pc)
	{
		LOG("[ERROR]pc NULL!");
		return;
	}

	SpectatorHUDWrapper spectatorHUD = pc.GetSpectatorHud();
	if (!spectatorHUD)
	{
		LOG("[ERROR]spectatorHUD NULL!");
		return;
	}

	ReplayViewerDataWrapper replayViewer = spectatorHUD.GetViewerData();
	if (!replayViewer)
	{
		LOG("[ERROR]replayViewer NULL!");
		return;
	}

	replayViewer.SetCameraMode("Camera_Director");
}

void AutoSwitchBotDirector::OnReplayEnds(std::string eventName)
{
	if (!enableForEndOfReplay) return;
	SwitchToBotDirector();
}

void AutoSwitchBotDirector::OnOvertimeKickOffBegins(std::string eventName)
{
	if (!enableForOvertimeKickOff) return;

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server)
	{
		LOG("[ERROR]server NULL!");
		return;
	}

	if (server.GetbOverTime())
	{
		SwitchToBotDirector();
	}
}