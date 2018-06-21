import Position from './Position';

export default class Unit {
  constructor(id, x, y) {
    this.id = id;
    this.position = new Position(x, y);
  }

  closestUnit = units =>
    units.reduce((closest, unit) => {
      const distanceA = this.position.distanceTo(unit.position);
      const distanceB = this.position.distanceTo(closest.position);
      return distanceA < distanceB ? unit : closest;
    });
}
