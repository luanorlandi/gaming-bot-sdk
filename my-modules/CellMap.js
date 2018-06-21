export default class CellMap {
  constructor(rows) {
    this.rows = rows;
    this.width = rows[0].length;
    this.height = rows.length;
  }
}
