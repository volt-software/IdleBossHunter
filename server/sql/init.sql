START TRANSACTION;

CREATE TABLE users (
    id BIGSERIAL PRIMARY KEY,
    username CITEXT NOT NULL,
    password VARCHAR(100) NOT NULL,
    email CITEXT NOT NULL,
    login_attempts SMALLINT NOT NULL DEFAULT 0,
    verification_code TEXT DEFAULT NULL,
    max_characters SMALLINT NOT NULL DEFAULT 0,
    subscription_tier SMALLINT NOT NULL DEFAULT 0,
    is_tester SMALLINT NOT NULL DEFAULT 0,
    is_game_master SMALLINT NOT NULL DEFAULT 0,
    discord_tag TEXT DEFAULT NULL
);

CREATE TABLE banned_users (
    id BIGSERIAL PRIMARY KEY,
    ip TEXT NULL,
    user_id BIGINT NULL,
    until BIGINT NULL
);

CREATE TABLE licenses (
    id BIGSERIAL PRIMARY KEY,
    license_name TEXT NOT NULL,
    author TEXT NOT NULL,
    license TEXT NOT NULL,
    software_name TEXT NOT NULL
);

CREATE TABLE characters (
    id BIGSERIAL PRIMARY KEY,
    user_id BIGINT NOT NULL,
    slot INT NOT NULL,
    level BIGINT NOT NULL,
    gold BIGINT NOT NULL,
    xp BIGINT NOT NULL,
    skill_points BIGINT NOT NULL,
    character_name CITEXT NOT NULL,
    race CITEXT NOT NULL,
    class CITEXT NOT NULL,
    x INT NOT NULL,
    y INT NOT NULL,
    map TEXT NOT NULL
);

