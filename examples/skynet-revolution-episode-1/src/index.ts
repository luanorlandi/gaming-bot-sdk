import { Graph } from '@gladosjs/core';

/* eslint-disable no-console */
/* eslint-disable radix */
/* eslint-disable no-plusplus */
/* eslint-disable no-constant-condition */
declare function readline(): string

const graph = new Graph();

let inputs = readline().split(' ');
// eslint-disable-next-line @typescript-eslint/no-unused-vars
const N = parseInt(inputs[0]); // the total number of nodes in the level, including the gateways
const L = parseInt(inputs[1]); // the number of links
const E = parseInt(inputs[2]); // the number of exit gateways
for (let i = 0; i < L; i++) {
  inputs = readline().split(' ');
  const N1 = parseInt(inputs[0]); // N1 and N2 defines a link between these nodes
  const N2 = parseInt(inputs[1]);

  graph.addLink(N1, N2);
}
const endIds = [];
for (let i = 0; i < E; i++) {
  const EI = parseInt(readline()); // the index of a gateway node
  endIds.push(EI);
}

// game loop
while (true) {
  // The index of the node on which the Skynet agent is positioned this turn
  const SI = parseInt(readline());

  // Write an action using console.log()
  // To debug: console.error('Debug messages...');

  const path = graph.bfs(SI, endIds);

  if (!path) {
    console.error(`Found no path to ${endIds.toString()}`);
  } else {
    const firstNodeId = path[0];
    const secondNodeId = path[1];

    // Example: 0 1 are the indices of the nodes you wish to sever the link between
    console.log(`${firstNodeId} ${secondNodeId}`);
  }
}
