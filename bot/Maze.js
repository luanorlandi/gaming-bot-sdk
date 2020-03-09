import CellMap from '../packages/CellMap';

export default class Maze extends CellMap {
  constructor(
    sanityLossLonely,
    sanityLossGroup,
    wandererSpawnTime,
    wandererLifeTime,
    rows,
  ) {
    super(rows);

    this.sanityLossLonely;
    this.sanityLossGroup;
    this.wandererSpawnTime;
    this.wandererLifeTime;
  }
}
