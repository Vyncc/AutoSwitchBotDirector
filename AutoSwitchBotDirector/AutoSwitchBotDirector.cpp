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


	cvarManager->registerNotifier("camera_director", [&](std::vector<std::string> args) {
		SwitchToBotDirector();
		}, "Switch to bot director", 0);


	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState",
		std::bind(&AutoSwitchBotDirector::OnReplayBegins, this, std::placeholders::_1));

	gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.ReplayPlayback.EndState",
		std::bind(&AutoSwitchBotDirector::OnReplayEnds, this, std::placeholders::_1));

	gameWrapper->HookEventWithCaller<ServerWrapper>("Function GameEvent_Soccar_TA.Countdown.BeginState",
		std::bind(&AutoSwitchBotDirector::OnKickOffBegins, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Countdown.EndState",
		std::bind(&AutoSwitchBotDirector::OnCountdownEnds, this, std::placeholders::_1));

	/*gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode",
		std::bind(&AutoSwitchBotDirector::OnBallExplode, this, std::placeholders::_1));*/

	gameWrapper->HookEvent("Function TAGame.Replay_TA.Tick",
		std::bind(&AutoSwitchBotDirector::OnReplayTick, this, std::placeholders::_1));

	gameWrapper->HookEventWithCaller<BallWrapper>("Function TAGame.Ball_TA.OnHitGoal",
		std::bind(&AutoSwitchBotDirector::OnHitGoal, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

std::string AutoSwitchBotDirector::GetCameraMode()
{
	PlayerControllerWrapper pc = gameWrapper->GetPlayerController();
	if (!pc) return "null";

	SpectatorHUDWrapper spectatorHUD = pc.GetSpectatorHud();
	if (!spectatorHUD) return "null";

	ReplayViewerDataWrapper replayViewer = spectatorHUD.GetViewerData();
	if (!replayViewer) return "null";

	return replayViewer.GetCameraMode();
}

void AutoSwitchBotDirector::CheckGoalReplayForAutoSwitch(const float& ballLocY, const float& treshHold)
{
	if (ballLocY > treshHold || ballLocY < -treshHold)
	{
		std::string camMode = GetCameraMode();
		//LOG("camMode : {}", camMode);

		if (camMode != "Camera_Director")
		{
			LOG("treshHold : {}", treshHold);

			if (cvarManager->getCvar("cl_goalreplay_pov").getBoolValue() == true)
			{
				LOG("Disabling POV goal replays");
				cvarManager->executeCommand("cl_goalreplay_pov 0");
			}
			SwitchToBotDirector();
		}
	}
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

//void AutoSwitchBotDirector::OnBallExplode(std::string eventName)
//{
//	if (isInReplay && *enableWhenGoalScoredInReplay)
//	{
//		if (cvarManager->getCvar("cl_goalreplay_pov").getBoolValue() == true)
//		{
//			LOG("Disabling POV goal replays");
//			cvarManager->executeCommand("cl_goalreplay_pov 0");
//		}
//		SwitchToBotDirector();
//	}
//}

void AutoSwitchBotDirector::OnReplayTick(std::string eventName)
{
	if (isInReplay && *enableWhenGoalScoredInReplay)
	{
		ServerWrapper server = gameWrapper->GetCurrentGameState();
		if (!server) return;

		BallWrapper ball = server.GetBall();
		if (!ball) return;

		float ballLocY = ball.GetLocation().Y;
		//LOG("Ball Y : {}", ballLocY);

		if (goalSpeed < 50.f)
		{
			CheckGoalReplayForAutoSwitch(ballLocY, 5210.f);
		}
		else if (goalSpeed < 70.f)
		{
			CheckGoalReplayForAutoSwitch(ballLocY, 5190.f);
		}
		else if (goalSpeed < 90.f)
		{
			CheckGoalReplayForAutoSwitch(ballLocY, 5175.f);
		}
		else if (goalSpeed < 110.f)
		{
			CheckGoalReplayForAutoSwitch(ballLocY, 5160.f);
		}
		else if (goalSpeed < 130.f)
		{
			CheckGoalReplayForAutoSwitch(ballLocY, 5145.f);
		}
		else if (goalSpeed < 150.f)
		{
			CheckGoalReplayForAutoSwitch(ballLocY, 5130.f);
		}
		else if (goalSpeed < 170.f)
		{
			CheckGoalReplayForAutoSwitch(ballLocY, 5115.f);
		}
		else
		{
			CheckGoalReplayForAutoSwitch(ballLocY, 5110.f);
		}
	}
}

void AutoSwitchBotDirector::OnHitGoal(BallWrapper caller, void* params, std::string eventName)
{
	if (!isInReplay)
	{
		goalSpeed = caller.GetVelocity().magnitude() * 0.036f; //in km/h
		LOG("goalSpeed : {}", goalSpeed);
	}
}
