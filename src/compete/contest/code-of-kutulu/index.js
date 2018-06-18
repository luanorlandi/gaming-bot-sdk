import constants from './json/constants.json';

const width = parseInt(readline(), 10);
const height = parseInt(readline(), 10);

for (let i = 0; i < height; i += 1) {
  const line = readline();
}

const inputs = readline().split(' ');
const sanityLossLonely = parseInt(inputs[0], 10);
const sanityLossGroup = parseInt(inputs[1], 10);
const wandererSpawnTime = parseInt(inputs[2], 10);
const wandererLifeTime = parseInt(inputs[3], 10);

printErr(constants);

// game loop
while (true) {
  const entityCount = parseInt(readline(), 10);
  for (let i = 0; i < entityCount; i += 1) {
    const inputs = readline().split(' ');
    const entityType = inputs[0];
    const id = parseInt(inputs[1], 10);
    const x = parseInt(inputs[2], 10);
    const y = parseInt(inputs[3], 10);
    const param0 = parseInt(inputs[4], 10);
    const param1 = parseInt(inputs[5], 10);
    const param2 = parseInt(inputs[6], 10);
  }

  print('WAIT');
}
