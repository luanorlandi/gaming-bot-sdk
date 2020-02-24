import Position from './Position';

class Unit {
  id: number;

  position: Position;

  constructor(id: number, x: number, y: number) {
    this.id = id;
    this.position = new Position(x, y);
  }

  public closestUnit = (units: Array<Unit>): Unit => (
    units.reduce((closest, unit) => {
      const distanceA = this.position.distanceTo(unit.position);
      const distanceB = this.position.distanceTo(closest.position);
      return distanceA < distanceB ? unit : closest;
    })
  );
}

export default Unit;
