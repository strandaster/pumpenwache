-- phpMyAdmin SQL Dump
-- version 4.9.4
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Erstellungszeit: 01. Mai 2020 um 21:25
-- Server-Version: 10.0.38-MariaDB-cll-lve
-- PHP-Version: 7.3.6

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Datenbank: `pumpenwacheV2`
--

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `dataV2`
--

CREATE TABLE `dataV2` (
  `Time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Amps` float NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Trigger `dataV2`
--
DELIMITER $$
CREATE TRIGGER `t1_diff` BEFORE INSERT ON `dataV2` FOR EACH ROW BEGIN
       DECLARE lastTimestamp TIMESTAMP;
       DECLARE difference INT(11);
       SELECT time INTO lastTimestamp FROM pumpenwacheV2.dataV2 WHERE time = (SELECT max(time) FROM pumpenwacheV2.dataV2); 
       
       SET difference = TIMESTAMPDIFF(MINUTE, lastTimestamp, CURRENT_TIMESTAMP);
       
       INSERT INTO `diff` (`Time`, `diff`) VALUES (CURRENT_TIMESTAMP, difference);
       
END
$$
DELIMITER ;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `diff`
--

CREATE TABLE `diff` (
  `Time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `diff` int(11) DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Tabellenstruktur für Tabelle `umwelt`
--

CREATE TABLE `umwelt` (
  `Time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `Temperatur` float DEFAULT NULL,
  `Feuchtigkeit` float DEFAULT NULL,
  `Luftdruck` float DEFAULT NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

--
-- Indizes der exportierten Tabellen
--

--
-- Indizes für die Tabelle `dataV2`
--
ALTER TABLE `dataV2`
  ADD UNIQUE KEY `Time` (`Time`);

--
-- Indizes für die Tabelle `diff`
--
ALTER TABLE `diff`
  ADD PRIMARY KEY (`Time`);

--
-- Indizes für die Tabelle `umwelt`
--
ALTER TABLE `umwelt`
  ADD PRIMARY KEY (`Time`);
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
