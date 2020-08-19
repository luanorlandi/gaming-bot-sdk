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

  bfs(startId: number, endId: number): number[] | boolean {
    if (!this.nodes.has(startId)) {
      throw new Error(`Graph does not have the node with id ${startId}`);
    }

    if (!this.nodes.has(endId)) {
      throw new Error(`Graph does not have the node with id ${endId}`);
    }

    const unvisitedQueue: Node[] = [];
    const visitedQueue: Map<number, Node> = new Map();
    const previousPath: Map<number, number> = new Map();

    const startingNode = this.nodes.get(startId);
    unvisitedQueue.push(startingNode);

    while (unvisitedQueue.length > 0) {
      const visitingNode = unvisitedQueue.shift();

      if (visitingNode.id === endId) {
        const path = [endId];
        let previousId = endId;

        while (previousId !== startId) {
          previousId = previousPath.get(previousId);

          if (!previousId) {
            throw new Error('Failed to create the path found');
          }

          path.unshift(previousId);
        }

        return path;
      }

      visitedQueue.set(visitingNode.id, visitingNode);
      visitingNode.links.forEach((childNode) => {
        if (visitedQueue.has(childNode.id)) {
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
