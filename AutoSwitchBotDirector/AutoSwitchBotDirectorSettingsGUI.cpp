#include "pch.h"
#include "AutoSwitchBotDirector.h"


void AutoSwitchBotDirector::RenderSettings()
{
	RenderCheckbox("Enable When Replay Ends", enableWhenEndOfReplay.get(), "autodirector_when_end_of_replay",
		"Auto switch to bot director when a replay ends");

	RenderCheckbox("Enable When Overtime Kick Off Starts", enableWhenOvertimeKickOff.get(), "autodirector_when_overtime_kickoff_start",
		"Auto switch to bot director when the overtime kick off starts");

	RenderCheckbox("Enable When End Of Kick Off Countdown", enableWhenEndOfCountdown.get(), "autodirector_when_end_of_kickoff_countdown",
		"Auto switch to bot director when the kick off countdown ends, when \"GO!\" is displayed");

	RenderCheckbox("Enable When Goal Scored In Replay", enableWhenGoalScoredInReplay.get(), "autodirector_when_goal_scored_in_replay",
		"Auto switch to bot director when a goal is scored during a replay");
}

void AutoSwitchBotDirector::RenderCheckbox(const std::string& _label, bool* _value, const std::string& _cvar, const std::string& toolTipLabel)
{
	if (ImGui::Checkbox(_label.c_str(), _value))
	{
		cvarManager->getCvar(_cvar).setValue(*_value);
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text(toolTipLabel.c_str());
		ImGui::EndTooltip();
	}
}