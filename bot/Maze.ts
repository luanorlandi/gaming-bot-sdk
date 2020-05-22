import CellMap from '../packages/CellMap';

export default class Maze extends CellMap<string> {
  private sanityLossLonely: number

  private sanityLossGroup: number

  private wandererSpawnTime: number

  private wandererLifeTime: number

  constructor(
    sanityLossLonely: number,
    sanityLossGroup: number,
    wandererSpawnTim: number,
    wandererLifeTime: number,
    rows: string[][],
  ) {
    super(rows);

    this.sanityLossLonely = sanityLossLonely;
    this.sanityLossGroup = sanityLossGroup;
    this.wandererSpawnTime = wandererSpawnTime;
    this.wandererLifeTime = wandererLifeTime;
  }
}
