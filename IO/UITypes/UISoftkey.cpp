/////////////////////////////////////////////////////////////////////////////
// This file is part of the Journey MMORPG client                           //
// Copyright � 2015 Daniel Allendorf                                        //
//                                                                          //
// This program is free software: you can redistribute it and/or modify     //
// it under the terms of the GNU Affero General Public License as           //
// published by the Free Software Foundation, either version 3 of the       //
// License, or (at your option) any later version.                          //
//                                                                          //
// This program is distributed in the hope that it will be useful,          //
// but WITHOUT ANY WARRANTY; without even the implied warranty of           //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
// GNU Affero General Public License for more details.                      //
//                                                                          //
// You should have received a copy of the GNU Affero General Public License //
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "UISoftKey.h"
#include "IO\Components\MapleButton.h"
#include "Net\Packets\SelectCharPackets83.h"
#include "nlnx\nx.hpp"
#include "nlnx\node.hpp"

#define button_ptr unique_ptr<Button>

namespace IO
{
	const uint8_t NUM_KEYS = 10;

	UISoftkey::UISoftkey(SkType t, UI& u, Session& ses) : ui(u), session(ses)
	{
		type = t;

		node src = nl::nx::ui["Login.img"]["Common"]["SoftKey"];
		sprites.push_back(Sprite(src["backgrnd"], vector2d<int32_t>()));
		sprites.push_back(Sprite(src["backgrnd2"], vector2d<int32_t>()));
		sprites.push_back(Sprite(src["backgrnd3"], vector2d<int32_t>()));

		buttons[BT_NEXT] = button_ptr(new MapleButton(src["BtNext"], vector2d<int32_t>()));
		buttons[BT_BACK] = button_ptr(new MapleButton(src["BtDel"], vector2d<int32_t>()));
		buttons[BT_OK] = button_ptr(new MapleButton(src["BtOK"], vector2d<int32_t>(72, 235)));
		buttons[BT_CANCEL] = button_ptr(new MapleButton(src["BtCancel"], vector2d<int32_t>(13, 235)));

		node keys = src["BtNum"];
		buttons[BT_0] = button_ptr(new MapleButton(keys["0"], vector2d<int32_t>()));
		buttons[BT_1] = button_ptr(new MapleButton(keys["1"], vector2d<int32_t>()));
		buttons[BT_2] = button_ptr(new MapleButton(keys["2"], vector2d<int32_t>()));
		buttons[BT_3] = button_ptr(new MapleButton(keys["3"], vector2d<int32_t>()));
		buttons[BT_4] = button_ptr(new MapleButton(keys["4"], vector2d<int32_t>()));
		buttons[BT_5] = button_ptr(new MapleButton(keys["5"], vector2d<int32_t>()));
		buttons[BT_6] = button_ptr(new MapleButton(keys["6"], vector2d<int32_t>()));
		buttons[BT_7] = button_ptr(new MapleButton(keys["7"], vector2d<int32_t>()));
		buttons[BT_8] = button_ptr(new MapleButton(keys["8"], vector2d<int32_t>()));
		buttons[BT_9] = button_ptr(new MapleButton(keys["9"], vector2d<int32_t>()));

		buttons[BT_OK]->setstate(Button::DISABLED);

		entry = Textlabel(Textlabel::DWF_12ML, Textlabel::TXC_BLACK, "", 0);

		shufflekeys();

		position = vector2d<int32_t>(330, 160);
		dimension = vector2d<int32_t>(140, 280);
		active = true;
	}

	UISoftkey::~UISoftkey() {}

	void UISoftkey::draw(float inter) const
	{
		UIElement::draw(inter);

		if (active)
		{
			entry.draw(position + vector2d<int32_t>(15, 41));
		}
	}

	void UISoftkey::buttonpressed(uint16_t id)
	{
		using::std::string;
		string entered = entry.gettext();

		switch (id)
		{
		case BT_0:
		case BT_1:
		case BT_2:
		case BT_3:
		case BT_4:
		case BT_5:
		case BT_6:
		case BT_7:
		case BT_8:
		case BT_9:
			if (entered.size() < 8)
			{
				entered.append(std::to_string(id));
				shufflekeys();
			}
			buttons[id]->setstate(Button::NORMAL);
			break;
		case BT_BACK:
			if (entered.size() > 0)
			{
				entered.pop_back();
			}
			buttons[id]->setstate(Button::NORMAL);
			break;
		case BT_CANCEL:
			active = false;
			break;
		case BT_OK:
			if (entered.size() > 5)
			{
				ui.disable();
				int32_t cid = session.getlogin().getcharid();
				switch (type)
				{
				case CHARSELECT:

					using::Net::SelectCharPicPacket83;
					session.dispatch(SelectCharPicPacket83(entered, cid));
					break;
				case REGISTER:

					using::Net::RegisterPicPacket83;
					session.dispatch(RegisterPicPacket83(cid, entered));
					break;
				}
				active = false;
			}
			break;
		}

		switch (entered.size())
		{
		case 5:
			buttons[BT_OK]->setstate(Button::DISABLED);
			break;
		case 6:
			buttons[BT_OK]->setstate(Button::NORMAL);
			break;
		case 7:
			if (entry.getlength() == 8)
			{
				for (uint8_t i = 0; i < NUM_KEYS; i++)
				{
					buttons[i]->setstate(Button::NORMAL);
				}
			}
			break;
		case 8:
			for (uint8_t i = 0; i < NUM_KEYS; i++)
			{
				buttons[i]->setstate(Button::DISABLED);
			}
			break;
		}

		entry.settext(entered, 0);
	}

	void UISoftkey::shufflekeys()
	{
		using::std::vector;
		vector<uint8_t> reserve;
		for (uint8_t i = 0; i < NUM_KEYS; i++)
		{
			reserve.push_back(i);
		}
		for (uint8_t i = 0; i < NUM_KEYS; i++)
		{
			size_t rand = random.nextint(reserve.size() - 1);
			vector2d<int32_t> pos = keypos(reserve[rand]);
			buttons[BT_0 + i]->setposition(pos);
			reserve.erase(reserve.begin() + rand);
		}
	}

	vector2d<int32_t> UISoftkey::keypos(uint8_t num) const
	{
		return vector2d<int32_t>(12 + (num % 3) * 39, 94 + (num / 3) * 35);
	}
}