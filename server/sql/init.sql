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
    stat_name TEXT NOT NULL,
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
    stat_name CITEXT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE bosses (
    id BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL
);

CREATE TABLE boss_stats (
    id BIGSERIAL PRIMARY KEY,
    boss_id BIGINT NOT NULL,
    stat_name TEXT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE clans (
    id BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL
);

CREATE TABLE clan_members (
    clan_id BIGINT NOT NULL,
    character_id BIGINT NOT NULL,
    member_level SMALLINT NOT NULL
);

CREATE TABLE clan_member_applications (
    clan_id BIGINT NOT NULL,
    character_id BIGINT NOT NULL
);

CREATE TABLE clan_stats (
    id BIGSERIAL PRIMARY KEY,
    clan_id BIGINT NOT NULL,
    stat_name TEXT NOT NULL,
    value BIGINT NOT NULL
);

CREATE TABLE clan_buildings (
    id BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    clan_id BIGINT NOT NULL
);

CREATE TABLE settings (
    setting_name CITEXT NOT NULL,
    value TEXT NOT NULL
);

CREATE TABLE schema_information (
    file_name TEXT NOT NULL,
    date TIMESTAMPTZ NOT NULL
);

ALTER TABLE users ADD CONSTRAINT "users_username_unique" UNIQUE (username);
ALTER TABLE banned_users ADD CONSTRAINT "banned_users_user_id_fkey" FOREIGN KEY (user_id) REFERENCES users(id);
ALTER TABLE characters ADD CONSTRAINT "characters_users_id_fkey" FOREIGN KEY (user_id) REFERENCES users(id);
ALTER TABLE characters ADD CONSTRAINT "characters_slot_unique" UNIQUE (user_id, slot);
ALTER TABLE character_stats ADD CONSTRAINT "character_stats_characters_id_fkey" FOREIGN KEY (character_id) REFERENCES characters(id);
ALTER TABLE items ADD CONSTRAINT "items_characters_id_fkey" FOREIGN KEY (character_id) REFERENCES characters(id);
ALTER TABLE item_stats ADD CONSTRAINT "item_stats_items_id_fkey" FOREIGN KEY (item_id) REFERENCES items(id);
ALTER TABLE boss_stats ADD CONSTRAINT "boss_stats_bosses_id_fkey" FOREIGN KEY (boss_id) REFERENCES bosses(id);
ALTER TABLE clan_members ADD CONSTRAINT "clan_members_clan_id_fkey" FOREIGN KEY (clan_id) REFERENCES clans(id);
ALTER TABLE clan_members ADD CONSTRAINT "clan_members_character_id_fkey" FOREIGN KEY (character_id) REFERENCES characters(id);
ALTER TABLE clan_members ADD CONSTRAINT "clan_members_unique" UNIQUE (character_id);
ALTER TABLE clan_member_applications ADD CONSTRAINT "clan_member_applications_clan_id_fkey" FOREIGN KEY (clan_id) REFERENCES clans(id);
ALTER TABLE clan_member_applications ADD CONSTRAINT "clan_member_applications_character_id_fkey" FOREIGN KEY (character_id) REFERENCES characters(id);
ALTER TABLE clan_member_applications ADD CONSTRAINT "clan_member_applications_unique" UNIQUE (character_id, clan_id);
ALTER TABLE clan_stats ADD CONSTRAINT "clan_stats_clans_id_fkey" FOREIGN KEY (clan_id) REFERENCES clans(id);
ALTER TABLE clan_buildings ADD CONSTRAINT "clan_buildings_clans_id_fkey" FOREIGN KEY (clan_id) REFERENCES clans(id);
ALTER TABLE clans ADD CONSTRAINT "clan_name_unique" UNIQUE (name);
ALTER TABLE settings ADD CONSTRAINT "settings_name_unique" UNIQUE (setting_name);
ALTER TABLE schema_information ADD CONSTRAINT "schema_information_name_unique" UNIQUE (file_name);

INSERT INTO schema_information(file_name, date) VALUES ('init.sql', CURRENT_TIMESTAMP);

COMMIT;
