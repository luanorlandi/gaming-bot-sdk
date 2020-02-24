class CellMap<T> {
  rows: Array<T>;

  width: number;

  height: number;

  constructor(rows: Array<T>) {
    this.rows = rows;
    this.width = rows[0].length;
    this.height = rows.length;
  }
}

export default CellMap;
