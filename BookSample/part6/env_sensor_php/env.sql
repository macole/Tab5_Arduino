CREATE TABLE `env` (
  `id` int(11) NOT NULL,
  `date` datetime NOT NULL,
  `temparature` float NOT NULL,
  `humidity` float NOT NULL,
  `pressure` float NOT NULL,
  `air` float NOT NULL
);
ALTER TABLE `env`
  ADD PRIMARY KEY (`id`);
ALTER TABLE `env`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT;
