interface Node {
  id: number
  links: Map<number, Node>
}

export default Node;
