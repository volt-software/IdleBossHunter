START TRANSACTION;

INSERT INTO settings (setting_name, value) VALUES ('xp_gain', '1');
INSERT INTO settings (setting_name, value) VALUES ('money_gain', '1');
INSERT INTO settings (setting_name, value) VALUES ('movement_multiplier', '1');
INSERT INTO settings (setting_name, value) VALUES ('drop_rate_multiplier', '1');
INSERT INTO settings (setting_name, value) VALUES ('maintenance_mode', '0');

INSERT INTO schema_information(file_name, date) VALUES ('seed.sql', CURRENT_TIMESTAMP);

COMMIT;