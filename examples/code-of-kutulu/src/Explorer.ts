import { Unit } from '@gladosjs/core';

export default class Explorer extends Unit {
  private param0: number

  private param1: number

  private param2: number

  constructor(
    id: number,
    x: number,
    y: number,
    param0: number,
    param1: number,
    param2: number,
  ) {
    super(id, x, y);

    this.param0 = param0;
    this.param1 = param1;
    this.param2 = param2;
  }
}
