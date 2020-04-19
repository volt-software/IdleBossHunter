/*
    IdleBossHunter
    Copyright (C) 2020 Michael de Lang

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


#pragma once

#include "scenes/scene.h"
#include <messages/company/get_company_listing_response.h>

namespace ibh {
    class companies_overview_scene : public scene  {
    public:
        explicit companies_overview_scene(iscene_manager *manager);
        ~companies_overview_scene() override = default;

        void update(iscene_manager *manager, TimeDelta dt) override;
        void handle_message(iscene_manager *manager, uint64_t type, message const* msg) override;
    private:
        string _error;
        bool _waiting_for_reply;
        bool _waiting_for_companies;
        string _selected_company;
        vector<company_object> _companies;
    };
}
