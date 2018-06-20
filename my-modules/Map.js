export default class Map {
  constructor(rows) {
    this.rows = rows;
    this.width = rows[0].length;
    this.height = rows.length;
  }
}
