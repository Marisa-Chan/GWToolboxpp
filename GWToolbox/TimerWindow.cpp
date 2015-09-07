#include "TimerWindow.h"

#include <sstream>
#include <string>

#include "GWToolbox.h"
#include "Config.h"
#include "../API/APIMain.h"
#include "GuiUtils.h"

using namespace OSHGui;

TimerWindow::TimerWindow() {
	current_time_ = 0;
	in_urgoz_ = false;

	Config* config = GWToolbox::instance()->config();
	int x = config->iniReadLong(TimerWindow::IniSection(), TimerWindow::IniKeyX(), 100);
	int y = config->iniReadLong(TimerWindow::IniSection(), TimerWindow::IniKeyY(), 100);

	SetLocation(x, y);
	SetSize(Drawing::SizeI(WIDTH, HEIGHT));

	Drawing::Theme::ControlTheme theme = Application::InstancePtr()->GetTheme().GetControlColorTheme("timer");
	SetBackColor(theme.BackColor);

	int offsetX = 2;
	int offsetY = 2;
	shadow_ = new TimerLabel();
	shadow_->SetText("Temp");
	shadow_->SetAnchor(AnchorStyles::Left);
	shadow_->SetSize(WIDTH, HEIGHT);
	shadow_->SetLocation(offsetX, offsetY);
	shadow_->SetFont(GuiUtils::getTBFont(26.0f, true));
	shadow_->SetBackColor(Drawing::Color::Empty());
	shadow_->SetForeColor(Drawing::Color::Black());
	shadow_->SetEnabled(false);
	AddControl(shadow_);
	
	timer_ = new TimerLabel();
	timer_->SetText("Temp");
	timer_->SetAnchor(AnchorStyles::Left);
	timer_->SetSize(WIDTH, HEIGHT);
	timer_->SetLocation(0, 0);
	timer_->SetFont(GuiUtils::getTBFont(26.0f, true));
	timer_->SetForeColor(theme.ForeColor);
	timer_->SetBackColor(Drawing::Color::Empty());
	timer_->GetMouseUpEvent() += MouseUpEventHandler([this](Control*, MouseEventArgs) {
		SaveLocation();
	});
	AddControl(timer_);

	urgoz_shadow_ = new TimerLabel();
	urgoz_shadow_->SetText("Temp");
	urgoz_shadow_->SetAnchor(AnchorStyles::Left);
	urgoz_shadow_->SetSize(WIDTH, URGOZ_HEIGHT);
	urgoz_shadow_->SetLocation(offsetX, HEIGHT + offsetY);
	urgoz_shadow_->SetFont(GuiUtils::getTBFont(16.0f, true));
	urgoz_shadow_->SetBackColor(Drawing::Color::Empty());
	urgoz_shadow_->SetForeColor(Drawing::Color::Black());
	urgoz_shadow_->SetEnabled(false);
	urgoz_shadow_->SetVisible(false);
	AddControl(urgoz_shadow_);

	urgoz_timer_ = new TimerLabel();
	urgoz_timer_->SetText("Temp");
	urgoz_timer_->SetAnchor(AnchorStyles::Left);
	urgoz_timer_->SetSize(WIDTH, URGOZ_HEIGHT);
	urgoz_timer_->SetLocation(0, HEIGHT);
	urgoz_timer_->SetFont(GuiUtils::getTBFont(16.0f, true));
	urgoz_timer_->SetForeColor(theme.ForeColor);
	urgoz_timer_->SetBackColor(Drawing::Color::Empty());
	urgoz_timer_->GetMouseUpEvent() += MouseUpEventHandler([this](Control*, MouseEventArgs) {
		SaveLocation();
	});
	urgoz_timer_->SetVisible(false);
	AddControl(urgoz_timer_);
}

void TimerWindow::SaveLocation() {
	CalculateAbsoluteLocation();
	int x = absoluteLocation_.X;
	int y = absoluteLocation_.Y;
	Config* config = GWToolbox::instance()->config();
	config->iniWriteLong(TimerWindow::IniSection(), TimerWindow::IniKeyX(), x);
	config->iniWriteLong(TimerWindow::IniSection(), TimerWindow::IniKeyY(), y);
}

void TimerWindow::UpdateLabel() {
	long uptime = GWAPIMgr::GetInstance()->Map->GetInstanceTime();
	long time = uptime / 1000;
	if (time != current_time_) {
		current_time_ = time;

		int seconds = time % 60;
		int minutes = (int)(time / 60) % 60;
		int hours = (int)(time / (60 * 60));

		stringstream ss;
		ss.fill('0');
		ss << hours;
		ss << ":";
		ss.width(2);
		ss << minutes;
		ss.width(1);
		ss << ":";
		ss.width(2);
		ss << seconds;
		
		timer_->SetText(ss.str());
		timer_->Invalidate();

		shadow_->SetText(ss.str());
		shadow_->Invalidate();

		DWORD map_id = GWAPIMgr::GetInstance()->Map->GetMapID();
		if (map_id == GwConstants::MapID::Urgoz) {
			if (!in_urgoz_) {
				in_urgoz_ = true;
				SetSize(Drawing::SizeI(WIDTH, HEIGHT + URGOZ_HEIGHT));
				urgoz_shadow_->SetVisible(true);
				urgoz_timer_->SetVisible(true);
			}

			int temp = (time - 1) % 25;
			
			string text;
			if (temp < 15) {
				urgoz_timer_->SetForeColor(Drawing::Color::Green());
				text = string("Open - ") + to_string(15 - temp);
			} else {
				urgoz_timer_->SetForeColor(Drawing::Color::Red());
				text = string("Closed - ") + to_string(25 - temp);
			}
			urgoz_timer_->SetText(text);
			urgoz_shadow_->SetText(text);

		} else {
			if (in_urgoz_) {
				in_urgoz_ = false;
				SetSize(Drawing::SizeI(WIDTH, HEIGHT));
				urgoz_shadow_->SetVisible(false);
				urgoz_timer_->SetVisible(false);
			}
		}
	}
	
}