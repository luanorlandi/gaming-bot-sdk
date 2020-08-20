import { debugMap } from '../utils/debug';
import Node from '../Node/Node';

class Graph {
  readonly nodes: Map<number, Node> = new Map()

  addLink(id1: number, id2: number): void {
    const node1 = this.nodes.get(id1) || { id: id1, links: new Map() } as Node;
    const node2 = this.nodes.get(id2) || { id: id2, links: new Map() } as Node;

    node1.links.set(id2, node2);
    node2.links.set(id1, node1);

    this.nodes.set(id1, node1);
    this.nodes.set(id2, node2);
  }

  removeLink(id1: number, id2: number): void {
    if (this.nodes.has(id1)) {
      const node1 = this.nodes.get(id1);
      node1.links.delete(id2);
    }

    if (this.nodes.has(id2)) {
      const node2 = this.nodes.get(id2);
      node2.links.delete(id1);
    }
  }

  bfs(startId: number, endId: number | number[]): number[] | false {
    // normalize input
    let endIdArray;
    if (typeof endId === 'number') {
      endIdArray = [endId];
    } else {
      endIdArray = endId;
    }

    // check if the graph has the node ids
    if (typeof endIdArray === 'object' && endIdArray.length === 0) {
      throw new Error('Search needs at least one ending node id in the array');
    }

    if (!this.nodes.has(startId)) {
      throw new Error(`Graph does not have the node with id ${startId}`);
    }

    const hasEndId = endIdArray.some((id) => this.nodes.has(id));

    if (!hasEndId) {
      throw new Error(`Graph does not have the node with id ${endId.toString()}`);
    }

    const unvisitedQueue: Node[] = [];
    const visitedQueue: Map<number, Node> = new Map();
    const previousPath: Map<number, number> = new Map();

    const startingNode = this.nodes.get(startId);
    unvisitedQueue.push(startingNode);

    while (unvisitedQueue.length > 0) {
      const visitingNode = unvisitedQueue.shift();

      if (endIdArray.includes(visitingNode.id)) {
        let previousId = visitingNode.id;
        const endIdFound = visitingNode.id;
        const path = [];
        path.push(previousId);

        // build path found from the start to the end
        while (previousId !== startId) {
          previousId = previousPath.get(previousId);

          if (typeof previousId === 'undefined') {
            throw new Error(`Failed to create the path found from ${startId} to ${endIdFound}. Map: ${debugMap(previousPath)}`);
          }

          path.unshift(previousId);
        }

        return path;
      }

      visitedQueue.set(visitingNode.id, visitingNode);
      visitingNode.links.forEach((childNode) => {
        const hasVisited = visitedQueue.has(childNode.id);
        const willVisited = unvisitedQueue.find((unvisitedNode) => (
          unvisitedNode.id === childNode.id
        ));
        if (hasVisited || willVisited) {
          return;
        }

        unvisitedQueue.push(childNode);
        previousPath.set(childNode.id, visitingNode.id);
      });
    }

    return false;
  }
}

export default Graph;
