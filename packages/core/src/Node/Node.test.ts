import Node from './Node';

describe('Node', () => {
  it('should have id', () => {
    const node: Node = { id: 0, links: new Map() };

    expect(node.id).toBe(0);
  });

  it('should set links', () => {
    const node1: Node = { id: 0, links: new Map() };
    const node2: Node = { id: 1, links: new Map() };

    node1.links.set(1, node2);

    expect(node1.links.get(1)).toBe(node2);
  });
});
