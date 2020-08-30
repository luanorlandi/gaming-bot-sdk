import { Graph } from '@gladosjs/core';

/* eslint-disable no-console */
/* eslint-disable radix */
/* eslint-disable no-plusplus */
/* eslint-disable no-constant-condition */
declare function readline(): string

const graph = new Graph();

const n = parseInt(readline()); // the number of relationships of influence
for (let i = 0; i < n; i++) {
  const inputs = readline().split(' ');
  const x = parseInt(inputs[0]); // a relationship of influence between two people (x influences y)
  const y = parseInt(inputs[1]);

  graph.addDirectLink(x, y);
}

// Write an answer using console.log()
// To debug: console.error('Debug messages...');


// The number of people involved in the longest succession of influences
const deepth = graph.dfs();
console.log(deepth);
