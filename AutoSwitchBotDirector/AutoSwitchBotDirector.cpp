#include "pch.h"
#include "AutoSwitchBotDirector.h"


BAKKESMOD_PLUGIN(AutoSwitchBotDirector, "AutoSwitchBotDirector", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void AutoSwitchBotDirector::onLoad()
{
	_globalCvarManager = cvarManager;

	enableWhenEndOfReplay = std::make_shared<bool>();
	cvarManager->registerCvar("autodirector_when_end_of_replay", "1", "Auto switch to bot director when a replay ends",
		true, true, 0, true, 1).bindTo(enableWhenEndOfReplay);

	enableWhenOvertimeKickOff = std::make_shared<bool>();
	cvarManager->registerCvar("autodirector_when_overtime_kickoff_start", "1", "Auto switch to bot director when the overtime kick off starts",
		true, true, 0, true, 1).bindTo(enableWhenOvertimeKickOff);
	
	enableWhenEndOfCountdown = std::make_shared<bool>();
	cvarManager->registerCvar("autodirector_when_end_of_kickoff_countdown", "1", "Auto switch to bot director when the kick off countdown ends, when \"GO!\" is displayed",
		true, true, 0, true, 1). bindTo(enableWhenEndOfCountdown);
	
	enableWhenGoalScoredInReplay = std::make_shared<bool>();
	cvarManager->registerCvar("autodirector_when_goal_scored_in_replay", "1", "Auto switch to bot director when a goal is scored during a replay",
		true, true, 0, true, 1). bindTo(enableWhenGoalScoredInReplay);

	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState",
		std::bind(&AutoSwitchBotDirector::OnReplayBegins, this, std::placeholders::_1));

	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.EndState",
		std::bind(&AutoSwitchBotDirector::OnReplayEnds, this, std::placeholders::_1));

	gameWrapper->HookEventWithCaller<ServerWrapper>("Function GameEvent_Soccar_TA.Countdown.BeginState",
		std::bind(&AutoSwitchBotDirector::OnKickOffBegins, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Countdown.EndState",
		std::bind(&AutoSwitchBotDirector::OnCountdownEnds, this, std::placeholders::_1));

	gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode",
		std::bind(&AutoSwitchBotDirector::OnBallExplode, this, std::placeholders::_1));


	cvarManager->registerNotifier("camera_director", [&](std::vector<std::string> args) {
		SwitchToBotDirector();
		}, "", 0);


	//Debug command to score a goal
	cvarManager->registerNotifier("tpball", [&](std::vector<std::string> args) {
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
		}, "", 0);
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

	spectatorHUD.SetFocusActorString("");
	replayViewer.SetCameraMode("Camera_Director");
	LOG("auto siwtch to bot director");
}

void AutoSwitchBotDirector::OnKickOffBegins(ServerWrapper caller, void* params, std::string eventName)
{
	if (*enableWhenOvertimeKickOff)
	{
		if (caller.GetbOverTime())
		{
			SwitchToBotDirector();
		}
	}
}

void AutoSwitchBotDirector::OnReplayBegins(std::string eventName)
{
	isInReplay = true;
}

void AutoSwitchBotDirector::OnReplayEnds(std::string eventName)
{
	isInReplay = false;

	if (*enableWhenEndOfReplay)
	{
		SwitchToBotDirector();
	}
}

void AutoSwitchBotDirector::OnCountdownEnds(std::string eventName)
{
	if (*enableWhenEndOfCountdown)
	{
		SwitchToBotDirector();
	}
}

void AutoSwitchBotDirector::OnBallExplode(std::string eventName)
{
	if (*enableWhenGoalScoredInReplay)
	{
		if (isInReplay)
		{
			SwitchToBotDirector();
		}
	}
}