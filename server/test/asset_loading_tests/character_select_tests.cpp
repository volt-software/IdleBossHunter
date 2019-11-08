/*
    Realm of Aesir
    Copyright (C) 2019  Michael de Lang

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <catch2/catch.hpp>
#include <iostream>
#include <fstream>

#include <ecs/components.h>
#include "asset_loading/load_character_select.h"

using namespace std;
using namespace lotr;

TEST_CASE("character select loading tests") {
    ofstream charsel_file("test_charselect.json", ios_base::trunc);
    charsel_file << R"({
      "classes": [{
        "name": "Warrior",
        "description": "",
        "expGain": 100,
        "levelStats": {
          "strength": 10,
          "agility": 5,
          "psi": 1,
          "vitality": 5,
          "speed": 5
        },
        "items": [{
          "slot": "weapon",
          "name": "Long Sword",
          "value": 10,
          "strength": 25
        },{
          "slot": "body",
          "name": "Shirt",
          "value": 10,
          "vitality": 25
        }],
        "skills": [{
          "name": "Melee Prowess",
          "level": 10
        }]
      },{
        "name": "Mage",
        "description": "",
        "expGain": 100,
        "levelStats": {
          "strength": 10,
          "agility": 5,
          "psi": 1,
          "vitality": 5,
          "speed": 5
        },
        "items": [{
          "slot": "weapon",
          "name": "Staff",
          "value": 10,
          "psi": 25
        },{
          "slot": "body",
          "name": "Shirt",
          "value": 10,
          "vitality": 25
        }],
        "skills": [{
          "name": "Magic Prowess",
          "level": 10
        }]
      },{
        "name": "Thief",
        "description": "",
        "expGain": 100,
        "levelStats": {
          "strength": 10,
          "agility": 5,
          "psi": 1,
          "vitality": 5,
          "speed": 5
        },
        "items": [{
          "slot": "weapon",
          "name": "Dagger",
          "value": 10,
          "agility": 10,
          "speed": 15
        },{
          "slot": "body",
          "name": "Shirt",
          "value": 10,
          "Vitality": 25
        }],
        "skills": [{
          "name": "Thievery",
          "level": 10
        }]
      }],
      "races": [{
        "name": "Human",
        "description": "",
        "levelStats": {
          "strength": 7,
          "agility": 7,
          "psi": 7,
          "vitality": 7,
          "speed": 7
        }
      },{
        "name": "Dwarf",
        "description": "",
        "levelStats": {
          "strength": 10,
          "agility": 5,
          "psi": 5,
          "vitality": 10,
          "speed": 5
        }
      },{
        "name": "Gnome",
        "description": "",
        "levelStats": {
          "strength": 5,
          "agility": 10,
          "psi": 5,
          "vitality": 5,
          "speed": 10
        }
      },{
        "name": "Elf",
        "description": "",
        "levelStats": {
          "strength": 5,
          "agility": 5,
          "psi": 5,
          "vitality": 10,
          "speed": 10
        }
      },{
        "name": "High Elf",
        "description": "",
        "levelStats": {
          "strength": 5,
          "agility": 5,
          "psi": 10,
          "vitality": 10,
          "speed": 5
        }
      }]
    }
    )";

    charsel_file.close();

    auto charsel = load_character_select("test_charselect.json");
    REQUIRE(charsel);
    REQUIRE(charsel->races.size() == 5);
    REQUIRE(charsel->classes.size() == 3);
}
