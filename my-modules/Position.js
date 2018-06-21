export default class Position {
  constructor(x = 0, y = 0) {
    this.x = x;
    this.y = y;
  }

  distanceTo = (position) => {
    const x = this.x - position.x;
    const y = this.y - position.y;

    return Math.sqrt((x ** 2) + (y ** 2));
  }
}