CREATE TABLE character_stats (
    id BIGSERIAL PRIMARY KEY,
    character_id BIGINT NOT NULL,
    stat_id BIGINT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE character_skills (
    id BIGSERIAL PRIMARY KEY,
    character_id BIGINT NOT NULL,
    skill_name TEXT NOT NULL,
    xp BIGINT NOT NULL,
    level BIGINT NOT NULL
);

CREATE TABLE items (
    id BIGSERIAL PRIMARY KEY,
    character_id BIGINT NOT NULL,
    item_name CITEXT NOT NULL,
    item_slot CITEXT NOT NULL,
    equip_slot CITEXT NOT NULL
);

CREATE TABLE item_stats (
    id BIGSERIAL PRIMARY KEY,
    item_id BIGINT NOT NULL,
    stat_id BIGINT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE bosses (
    id BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL
);

CREATE TABLE boss_stats (
    id BIGSERIAL PRIMARY KEY,
    boss_id BIGINT NOT NULL,
    stat_id BIGINT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE companies (
    id BIGSERIAL PRIMARY KEY,
    name CITEXT NOT NULL,
    no_of_shares BIGINT NOT NULL,
    company_type SMALLINT NOT NULL
);

CREATE TABLE company_stats (
    id BIGSERIAL PRIMARY KEY,
    company_id BIGINT NOT NULL,
    stat_id BIGINT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE company_members (
    company_id BIGINT NOT NULL,
    character_id BIGINT NOT NULL,
    member_level SMALLINT NOT NULL,
    wage BIGINT NOT NULL
);

CREATE TABLE company_member_applications (
    company_id BIGINT NOT NULL,
    character_id BIGINT NOT NULL
);

CREATE TABLE company_buildings (
    id BIGSERIAL PRIMARY KEY,
    name CITEXT NOT NULL,
    company_id BIGINT NOT NULL,
    size SMALLINT NOT NULL,
    resource_type SMALLINT NOT NULL
);

CREATE TABLE company_building_machines (
    id BIGSERIAL PRIMARY KEY,
    name CITEXT NOT NULL,
    company_building_id BIGINT NOT NULL,
    type SMALLINT NOT NULL,
    level INT NOT NULL,
    last_serviced BIGINT NOT NULL
);

CREATE TABLE company_shareholders (
    id BIGSERIAL PRIMARY KEY,
    company_id BIGINT NOT NULL,
    character_id BIGINT NOT NULL,
    no_of_shares BIGINT NOT NULL
);

CREATE TABLE contracts (
    id BIGSERIAL PRIMARY KEY,
    name CITEXT NOT NULL,
    type SMALLINT NOT NULL
);

CREATE TABLE contract_parties (
    id BIGSERIAL PRIMARY KEY,
    contract_id BIGINT NOT NULL,
    character_id BIGINT NULL,
    company_id BIGINT NULL,
    primary_party BOOLEAN NOT NULL
);

CREATE TABLE settings (
    setting_name CITEXT NOT NULL,
    value TEXT NOT NULL
);

CREATE TABLE schema_information (
    file_name TEXT NOT NULL,
    date TIMESTAMPTZ NOT NULL
);

ALTER TABLE banned_users ADD CONSTRAINT "banned_users_user_id_fkey" FOREIGN KEY (user_id) REFERENCES users(id);
ALTER TABLE boss_stats ADD CONSTRAINT "boss_stats_bosses_id_fkey" FOREIGN KEY (boss_id) REFERENCES bosses(id);
ALTER TABLE characters ADD CONSTRAINT "characters_users_id_fkey" FOREIGN KEY (user_id) REFERENCES users(id);
ALTER TABLE character_stats ADD CONSTRAINT "character_stats_characters_id_fkey" FOREIGN KEY (character_id) REFERENCES characters(id);
ALTER TABLE company_members ADD CONSTRAINT "company_members_company_id_fkey" FOREIGN KEY (company_id) REFERENCES companies(id);
ALTER TABLE company_members ADD CONSTRAINT "company_members_character_id_fkey" FOREIGN KEY (character_id) REFERENCES characters(id);
ALTER TABLE company_member_applications ADD CONSTRAINT "company_member_applications_company_id_fkey" FOREIGN KEY (company_id) REFERENCES companies(id);
ALTER TABLE company_member_applications ADD CONSTRAINT "company_member_applications_character_id_fkey" FOREIGN KEY (character_id) REFERENCES characters(id);
ALTER TABLE company_stats ADD CONSTRAINT "company_stats_companies_id_fkey" FOREIGN KEY (company_id) REFERENCES companies(id);
ALTER TABLE company_buildings ADD CONSTRAINT "company_buildings_companies_id_fkey" FOREIGN KEY (company_id) REFERENCES companies(id);
ALTER TABLE company_building_machines ADD CONSTRAINT "company_building_machines_company_buildings_id_fkey" FOREIGN KEY (company_building_id) REFERENCES company_buildings(id);
ALTER TABLE company_shareholders ADD CONSTRAINT "company_shareholders_companies_id_fkey" FOREIGN KEY (company_id) REFERENCES companies(id);
ALTER TABLE company_shareholders ADD CONSTRAINT "company_shareholders_character_id_fkey" FOREIGN KEY (character_id) REFERENCES characters(id);
ALTER TABLE items ADD CONSTRAINT "items_characters_id_fkey" FOREIGN KEY (character_id) REFERENCES characters(id);
ALTER TABLE item_stats ADD CONSTRAINT "item_stats_items_id_fkey" FOREIGN KEY (item_id) REFERENCES items(id);

ALTER TABLE characters ADD CONSTRAINT "characters_slot_unique" UNIQUE (user_id, slot);
ALTER TABLE companies ADD CONSTRAINT "company_name_unique" UNIQUE (name);
ALTER TABLE company_members ADD CONSTRAINT "company_members_unique" UNIQUE (character_id);
ALTER TABLE company_member_applications ADD CONSTRAINT "company_member_applications_unique" UNIQUE (character_id, company_id);
ALTER TABLE schema_information ADD CONSTRAINT "schema_information_name_unique" UNIQUE (file_name);
ALTER TABLE settings ADD CONSTRAINT "settings_name_unique" UNIQUE (setting_name);
ALTER TABLE users ADD CONSTRAINT "users_username_unique" UNIQUE (username);

CREATE INDEX boss_stats_idx ON boss_stats (boss_id, stat_id);
CREATE INDEX character_stats_idx ON character_stats (character_id, stat_id);
CREATE INDEX company_stats_idx ON company_stats (company_id, stat_id);
CREATE INDEX item_stats_idx ON item_stats (item_id, stat_id);

INSERT INTO schema_information(file_name, date) VALUES ('init.sql', CURRENT_TIMESTAMP);

COMMIT;
