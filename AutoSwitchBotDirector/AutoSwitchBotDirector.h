#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class AutoSwitchBotDirector: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	//void onUnload() override; // Uncomment and implement if you need a unload method

	std::shared_ptr<bool> enableWhenEndOfReplay;
	std::shared_ptr<bool> enableWhenOvertimeKickOff;
	std::shared_ptr<bool> enableWhenEndOfCountdown;
	std::shared_ptr<bool> enableWhenGoalScoredInReplay;

	bool isInReplay = false;
	float goalSpeed = -1.f;

	std::string GetCameraMode();
	void CheckGoalReplayForAutoSwitch(const float& ballLocY, const float& treshHold);
	void SwitchToBotDirector();

	//Events
	void OnKickOffBegins(ServerWrapper caller, void* params, std::string eventName);
	void OnReplayBegins(std::string eventName);
	void OnReplayEnds(std::string eventName);
	void OnCountdownEnds(std::string eventName);
	//void OnBallExplode(std::string eventName);
	void OnReplayTick(std::string eventName);
	void OnHitGoal(BallWrapper caller, void* params, std::string eventName);

	void RenderCheckbox(const std::string& _label, bool* _value, const std::string& _cvar, const std::string& toolTipLabel);

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window
};
