SET SESSION sql_mode = "";

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for ikashop_notification
-- ----------------------------
DROP TABLE IF EXISTS `ikashop_notification`;
CREATE TABLE `ikashop_notification` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `owner` int unsigned NOT NULL,
  `type` int unsigned NOT NULL,
  `who` varchar(50) NOT NULL,
  `what` int unsigned NOT NULL,
  `format` varchar(50) NOT NULL,
  `seen` tinyint NOT NULL DEFAULT '0',
  `datetime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
);

-- ----------------------------
-- Table structure for ikashop_offlineshop
-- ----------------------------
DROP TABLE IF EXISTS `ikashop_offlineshop`;
CREATE TABLE `ikashop_offlineshop` (
  `owner` int unsigned NOT NULL,
  `duration` int unsigned NOT NULL DEFAULT '0',
  `map` int unsigned NOT NULL DEFAULT '0',
  `x` int unsigned NOT NULL DEFAULT '0',
  `y` int unsigned NOT NULL DEFAULT '0',
  `decoration` int unsigned NOT NULL DEFAULT '0',
  `decoration_time` int unsigned NOT NULL DEFAULT '0',
  `lock_index` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`owner`)
);

-- ----------------------------
-- Table structure for ikashop_safebox
-- ----------------------------
DROP TABLE IF EXISTS `ikashop_safebox`;
CREATE TABLE `ikashop_safebox` (
  `owner` int unsigned NOT NULL,
  `gold` bigint unsigned NOT NULL,
  `cheque` int unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`owner`)
);

-- ----------------------------
-- Table structure for ikashop_sale
-- ----------------------------
DROP TABLE IF EXISTS `ikashop_sale`;
CREATE TABLE `ikashop_sale` (
  `id` int unsigned NOT NULL,
  `vnum` int unsigned NOT NULL,
  `count` int unsigned NOT NULL,
  `price` bigint unsigned NOT NULL,
  `datetime` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP
);
SET FOREIGN_KEY_CHECKS=1;
