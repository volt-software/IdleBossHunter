START TRANSACTION;

INSERT INTO settings (setting_name, value) VALUES ('xp_gain', '1');
INSERT INTO settings (setting_name, value) VALUES ('money_gain', '1');
INSERT INTO settings (setting_name, value) VALUES ('movement_multiplier', '1');
INSERT INTO settings (setting_name, value) VALUES ('drop_rate_multiplier', '1');
INSERT INTO settings (setting_name, value) VALUES ('maintenance_mode', '0');

INSERT INTO schema_information(file_name, date) VALUES ('seed.sql', CURRENT_TIMESTAMP);

INSERT INTO licenses(license_name, author, license, software_name) VALUES('MIT', 'Sharon W', 'https://github.com/zeroxs/aegis.cpp/blob/b1032278eff88063dd13e727faa5f00ad19cd252/LICENSE', 'parts of aegis.cpp');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('MIT', 'Gabi Melman & spdlog contributors', 'https://github.com/gabime/spdlog/blob/dd38e096b2ae316bb98eb24c0ce65652f1dfe261/LICENSE', 'spdlog');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('MIT', 'Michele Caini', 'https://github.com/skypjack/entt/blob/9e8f2c52a0d4263793a2c4480afccf7d74dcc353/LICENSE', 'entt');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('MIT', 'Victor Zverovich', 'https://github.com/fmtlib/fmt/blob/21a295c272f7ee809774805fd8e31361ac882be2/LICENSE.rst', 'fmt');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('MIT', 'G-Truc Creation', 'yhttps://github.com/g-truc/glm/blob/master/copying.txt', 'glm');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('BSL', 'Christopher M. Kohlhoff', 'https://github.com/boostorg/asio/blob/95f7e936356810addf3a6b54dbd1129d10f3ba98/include/boost/asio.hpp', 'asio');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('BSL', 'Two Blue Cubes Ltd', 'https://github.com/catchorg/Catch2/blob/255aa5f2afe1a622c97422f65ace6ca915be0d8d/LICENSE.txt', 'Catch2');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('BSD', 'Cameron Desrochers', 'https://github.com/cameron314/concurrentqueue/blob/0b410f5edfaec41d4e65509b46c2d3b719c13fe4/LICENSE.md', 'concurrentqueue');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('BSD-3-clause', 'Jeroen T. Vermeulen', 'https://github.com/jtv/libpqxx/blob/6db6e13171c4fa9de98a6a8f2d7fa05aa8f7999f/COPYING', 'libpqxx');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('PostgreSQL license', 'The PostgreSQL Global Development Group & The Regents of the University of California ', 'https://www.postgresql.org/about/licence/', 'PostgreSQL and libpq');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('ISC', 'Frank Denis', 'https://github.com/jedisct1/libsodium/blob/ca4e570f7b035ceeb712e61c714742495900c377/LICENSE', 'libsodium');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('Apache', 'Melissa O''Neill & PCG Project contributors', 'https://github.com/imneme/pcg-cpp/blob/5b5cac8d61339e810c5dbb4692d868a1d7ca1b2d/LICENSE-APACHE.txt', 'pcg-cpp');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('BSL', 'Eric Niebler', 'https://github.com/ericniebler/range-v3/blob/97a5b450e53687dad119dddd19b71dfd2614b6e3/LICENSE.txt', 'range-v3');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('MIT, BSD and JSON licenses', 'THL A29 Limited, a Tencent company, and Milo Yip', 'https://github.com/Tencent/rapidjson/blob/2661a17c7eaede8c881e7455f5a66fd593ed8633/license.txt', 'rapidjson');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('MIT', 'Martin Ankerl', 'https://github.com/martinus/robin-hood-hashing/blob/b21730713f4b5296bec411917c46919f7b38b178/LICENSE', 'robin-hood-hashing');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('custom', 'René Nyffenegger', 'https://github.com/ReneNyffenegger/cpp-base64/blob/a8aae956a2f07df9aac25b064cf4cd92d56aac45/LICENSE', 'base64');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('custom', 'Peter Thorson', 'https://github.com/zaphoyd/websocketpp/blob/72e2760a4cceef2d270450746ce90efce9374eb8/COPYING', 'websocketpp');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('Unlicense', 'Wang Yi', 'https://github.com/wangyi-fudan/wyhash/blob/9f68c1b10166a54c17f55b284c21bd455fd0f7e2/LICENSE', 'wyhash');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('BSD 2-clause', 'Yann Collet', 'https://github.com/Cyan4973/xxHash/blob/7fc2b540470de98d5ad70e327d4400a5c9420cbf/LICENSE', 'xxhash');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('BSD', 'Facebook, Inc', 'https://github.com/facebook/zstd/blob/f5029e285f65860e57edf3685285e1fb8bc92cf9/LICENSE', 'zstd');
INSERT INTO licenses(license_name, author, license, software_name) VALUES('See their website', 'The OpenSSL Project', 'https://www.openssl.org/source/license.html', 'openssl');

COMMIT;