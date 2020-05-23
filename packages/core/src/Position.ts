class Position {
  x: number;

  y: number;

  constructor(x = 0, y = 0) {
    this.x = x;
    this.y = y;
  }

  public distanceTo = (position: Position): number => {
    const x = this.x - position.x;
    const y = this.y - position.y;

    return Math.sqrt((x ** 2) + (y ** 2));
  }
}

export default Position;
