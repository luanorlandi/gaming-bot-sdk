import distance from '../../../../my-modules/Position';

const numSites = parseInt(readline(), 10);
for (let i = 0; i < numSites; i += 1) {
  const inputs = readline().split(' ');
  const siteId = parseInt(inputs[0], 10);
  const x = parseInt(inputs[1], 10);
  const y = parseInt(inputs[2], 10);
  const radius = parseInt(inputs[3], 10);
}

const isGameRunning = true;

// game loop
while (isGameRunning) {
  const inputs = readline().split(' ');
  const gold = parseInt(inputs[0], 10);
  const touchedSite = parseInt(inputs[1], 10); // -1 if none
  for (let i = 0; i < numSites; i += 1) {
    const sites = readline().split(' ');
    const siteId = parseInt(sites[0], 10);
    const ignore1 = parseInt(sites[1], 10); // used in future leagues
    const ignore2 = parseInt(sites[2], 10); // used in future leagues
    const structureType = parseInt(sites[3], 10); // -1 = No structure, 2 = Barracks
    const owner = parseInt(sites[4], 10); // -1 = No structure, 0 = Friendly, 1 = Enemy
    const param1 = parseInt(sites[5], 10);
    const param2 = parseInt(sites[6], 10);
  }
  const numUnits = parseInt(readline(), 10);
  for (let i = 0; i < numUnits; i += 1) {
    const inputs = readline().split(' ');
    const x = parseInt(inputs[0], 10);
    const y = parseInt(inputs[1], 10);
    const owner = parseInt(inputs[2], 10);
    const unitType = parseInt(inputs[3], 10); // -1 = QUEEN, 0 = KNIGHT, 1 = ARCHER
    const health = parseInt(inputs[4], 10);
  }

  // Write an action using print()
  // To debug: printErr('Debug messages...');


  // First line: A valid queen action
  // Second line: A set of training instructions
  print('WAIT');
  print('TRAIN');
}
