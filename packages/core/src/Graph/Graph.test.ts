import Graph from './Graph';

const startShape = [
  [11, 6],
  [0, 9],
  [1, 2],
  [0, 1],
  [10, 1],
  [11, 5],
  [2, 3],
  [4, 5],
  [8, 9],
  [6, 7],
  [7, 8],
  [0, 6],
  [3, 4],
  [0, 2],
  [11, 7],
  [0, 8],
  [0, 4],
  [9, 10],
  [0, 5],
  [0, 7],
  [0, 3],
  [0, 10],
  [5, 6],
];

describe('Graph', () => {
  describe('when adding a link', () => {
    it('should add the ids in the graph', () => {
      const graph = new Graph();

      graph.addLink(1, 2);

      expect(graph.nodes.has(1)).toBe(true);
      expect(graph.nodes.has(2)).toBe(true);
    });

    it('should add a link of a node with it self', () => {
      const graph = new Graph();

      graph.addLink(1, 1);

      expect(graph.nodes.get(1).links.has(1)).toBe(true);
    });
  });

  describe('when searching with bfs', () => {
    describe('and when there is a path solution', () => {
      it('should find the path in a graph with two nodes', () => {
        const graph = new Graph();

        graph.addLink(1, 2);

        expect(graph.bfs(1, 2)).toEqual([1, 2]);
      });

      it('should find the path in a graph with a line shape', () => {
        const graph = new Graph();

        graph.addLink(1, 2);
        graph.addLink(2, 3);

        expect(graph.bfs(1, 3)).toEqual([1, 2, 3]);
      });

      it('should find the path in a graph with a triangle shape', () => {
        const graph = new Graph();

        graph.addLink(1, 2);
        graph.addLink(1, 3);
        graph.addLink(2, 4);
        graph.addLink(2, 5);
        graph.addLink(3, 6);
        graph.addLink(3, 7);

        expect(graph.bfs(1, 7)).toEqual([1, 3, 7]);
      });

      it('should find the path in a graph with a start shape', () => {
        const graph = new Graph();

        startShape.forEach((link) => {
          graph.addLink(link[0], link[1]);
        });

        expect(graph.bfs(5, [0])).toEqual([5, 0]);
      });

      it('should find the path in a graph with a triangle shape and with multiple end search', () => {
        const graph = new Graph();

        graph.addLink(1, 2);
        graph.addLink(1, 3);
        graph.addLink(2, 4);
        graph.addLink(2, 5);
        graph.addLink(3, 6);
        graph.addLink(3, 7);

        expect(graph.bfs(1, [5, 7])).toEqual([1, 2, 5]);
      });
    });

    describe('and when there is no path', () => {
      it('should throw an error with starting id not present in the graph', () => {
        const graph = new Graph();

        graph.addLink(1, 2);

        expect(() => graph.bfs(3, 2)).toThrow();
      });

      it('should throw an error with ending id not present in the graph', () => {
        const graph = new Graph();

        graph.addLink(1, 2);

        expect(() => graph.bfs(1, 3)).toThrow();
      });

      it('should return false there is no path', () => {
        const graph = new Graph();

        graph.addLink(1, 2);
        graph.addLink(3, 4);

        expect(graph.bfs(1, 4)).toBe(false);
      });
    });
  });
});
