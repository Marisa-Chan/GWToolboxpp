#include "EditBuild.h"

#include <string>

#include "GWToolbox.h"
#include "logger.h"

using namespace std;
using namespace OSHGui;

EditBuild::EditBuild() {
	names = vector<TextBox*>(N_PLAYERS);
	templates = vector<TextBox*>(N_PLAYERS);

	SetSize(450, 500);

	Label* label_name = new Label();
	label_name->SetText(L"Build Name:");
	label_name->SetLocation(DefaultBorderPadding, DefaultBorderPadding + 3);
	AddControl(label_name);

	Button* button = new Button();
	button->SetSize(ITEM_HEIGHT, ITEM_HEIGHT);
	button->SetLocation(GetWidth() - ITEM_HEIGHT - DefaultBorderPadding, DefaultBorderPadding);
	button->SetText(L"^");
	button->GetClickEvent() += ClickEventHandler([this](Control*) {
		up_ = !up_;
		UpdateLocation();
	});
	AddControl(button);

	name = new TextBox();
	name->SetSize(button->GetLeft() - label_name->GetRight() - 2 * DefaultBorderPadding, ITEM_HEIGHT);
	name->SetLocation(label_name->GetRight() + DefaultBorderPadding, DefaultBorderPadding);
	EnableTextInput(name);
	AddControl(name);

	Label* label_names = new Label();
	label_names->SetText(L"Names:");
	label_names->SetLocation(NAME_LEFT, name->GetBottom() + DefaultBorderPadding * 2);
	AddControl(label_names);

	const int starty = label_names->GetBottom() + DefaultBorderPadding;
	const int row_height = ITEM_HEIGHT + DefaultBorderPadding;
	for (int i = 0; i < N_PLAYERS; ++i) {
		Label* label_index = new Label();
		label_index->SetText(wstring(L"#") + to_wstring(i + 1));
		label_index->SetLocation(DefaultBorderPadding, starty + row_height * i + 3);
		AddControl(label_index);

		names[i] = new TextBox();
		names[i]->SetLocation(NAME_LEFT, starty + row_height * i);
		names[i]->SetSize(NAME_WIDTH, ITEM_HEIGHT);
		EnableTextInput(names[i]);
		AddControl(names[i]);

		Button* send = new Button();
		send->SetText(L"Send");
		send->SetSize(SEND_WIDTH, names[i]->GetHeight());
		send->SetLocation(GetWidth() - send->GetWidth() - DefaultBorderPadding / 2, 
			starty + row_height * i);
		send->GetClickEvent() += ClickEventHandler([this, i](Control*) {
			wstring message = L"[";
			if (show_numbers->GetChecked()) {
				message += to_wstring(i + 1);
				message += L" - ";
			}
			message += names[i]->GetText();
			message += L";";
			message += templates[i]->GetText();
			message += L"]";
			GWCA api;
			api().Chat().SendChat(message.c_str(), L'#');
		});
		AddControl(send);

		templates[i] = new TextBox();
		templates[i]->SetLocation(names[i]->GetRight() + DefaultBorderPadding / 2, starty + row_height * i);
		templates[i]->SetSize(send->GetLeft() - names[i]->GetRight() - DefaultBorderPadding, ITEM_HEIGHT);
		EnableTextInput(templates[i]);
		AddControl(templates[i]);
	}

	Label* label_templates = new Label();
	label_templates->SetText(L"Templates:");
	label_templates->SetLocation(templates[0]->GetLeft(), name->GetBottom() + DefaultBorderPadding * 2);
	AddControl(label_templates);

	const int last_row_y = names[11]->GetBottom() + DefaultBorderPadding;
	const int buttons_width = (GetWidth() - templates[0]->GetLeft() - DefaultBorderPadding) / 2;

	show_numbers = new CheckBox();
	show_numbers->SetText(L"Show Build numbers");
	show_numbers->SetLocation(DefaultBorderPadding, last_row_y);
	AddControl(show_numbers);
	
	Button* cancel = new Button();
	cancel->SetText(L"Cancel");
	cancel->SetLocation(templates[0]->GetLeft(), last_row_y);
	cancel->SetSize(buttons_width, ITEM_HEIGHT);
	cancel->GetClickEvent() += ClickEventHandler([this](Control*) {
		this->SetVisible(false);
	});
	AddControl(cancel);

	Button* ok = new Button();
	ok->SetText(L"Ok");
	ok->SetLocation(cancel->GetRight() + DefaultBorderPadding, last_row_y);
	ok->SetSize(buttons_width, ITEM_HEIGHT);
	ok->GetClickEvent() += ClickEventHandler([this](Control*) {
		this->SetVisible(false);
		SaveBuild();
	});
	AddControl(ok);

	SetSize(templates[0]->GetRight() + SEND_WIDTH + 2 * DefaultBorderPadding,
		ok->GetBottom() + DefaultBorderPadding);

	up_ = true;
	left_ = false;
	UpdateLocation();
}

void EditBuild::EnableTextInput(TextBox* tb) {
	tb->GetFocusGotEvent() += FocusGotEventHandler([](Control*) {
		GWToolbox::instance().set_capture_input(true);
	});
	tb->GetFocusLostEvent() += FocusLostEventHandler([](Control*, Control*) {
		GWToolbox::instance().set_capture_input(false);
	});
}

void EditBuild::SetEditedBuild(int index, Button* button) {
	editing_index = index;
	editing_button = button;

	Config& config = GWToolbox::instance().config();
	wstring section = wstring(L"builds") + to_wstring(index);
	wstring key;
	
	key = L"buildname";
	wstring buildname = config.IniRead(section.c_str(), key.c_str(), L"");
	name->SetText(buildname);
	for (int i = 0; i < N_PLAYERS; ++i) {
		key = L"name" + to_wstring(i + 1);
		wstring name = config.IniRead(section.c_str(), key.c_str(), L"");
		names[i]->SetText(name);

		key = L"template" + to_wstring(i + 1);
		wstring temp = config.IniRead(section.c_str(), key.c_str(), L"");
		templates[i]->SetText(temp);
	}
	show_numbers->SetChecked(config.IniReadBool(section.c_str(), L"showNumbers", true));

	SetVisible(true);
}

void EditBuild::SaveBuild() {
	Config& config = GWToolbox::instance().config();
	wstring section = wstring(L"builds") + to_wstring(editing_index);
	wstring key;

	wstring s_name = name->GetText();
	key = L"buildname";
	config.IniWrite(section.c_str(), key.c_str(), s_name.c_str());
	editing_button->SetText(s_name);

	for (int i = 0; i < N_PLAYERS; ++i) {
		wstring s_name = names[i]->GetText();
		key = L"name" + to_wstring(i + 1);
		config.IniWrite(section.c_str(), key.c_str(), s_name.c_str());

		wstring s_template = templates[i]->GetText();
		key = L"template" + to_wstring(i + 1);
		config.IniWrite(section.c_str(), key.c_str(), s_template.c_str());
	}

	config.IniWriteBool(section.c_str(), L"showNumbers", show_numbers->GetChecked());
}

void EditBuild::UpdateLocation() {
	int x = left_ ? -GetWidth() : MainWindow::SIDE_PANEL_WIDTH;
	int y = up_ ? 0 : MainWindow::HEIGHT - GetHeight();

	SetLocation(x, y);
}
