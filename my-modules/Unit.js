import Position from './Position';

export default class Unit {
  constructor(id, x, y) {
    this.id = id;
    this.position = new Position(x, y);
  }

  values() {
    printErr('id: ', this.id);
    printErr('position.x: ', this.position.x);
    printErr('position.y: ', this.position.y);
  }
}
