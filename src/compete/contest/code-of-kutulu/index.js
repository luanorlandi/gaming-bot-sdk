import constants from './constants.json';
import Explorer from './Explorer';

parseInt(readline(), 10);
const height = parseInt(readline(), 10);
const rows = [];

for (let i = 0; i < height; i += 1) {
  const row = readline();
  rows.push(row);
}

const map = new Map(rows);

const inputs = readline().split(' ');
const sanityLossLonely = parseInt(inputs[0], 10);
const sanityLossGroup = parseInt(inputs[1], 10);
const wandererSpawnTime = parseInt(inputs[2], 10);
const wandererLifeTime = parseInt(inputs[3], 10);

printErr(inputs);
printErr(sanityLossLonely);
printErr(sanityLossGroup);
printErr(wandererSpawnTime);
printErr(wandererLifeTime);

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

    const explorer = new Explorer(id, x, y);
    explorer.values();
  }

  print('WAIT');
}
